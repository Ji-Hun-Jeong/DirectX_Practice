#include "pch.h"
#include "ImageFilter.h"
#include "D3DUtils.h"
#include "GeometryGenerator.h"
#include "Scene.h"
#include "SceneMgr.h"
#include "PostProcess.h"

ImageFilter::ImageFilter(PostProcess* owner, ComPtr<ID3D11PixelShader>& ps
	, float width, float height)
	: m_pOwner(owner)
	, m_fWidth(width)
	, m_fHeight(height)
{
	m_pixelShader = ps;

	ComPtr<ID3D11Device> device = D3DUtils::GetInst().GetDevice();
	D3D11_TEXTURE2D_DESC textureDesc;
	ZeroMemory(&textureDesc, sizeof(textureDesc));
	textureDesc.Width = UINT(m_fWidth);
	textureDesc.Height = UINT(m_fHeight);
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;
	textureDesc.SampleDesc.Count = 1;
	HRESULT result = device->CreateTexture2D(&textureDesc, nullptr, m_texture.GetAddressOf());
	CHECKRESULT(result);
	result = device->CreateShaderResourceView(m_texture.Get(), nullptr, m_shaderResourceView.GetAddressOf());
	CHECKRESULT(result);
	result = device->CreateRenderTargetView(m_texture.Get(), nullptr, m_renderTargetView.GetAddressOf());
	CHECKRESULT(result);

	ZeroMemory(&m_viewPort, sizeof(D3D11_VIEWPORT));
	m_viewPort.TopLeftX = 0;
	m_viewPort.TopLeftY = 0;
	m_viewPort.Width = float(m_fWidth);
	m_viewPort.Height = float(m_fHeight);
	m_viewPort.MinDepth = 0.0f;
	m_viewPort.MaxDepth = 1.0f;

	SetRenderTargetViews({ m_renderTargetView.Get() });
}


void ImageFilter::Render(ComPtr<ID3D11DeviceContext>& context)
{
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	context->OMSetRenderTargets(UINT(m_anotherRTVs.size()), m_anotherRTVs.data(), nullptr);

	context->RSSetViewports(1, &m_viewPort);

	context->PSSetShader(m_pixelShader.Get(), nullptr, 0);
	context->PSSetShaderResources(0, UINT(m_anotherSRVs.size()), m_anotherSRVs.data());

	context->DrawIndexed(m_pOwner->m_indexCount, 0, 0);
}

void ImageFilter::SetShaderResourceViews(const vector<ID3D11ShaderResourceView*>& srv)
{
	m_anotherSRVs.clear();
	for (ID3D11ShaderResourceView* s : srv)
	{
		if (s)
			m_anotherSRVs.push_back(s);
	}
}

void ImageFilter::SetRenderTargetViews(const vector<ID3D11RenderTargetView*>& rtv)
{
	m_anotherRTVs.clear();
	for (ID3D11RenderTargetView* r : rtv)
	{
		if (r)
			m_anotherRTVs.push_back(r);
	}
}
