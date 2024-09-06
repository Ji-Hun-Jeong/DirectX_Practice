#include "pch.h"
#include "CurlNoise.h"
#include "D3DUtils.h"

CurlNoise::CurlNoise(SceneMgr* pOwner)
	: AnimateScene(pOwner)
{
	vector<D3D11_INPUT_ELEMENT_DESC> v =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	D3D11_BLEND_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.AlphaToCoverageEnable = true; // MSAA
	bd.IndependentBlendEnable = false;
	bd.RenderTarget[0].BlendEnable = true;
	bd.RenderTarget[0].SrcBlend = D3D11_BLEND_BLEND_FACTOR;	// <- µÎ »öÀ» ÀüºÎ ¾¸
	bd.RenderTarget[0].DestBlend = D3D11_BLEND_BLEND_FACTOR; // INV ¾Æ´Ô
	bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	bd.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	bd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
	bd.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	bd.RenderTarget[0].RenderTargetWriteMask =
		D3D11_COLOR_WRITE_ENABLE_ALL;

	auto& d3dUtils = D3DUtils::GetInst();
	d3dUtils.CreateVertexShaderAndInputLayout(L"CurlNoise", m_curlNoiseVS, v, m_curlNoiseIL);
	d3dUtils.CreateGeometryShader(L"CurlNoise", m_curlNoiseGS);
	d3dUtils.CreatePixelShader(L"CurlNoise", m_curlNoisePS);
	d3dUtils.CreateComputeShader(L"CurlNoise", m_curlNoiseCS);
	d3dUtils.CreateBlendState(&bd, m_curlNoiseBS);

	m_stagingBuffer.Init(UINT(m_pOwner->m_fWidth), UINT(m_pOwner->m_fHeight),
		DXGI_FORMAT_R16G16B16A16_FLOAT);

	random_device rd;
	mt19937 gen(rd());
	uniform_real_distribution<float> randomPos(-1.0f, 1.0f);
	uniform_int_distribution<uint32_t> randomColor(0, 6);
	vector<Vector3> rainbow =
	{
		{1.0f, 0.0f, 0.0f},  // Red
		{1.0f, 0.65f, 0.0f}, // Orange
		{1.0f, 1.0f, 0.0f},  // Yellow
		{0.0f, 1.0f, 0.0f},  // Green
		{0.0f, 0.0f, 1.0f},  // Blue
		{0.3f, 0.0f, 0.5f},  // Indigo
		{0.5f, 0.0f, 1.0f}   // Violet/Purple
	};

	auto& vec = m_particle.GetVec();
	vec.resize(100);

	for (auto& p : vec)
	{
		p.pos = Vector3(randomPos(rd), randomPos(rd), 0.0f);
		p.color = rainbow[randomColor(gen)];
	}

	m_particle.Init();

}

void CurlNoise::Update(float dt)
{
}

void CurlNoise::Render(ComPtr<ID3D11DeviceContext>& context, bool drawWireFrame)
{
	{
		context->CSSetShader(m_curlNoiseCS.Get(), nullptr, 0);
		
		ID3D11UnorderedAccessView* uav[] =
		{
			m_stagingBuffer.GetUAV().Get(),
			m_particle.GetUAV()
		};
		context->CSSetUnorderedAccessViews(0, 2, uav, nullptr);
		context->Dispatch(UINT(ceil(m_pOwner->m_fWidth / 32.0f))
			, UINT(ceil(m_pOwner->m_fHeight / 32.0f)), 1);

		AnimateScene::ComputeShaderBarrier(context);
	}
	context->CSSetShader(nullptr, nullptr, 0);
	context->RSSetViewports(1, &m_pOwner->GetViewPort());
	context->OMSetRenderTargets(1, m_stagingBuffer.GetRTV().GetAddressOf(), nullptr);

	context->IASetInputLayout(m_curlNoiseIL.Get());
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	context->VSSetShader(m_curlNoiseVS.Get(), nullptr, 0);
	context->VSSetShaderResources(0, 1, m_particle.GetAddressSRV());

	context->GSSetShader(m_curlNoiseGS.Get(), nullptr, 0);

	context->RSSetState(Graphics::g_solidCWRS.Get());

	context->PSSetShader(m_curlNoisePS.Get(), nullptr, 0);

	const float f = 1.0f;
	const float blendFactor[4] = { f,f,f,1.0f };
	context->OMSetBlendState(m_curlNoiseBS.Get(), blendFactor, 0xff);

	context->Draw(m_particle.GetBufferSize(), 0);

	context->CopyResource(m_pOwner->GetBackBufferTexture().Get(), m_stagingBuffer.GetTexture().Get());
}

void CurlNoise::Enter()
{
	
}
