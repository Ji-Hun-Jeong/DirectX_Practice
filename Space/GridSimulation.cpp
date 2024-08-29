#include "pch.h"
#include "GridSimulation.h"
#include "KeyMgr.h"
#include "D3DUtils.h"

GridSimulation::GridSimulation(SceneMgr* pOwner)
	: AnimateScene(pOwner)
	, m_constData{}
	, m_density{}
	, m_velocity{}
{
}


void GridSimulation::Enter()
{
	auto d3dUtilsInst = D3DUtils::GetInst();
	d3dUtilsInst.CreateComputeShader(L"GridDensity", m_densityCS);
	d3dUtilsInst.CreateComputeShader(L"GridAdvection", m_advectionCS);
	d3dUtilsInst.CreateComputeShader(L"GridDivergence", m_divergenceCS);
	d3dUtilsInst.CreateComputeShader(L"GridJacobi", m_jacobiCS);
	d3dUtilsInst.CreateComputeShader(L"GridApplyPressure", m_applyPressureCS);

	m_density.Init(UINT(m_pOwner->m_fWidth), UINT(m_pOwner->m_fHeight),
		DXGI_FORMAT_R16G16B16A16_FLOAT);

	m_densityTemp.Init(UINT(m_pOwner->m_fWidth), UINT(m_pOwner->m_fHeight),
		DXGI_FORMAT_R16G16B16A16_FLOAT);

	m_velocity.Init(UINT(m_pOwner->m_fWidth), UINT(m_pOwner->m_fHeight),
		DXGI_FORMAT_R16G16_FLOAT);

	m_velocityTemp.Init(UINT(m_pOwner->m_fWidth), UINT(m_pOwner->m_fHeight),
		DXGI_FORMAT_R16G16_FLOAT);

	m_divergence.Init(UINT(m_pOwner->m_fWidth), UINT(m_pOwner->m_fHeight),
		DXGI_FORMAT_R16_FLOAT);

	m_pressure.Init(UINT(m_pOwner->m_fWidth), UINT(m_pOwner->m_fHeight),
		DXGI_FORMAT_R16_FLOAT);

	m_pressureTemp.Init(UINT(m_pOwner->m_fWidth), UINT(m_pOwner->m_fHeight),
		DXGI_FORMAT_R16_FLOAT);

	d3dUtilsInst.CreateConstantBuffer<GridConst>(m_constData, m_constBuffer);
}


void GridSimulation::Update(float dt)
{
	m_constData.dt = dt;
	static const float densityColor[][4] =
	{
		  {1.0f,0.0f,0.0f,1.0f}
		, {0.0f,1.0f,0.0f,1.0f}
		, {0.0f,0.0f,1.0f,1.0f}
		, {1.0f,0.0f,1.0f,1.0f}
		, {0.0f,1.0f,1.0f,1.0f}
		, {1.0f,1.0f,0.0f,1.0f}
	};

	if (KEYCHECK(LBUTTON, HOLD))
	{
		m_curMousePos = KeyMgr::GetInst().GetMouseScreenPos();

		m_constData.mouseX = static_cast<UINT>(m_curMousePos.x);
		m_constData.mouseY = static_cast<UINT>(m_curMousePos.y);
		m_constData.velocity = m_curMousePos - m_prevMousePos;

		D3DUtils::GetInst().UpdateBuffer<GridConst>(m_constData, m_constBuffer);

		m_prevMousePos = m_curMousePos;
	}
	else if (KEYCHECK(LBUTTON, TAP))
	{
		m_curMousePos = KeyMgr::GetInst().GetMouseScreenPos();
		m_prevMousePos = m_curMousePos;
		static int mouseClickCount = 0;
		memcpy(&m_constData.densityColor.x, densityColor[mouseClickCount++ % 6], sizeof(float) * 4);
	}
	else
	{
		m_constData.mouseX = 9000;
		m_constData.mouseY = 9000;
		m_constData.velocity = Vector2(0.0f, 0.0f);
		D3DUtils::GetInst().UpdateBuffer<GridConst>(m_constData, m_constBuffer);
	}
}

