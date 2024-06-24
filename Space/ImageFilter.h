#pragma once
#include "NonObject.h"
class ImageFilter : public NonObject
{
public:
	ImageFilter(UINT width, UINT height,
		const wstring& vsPrefix, const wstring& psPrefix);
// Default
public:
	virtual void Update(float dt) override;
	virtual void UpdatePixelConstantData() override;

	virtual void Render(ID3D11DeviceContext* context) override;

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

	vector<ID3D11ShaderResourceView*> m_anotherSRVs;
	vector<ID3D11RenderTargetView*> m_anotherRTVs;
};

