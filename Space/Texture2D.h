#pragma once
class Texture2D
{
public:
	Texture2D();
public:
	void Init(const D3D11_TEXTURE2D_DESC& desc);
	void Init(const vector<uint8_t>& vec, UINT width, UINT height, DXGI_FORMAT pixelFormat);

public:
	ComPtr<ID3D11Texture2D>& GetTexture() { return m_texture; }
	ComPtr<ID3D11ShaderResourceView>& GetSRV() { return m_srv; }
	ComPtr<ID3D11RenderTargetView>& GetRTV() { return m_rtv; }
	ComPtr<ID3D11UnorderedAccessView>& GetUAV() { return m_uav; }

private:
	UINT m_iWidth;
	UINT m_iHeight;

	ComPtr<ID3D11Texture2D> m_texture;
	ComPtr<ID3D11Texture2D> m_stagingTexture;

	ComPtr<ID3D11ShaderResourceView> m_srv;
	ComPtr<ID3D11RenderTargetView> m_rtv;
	ComPtr<ID3D11UnorderedAccessView> m_uav;
};

