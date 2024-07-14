#pragma once
class PostProcess;
class ImageFilter
{
public:
	ImageFilter(PostProcess* owner, ComPtr<ID3D11PixelShader>& ps
		, float width, float height);
	// Default
public:
	void Render(ComPtr<ID3D11DeviceContext>& context);

	// Getter, Setter
public:
	void SetShaderResourceViews(const vector<ID3D11ShaderResourceView*>& srv);
	ComPtr<ID3D11ShaderResourceView>& GetShaderResourceView() { return m_shaderResourceView; }
	void SetRenderTargetViews(const vector<ID3D11RenderTargetView*>& rtv);
	ComPtr<ID3D11RenderTargetView>& GetRenderTargetView() { return m_renderTargetView; }

	// D3D11Member
private:
	ComPtr<ID3D11Texture2D> m_texture;
	ComPtr<ID3D11ShaderResourceView> m_shaderResourceView;
	ComPtr<ID3D11RenderTargetView> m_renderTargetView;
	D3D11_VIEWPORT m_viewPort;

	ComPtr<ID3D11PixelShader> m_pixelShader;

	vector<ID3D11ShaderResourceView*> m_anotherSRVs;
	vector<ID3D11RenderTargetView*> m_anotherRTVs;

	PostProcess* m_pOwner;
	float m_fWidth;
	float m_fHeight;
};

