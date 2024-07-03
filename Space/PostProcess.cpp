#include "pch.h"
#include "PostProcess.h"
#include "GeometryGenerator.h"
#include "D3DUtils.h"
#include "ImageFilter.h"
#include "Scene.h"
#include "SceneMgr.h"
PostProcess::PostProcess(float width, float height, ComPtr<ID3D11ShaderResourceView>& srv, ComPtr<ID3D11RenderTargetView>& rtv)
	: m_originalSRV(srv)
	, m_finalRTV(rtv)
	, m_fWidth(width)
	, m_fHeight(height)
{
	MeshData squareData = GeometryGenerator::MakeSquare();
	Mesh::Init(squareData, L"Copy", L"Copy");
	D3DUtils& dx = D3DUtils::GetInst();
	dx.CreatePixelShader(L"BlurX", m_blurXShader);
	dx.CreatePixelShader(L"BlurY", m_blurYShader);
	dx.CreatePixelShader(L"Bloom", m_bloomShader);
	dx.CreatePixelShader(L"Combine", m_combineShader);

	CreateFilters();
}

void PostProcess::Update(float dt)
{
	for (auto& filter : m_filters)
	{
		if (filter)
			filter->Update(dt);
	}
}

void PostProcess::Render(ID3D11DeviceContext* context)
{
	for (auto& filter : m_filters)
	{
		if (filter)
			filter->Render(context);
	}
}

void PostProcess::CreateFilters()
{
	GETCURSCENE()->m_pixelConstantData.bloom.dx = 1.0f / m_fWidth;
	GETCURSCENE()->m_pixelConstantData.bloom.dy = 1.0f / m_fHeight;
	// imagefilter가 postprocess를 가지는형식으로
	auto copyFilter = make_shared<ImageFilter>(this);
	copyFilter->SetShaderResourceViews({ m_originalSRV.Get() });
	copyFilter->SetRenderTargetViews({ m_finalRTV.Get() });
	m_filters.push_back(copyFilter);
}