void GridSimulation::Render(ComPtr<ID3D11DeviceContext>& context, bool drawWireFrame)
{
	float clearColor[4] = { 0.0f,0.0f,0.0f,1.0f };
	//context->ClearRenderTargetView(m_stagingBuffer->GetRTV().Get(), clearColor);

	ID3D11SamplerState* samplers[] =
	{
		Graphics::g_linearSampler.Get(),
		Graphics::g_clampSampler.Get()
	};
	context->CSSetSamplers(0, sizeof(samplers) / sizeof(ID3D11SamplerState*), samplers);

	// 색을 뿌리고 속도를 만든다.
	{
		context->CSSetShader(m_densityCS.Get(), nullptr, 0);
		ID3D11UnorderedAccessView* uav[2] =
		{
			m_density.GetUAV().Get(),
			m_velocity.GetUAV().Get()
		};
		context->CSSetUnorderedAccessViews(0, sizeof(uav) / sizeof(ID3D11UnorderedAccessView*)
			, uav, nullptr);
		context->CSSetConstantBuffers(0, 1, m_constBuffer.GetAddressOf());

		context->Dispatch(UINT(ceil(m_pOwner->m_fWidth / 32.0f))
			, UINT(ceil(m_pOwner->m_fHeight / 32.0f)), 1);

		this->ComputeShaderBarrier(context);

		context->CopyResource(m_densityTemp.GetTexture().Get()
			, m_density.GetTexture().Get());
		context->CopyResource(m_velocityTemp.GetTexture().Get()
			, m_velocity.GetTexture().Get());
	}

	// 속도를 이용하여 Divergence를 계산한다.
	{
		context->CSSetShader(m_divergenceCS.Get(), nullptr, 0);
		ID3D11UnorderedAccessView* uav[] =
		{
			m_velocity.GetUAV().Get(),
			m_divergence.GetUAV().Get(),
			m_pressure.GetUAV().Get(),
			m_pressureTemp.GetUAV().Get()
		};
		context->CSSetUnorderedAccessViews(0, sizeof(uav) / sizeof(ID3D11UnorderedAccessView*)
			, uav, nullptr);

		context->Dispatch(UINT(ceil(m_pOwner->m_fWidth / 32.0f))
			, UINT(ceil(m_pOwner->m_fHeight / 32.0f)), 1);

		ComputeShaderBarrier(context);
	}

	// Divergence를 이용하여 압력을 구한다.
	{
		context->CSSetShader(m_jacobiCS.Get(), nullptr, 0);
		ID3D11UnorderedAccessView* evenUav[] =
		{
			m_divergence.GetUAV().Get(),
			m_pressure.GetUAV().Get(),
			m_pressureTemp.GetUAV().Get()
		};
		ID3D11UnorderedAccessView* oddUav[] =
		{
			m_divergence.GetUAV().Get(),
			m_pressureTemp.GetUAV().Get(),
			m_pressure.GetUAV().Get()
		};
		for (int i = 0; i < 10; ++i)
		{
			if (i % 2 == 0)
			{
				context->CSSetUnorderedAccessViews(0, sizeof(evenUav) / sizeof(ID3D11UnorderedAccessView*)
					, evenUav, nullptr);
			}
			else
			{
				context->CSSetUnorderedAccessViews(0, sizeof(oddUav) / sizeof(ID3D11UnorderedAccessView*)
					, oddUav, nullptr);
			}

			context->Dispatch(UINT(ceil(m_pOwner->m_fWidth / 32.0f))
				, UINT(ceil(m_pOwner->m_fHeight / 32.0f)), 1);

			ComputeShaderBarrier(context);
		}
	}

	// 압력을 이용하여 속도를 바꾼다.
	{
		context->CSSetShader(m_applyPressureCS.Get(), nullptr, 0);
		ID3D11UnorderedAccessView* uav[] =
		{
			m_velocityTemp.GetUAV().Get(),
			m_pressure.GetUAV().Get()
		};
		context->CSSetUnorderedAccessViews(0, sizeof(uav) / sizeof(ID3D11UnorderedAccessView*)
			, uav, nullptr);

		context->Dispatch(UINT(ceil(m_pOwner->m_fWidth / 32.0f))
			, UINT(ceil(m_pOwner->m_fHeight / 32.0f)), 1);

		ComputeShaderBarrier(context);
	}

	// 구한 속도를 이용하여 위치를 바꾼다.
	{
		context->CSSetShader(m_advectionCS.Get(), nullptr, 0);
		ID3D11UnorderedAccessView* uav[2] =
		{
			m_density.GetUAV().Get(),
			m_velocity.GetUAV().Get()
		};
		context->CSSetUnorderedAccessViews(0, sizeof(uav) / sizeof(ID3D11UnorderedAccessView*)
			, uav, nullptr);

		ID3D11ShaderResourceView* srv[] =
		{
			m_densityTemp.GetSRV().Get(),
			m_velocityTemp.GetSRV().Get()
		};
		context->CSSetShaderResources(0, sizeof(srv) / sizeof(ID3D11ShaderResourceView*)
			, srv);

		context->Dispatch(UINT(ceil(m_pOwner->m_fWidth / 32.0f))
			, UINT(ceil(m_pOwner->m_fHeight / 32.0f)), 1);

		this->ComputeShaderBarrier(context);
	}

	context->CopyResource(m_pOwner->GetBackBufferTexture().Get(), m_densityTemp.GetTexture().Get());
}
