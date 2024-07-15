#include "pch.h"
#include "PostProcess.h"
#include "GeometryGenerator.h"
#include "D3DUtils.h"
#include "ImageFilter.h"
#include "Scene.h"
#include "SceneMgr.h"
#include "GraphicsCommons.h"
PostProcess::PostProcess(float width, float height, UINT bloomLevel,
	ComPtr<ID3D11ShaderResourceView>& srv, ComPtr<ID3D11RenderTargetView>& rtv)
	: m_originalSRV(srv)
	, m_finalRTV(rtv)
	, m_fWidth(width)
	, m_fHeight(height)
{
	MeshData squareData = GeometryGenerator::MakeSquare();
	D3DUtils::GetInst().CreateVertexBuffer<Vertex>(squareData.vertices, m_vertexBuffer);
	m_indexCount = squareData.indices.size();
	D3DUtils::GetInst().CreateIndexBuffer<uint32_t>(squareData.indices, m_indexBuffer);

	m_vertexShader = Graphics::g_basicVS;
	m_blurShader = Graphics::g_blurPS;
	m_combineShader = Graphics::g_combinePS;
	m_copyShader = Graphics::g_copyPS;

	m_constData.dx = 1.0f / (SceneMgr::GetInst().m_fWidth);
	m_constData.dy = 1.0f / (SceneMgr::GetInst().m_fHeight);
	m_constData.strength = 0.0f;
	m_constData.threshold = 1.0f;

	D3DUtils::GetInst().CreateConstantBuffer<ImageFilterConstData>(m_constData, m_constBuffer);
	CreateFilters(bloomLevel);
}

void PostProcess::Update(float dt)
{
	D3DUtils::GetInst().UpdateBuffer<ImageFilterConstData>(m_constData, m_constBuffer);
}

void PostProcess::Render(ComPtr<ID3D11DeviceContext>& context)
{
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	context->IASetVertexBuffers(0, 1, m_vertexBuffer.GetAddressOf(), &stride, &offset);
	context->IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	
	context->PSSetConstantBuffers(2, 1, m_constBuffer.GetAddressOf());
	for (auto& filter : m_filters)
	{
		if (filter)
			filter->Render(context);
	}
}

void PostProcess::CreateFilters(UINT bloomLevel)
{
	// imagefilter가 postprocess를 가지는형식으로
	auto copyFilter = make_shared<ImageFilter>(this, m_copyShader, m_fWidth, m_fHeight);
	copyFilter->SetShaderResourceViews({ m_originalSRV.Get() });
	m_filters.push_back(copyFilter);

	for (int i = 1; i <= bloomLevel; ++i)
	{
		const int divideValue = pow(2, i);
		const int width = int(m_fWidth) / divideValue;
		const int height = int(m_fHeight) / divideValue;

		auto blurXFilter = make_shared<ImageFilter>(this, m_blurShader, width, height);
		blurXFilter->SetShaderResourceViews({ m_filters.back()->GetShaderResourceView().Get() });
		m_filters.push_back(blurXFilter);
	}

	for (int i = bloomLevel - 1; i >= 0; --i)
	{
		const int divideValue = pow(2, i);
		const int width = int(m_fWidth) / divideValue;
		const int height = int(m_fHeight) / divideValue;

		auto blurXFilter = make_shared<ImageFilter>(this, m_blurShader, width, height);
		blurXFilter->SetShaderResourceViews({ m_filters.back()->GetShaderResourceView().Get() });
		m_filters.push_back(blurXFilter);

	}

	auto combineFilter = make_shared<ImageFilter>(this, m_combineShader, m_fWidth, m_fHeight);
	combineFilter->SetShaderResourceViews({ copyFilter->GetShaderResourceView().Get()
		,m_filters.back()->GetShaderResourceView().Get() });
	combineFilter->SetRenderTargetViews({ m_finalRTV.Get() });
	m_filters.push_back(combineFilter);
}
