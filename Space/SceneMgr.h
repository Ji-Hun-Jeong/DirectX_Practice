#pragma once
class Scene;
class ImageFilter;
class PostProcess;
class SceneMgr
{
public:
	bool Init(float width, float height);
	bool InitGUI();
	bool InitDirect3D();
	void Update(float dt);
	void Render();
	void ChangeCurScene(SCENE_TYPE scene);

private:
	shared_ptr<Scene> m_arrScene[(UINT)SCENE_TYPE::END];
	shared_ptr<Scene> m_curScene;
	shared_ptr<PostProcess> m_postProcess;
	
public:
	float m_fWidth;
	float m_fHeight;
	bool m_drawWireFrame = false;

public:
	ComPtr<ID3D11DepthStencilView>& GetDepthStencilView() { return m_depthStencilView; }
	ComPtr<ID3D11RenderTargetView>& GetRenderTargetView() { return m_renderTargetView; }
	shared_ptr<Scene>& GetCurScene() { return m_curScene; }
private:
	bool CreateRenderTargetView();
	void CreateViewPort();
	void CreateRenderBuffer();
	void SetViewPort();
	bool CreateDepthStencilView();

private:
	ComPtr<ID3D11Texture2D> m_swapChainBackBuffer;
	ComPtr<ID3D11ShaderResourceView> m_shaderResourceView;
	ComPtr<ID3D11RenderTargetView> m_renderTargetView;

	ComPtr<ID3D11Texture2D> m_msaaTexture;
	ComPtr<ID3D11ShaderResourceView> m_msaaSRV;
	ComPtr<ID3D11RenderTargetView> m_msaaRTV;

	ComPtr<ID3D11Texture2D> m_notMsaaTexture;
	ComPtr<ID3D11ShaderResourceView> m_notMsaaSRV;
	ComPtr<ID3D11RenderTargetView> m_notMsaaRTV;

	D3D11_VIEWPORT m_viewPort;

	ComPtr<ID3D11Texture2D> m_depthBuffer;
	ComPtr<ID3D11DepthStencilView> m_depthStencilView;

	UINT m_iNumOfMultiSamplingLevel;
// SingleTon
	SINGLE(SceneMgr)
};

