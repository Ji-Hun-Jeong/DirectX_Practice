#pragma once
#include "D3DUtils.h"
class Texture2D
{
public:
	Texture2D();
public:
	void Init(const D3D11_TEXTURE2D_DESC& desc);
	void Init(UINT width, UINT height, DXGI_FORMAT pixelFormat);

	template <typename T_Element>
	void Upload(const vector<T_Element>& vec);

	template <typename T_Element>
	void Download(vector<T_Element>& vec);

public:
	ComPtr<ID3D11Texture2D>& GetTexture() { return m_texture; }
	ComPtr<ID3D11ShaderResourceView>& GetSRV() { return m_srv; }
	ComPtr<ID3D11RenderTargetView>& GetRTV() { return m_rtv; }
	ComPtr<ID3D11UnorderedAccessView>& GetUAV() { return m_uav; }

	UINT m_iWidth;
	UINT m_iHeight;
private:


	ComPtr<ID3D11Texture2D> m_texture;
	ComPtr<ID3D11Texture2D> m_stagingTexture;

	ComPtr<ID3D11ShaderResourceView> m_srv;
	ComPtr<ID3D11RenderTargetView> m_rtv;
	ComPtr<ID3D11UnorderedAccessView> m_uav;
};

template <typename T_Element>
void Texture2D::Upload(const vector<T_Element>& vec)
{
	auto& context = D3DUtils::GetInst().GetContext();
	D3D11_MAPPED_SUBRESOURCE ms;
	context->Map(m_stagingTexture.Get(), 0, D3D11_MAP_WRITE, NULL, &ms);
	memcpy(ms.pData, vec.data(), vec.size() * sizeof(T_Element));
	context->Unmap(m_stagingTexture.Get(), 0);
	context->CopyResource(m_texture.Get(), m_stagingTexture.Get());
}

template <typename T_Element>
void Texture2D::Download(vector<T_Element>& vec)
{
	auto& context = D3DUtils::GetInst().GetContext();
	context->CopyResource(m_stagingTexture.Get(), m_texture.Get());
	D3D11_MAPPED_SUBRESOURCE ms;
	context->Map(m_stagingTexture.Get(), 0, D3D11_MAP_READ, NULL, &ms);
	memcpy(vec.data(), ms.pData, vec.size() * sizeof(T_Element));
	context->Unmap(m_stagingTexture.Get(), 0);
}