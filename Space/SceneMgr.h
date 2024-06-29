#pragma once
class Scene;
class ImageFilter;
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
	vector<shared_ptr<ImageFilter>> m_filters;
	
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
	bool CreateRasterizerState();
	void CreateViewPort();
	void SetViewPort();
	bool CreateDepthStencilView();
	bool CreateDepthStencilState();
	void CreateFilters();

private:
	ComPtr<ID3D11ShaderResourceView> m_shaderResourceView;
	ComPtr<ID3D11RenderTargetView> m_renderTargetView;
	ComPtr<ID3D11RasterizerState> m_solidRasterizerState;
	ComPtr<ID3D11RasterizerState> m_wireRasterizerState;
	D3D11_VIEWPORT m_viewPort;

	ComPtr<ID3D11Texture2D> m_depthBuffer;
	ComPtr<ID3D11DepthStencilView> m_depthStencilView;
	ComPtr<ID3D11DepthStencilState> m_depthStencilState;

	UINT m_iNumOfMultiSamplingLevel;
// SingleTon
	SINGLE(SceneMgr)
};

