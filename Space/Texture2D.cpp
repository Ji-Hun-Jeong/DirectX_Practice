#include "pch.h"
#include "Texture2D.h"
#include "D3DUtils.h"

Texture2D::Texture2D()
	: m_iWidth(0)
	, m_iHeight(0)
{
}

void Texture2D::Init(const D3D11_TEXTURE2D_DESC& desc)
{
	auto& device = D3DUtils::GetInst().GetDevice();
	CHECKRESULT(device->CreateTexture2D(&desc, nullptr, m_texture.GetAddressOf()));

	if (desc.BindFlags & D3D11_BIND_SHADER_RESOURCE)
		CHECKRESULT(device->CreateShaderResourceView(m_texture.Get(), nullptr, m_srv.GetAddressOf()));

	if (desc.BindFlags & D3D11_BIND_RENDER_TARGET)
		CHECKRESULT(device->CreateRenderTargetView(m_texture.Get(), nullptr, m_rtv.GetAddressOf()));

	if (desc.BindFlags & D3D11_BIND_UNORDERED_ACCESS)
		CHECKRESULT(device->CreateUnorderedAccessView(m_texture.Get(), nullptr, m_uav.GetAddressOf()));
}


void Texture2D::Init(UINT width, UINT height, DXGI_FORMAT pixelFormat)
{
	m_iWidth = width;
	m_iHeight = height;
	auto& context = D3DUtils::GetInst().GetContext();
	auto& device = D3DUtils::GetInst().GetDevice();

	D3D11_TEXTURE2D_DESC texDesc;
	ZeroMemory(&texDesc, sizeof(texDesc));
	texDesc.Width = width;
	texDesc.Height = height;
	texDesc.MipLevels = texDesc.ArraySize = 1;
	texDesc.Format = pixelFormat;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_STAGING;
	texDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ;
	texDesc.MiscFlags = 0;

	device->CreateTexture2D(&texDesc, nullptr, m_stagingTexture.GetAddressOf());

	texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET
		| D3D11_BIND_UNORDERED_ACCESS;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.CPUAccessFlags = 0;

	device->CreateTexture2D(&texDesc, nullptr, m_texture.GetAddressOf());

	CHECKRESULT(device->CreateShaderResourceView(m_texture.Get(), nullptr, m_srv.GetAddressOf()));
	CHECKRESULT(device->CreateRenderTargetView(m_texture.Get(), nullptr, m_rtv.GetAddressOf()));
	CHECKRESULT(device->CreateUnorderedAccessView(m_texture.Get(), nullptr, m_uav.GetAddressOf()));
}
