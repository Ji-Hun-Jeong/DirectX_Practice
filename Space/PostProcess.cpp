#include "pch.h"
#include "PostProcess.h"
#include "GeometryGenerator.h"
#include "D3DUtils.h"
#include "ImageFilter.h"
#include "Scene.h"
#include "SceneMgr.h"
PostProcess::PostProcess(float width, float height, UINT bloomLevel,
	ComPtr<ID3D11ShaderResourceView>& srv, ComPtr<ID3D11RenderTargetView>& rtv)
	: m_originalSRV(srv)
	, m_finalRTV(rtv)
	, m_fWidth(width)
	, m_fHeight(height)
{
	MeshData squareData = GeometryGenerator::MakeSquare();
	Mesh::Init(squareData, L"Copy", L"Copy");
	D3DUtils& dx = D3DUtils::GetInst();
	dx.CreatePixelShader(L"Blur", m_blurShader);
	dx.CreatePixelShader(L"Combine", m_combineShader);

	D3D11_RASTERIZER_DESC rastDesc;
	ZeroMemory(&rastDesc, sizeof(D3D11_RASTERIZER_DESC)); // Need this
	rastDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
	rastDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_NONE;
	rastDesc.FrontCounterClockwise = false;
	rastDesc.DepthClipEnable = false;

	D3DUtils::GetInst().GetDevice()->CreateRasterizerState(&rastDesc, m_rss.GetAddressOf());

	CreateFilters(bloomLevel);
}

void PostProcess::Update(float dt)
{
	for (auto& filter : m_filters)
	{
		if (filter)
			filter->Update(dt);
	}
}

void PostProcess::Render(ComPtr<ID3D11DeviceContext>& context)
{
	context->RSSetState(m_rss.Get());
	for (auto& filter : m_filters)
	{
		if (filter)
			filter->Render(context);
	}
}

void PostProcess::CreateFilters(UINT bloomLevel)
{
	GETCURSCENE()->m_pixelConstantData.bloom.dx = 1.0f / m_fWidth;
	GETCURSCENE()->m_pixelConstantData.bloom.dy = 1.0f / m_fHeight;
	// imagefilter가 postprocess를 가지는형식으로
	auto copyFilter = make_shared<ImageFilter>(this, m_vertexShader, m_pixelShader, m_fWidth, m_fHeight);
	copyFilter->SetShaderResourceViews({ m_originalSRV.Get() });
	m_filters.push_back(copyFilter);

	for (int i = 1; i <= bloomLevel; ++i)
	{
		const int divideValue = pow(2, i);
		const int width = int(m_fWidth) / divideValue;
		const int height = int(m_fHeight) / divideValue;

		auto blurXFilter = make_shared<ImageFilter>(this, m_vertexShader, m_blurShader, width, height);
		blurXFilter->SetShaderResourceViews({ m_filters.back()->GetShaderResourceView().Get() });
		m_filters.push_back(blurXFilter);
	}

	for (int i = bloomLevel - 1; i >= 0; --i)
	{
		const int divideValue = pow(2, i);
		const int width = int(m_fWidth) / divideValue;
		const int height = int(m_fHeight) / divideValue;

		auto blurXFilter = make_shared<ImageFilter>(this, m_vertexShader, m_blurShader, width, height);
		blurXFilter->SetShaderResourceViews({ m_filters.back()->GetShaderResourceView().Get() });
		m_filters.push_back(blurXFilter);

	}

	auto combineFilter = make_shared<ImageFilter>(this, m_vertexShader, m_combineShader, m_fWidth, m_fHeight);
	combineFilter->SetShaderResourceViews({ copyFilter->GetShaderResourceView().Get()
		,m_filters.back()->GetShaderResourceView().Get() });
	combineFilter->SetRenderTargetViews({ m_finalRTV.Get() });
	m_filters.push_back(combineFilter);
}
