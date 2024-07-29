#pragma once
class Scene;
class PostProcess;
class Mesh;
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
	
public:
	float m_fWidth;
	float m_fHeight;
	bool m_drawWireFrame = false;

public:
	ComPtr<ID3D11DepthStencilView>& GetDepthStencilView() { return m_useMSAADSV; }
	ComPtr<ID3D11RenderTargetView>& GetRenderTargetView() { return m_renderTargetView; }
	shared_ptr<Scene>& GetCurScene() { return m_curScene; }
private:
	bool CreateRenderTargetView();
	void CreateViewPort();
	void SetViewPort();
	bool CreateDepthStencilView();

private:
	ComPtr<ID3D11Texture2D> m_swapChainBackBuffer;
	ComPtr<ID3D11ShaderResourceView> m_shaderResourceView;
	ComPtr<ID3D11RenderTargetView> m_renderTargetView;

	ComPtr<ID3D11Texture2D> m_useMSAADepthBuffer;
	ComPtr<ID3D11DepthStencilView> m_useMSAADSV;

	D3D11_VIEWPORT m_viewPort;

	UINT m_iNumOfMultiSamplingLevel;
	friend class RenderScene;
// SingleTon
	SINGLE(SceneMgr)
};

