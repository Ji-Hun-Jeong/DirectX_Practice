#pragma once
class CubeMap;
class Mesh;
class Camera;
class ImageFilter;
class Core
{
// Start
public:
	bool Init();
	bool InitGUI();
	void InitMesh();
	void InitCubeMap();
	void Update(float dt);
	void UpdateGUI();
	void Render();
	int Progress();

// Object
private:
	vector<shared_ptr<Mesh>> m_vecMeshes;
	shared_ptr<Mesh> m_focusMesh;
	shared_ptr<CubeMap> m_cubeMap;

public:
	float m_normalSize = 1.0f;
	PixelConstantData m_pixelConstantData;
	bool m_drawNormal = false;
	bool m_drawWireFrame = false;
	
// Window
private:
	bool InitMainWindow();
	HWND m_mainWindow;
public:
	HWND GetMainWindow() { return m_mainWindow; }
	LRESULT MsgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

// Screen 
protected:
	float m_fWidth;
	float m_fHeight;
	
	shared_ptr<Camera> m_camera;
	vector<shared_ptr<ImageFilter>> m_filters;

	float m_angleY = 70.0f;
	float m_nearZ = 0.01f;
	float m_farZ = 10000.0f;
public:
	shared_ptr<Camera>& GetCamera() { return m_camera; }
	float GetScreenWidth() const { return m_fWidth; }
	float GetScreenHeight() const { return m_fHeight; }
	float GetAspect() const { return m_fWidth / m_fHeight; }
	float GetAngleY() const { return XMConvertToRadians(m_angleY); }
	float GetNearZ() const { return m_nearZ; }
	float GetFarZ() const { return m_farZ; } 

// D3D11Member

// Getter, Setter
public:
	UINT m_iNumOfMultiSamplingLevel;
	ComPtr<ID3D11DepthStencilView>& GetDepthStencilView() { return m_depthStencilView; }
	ComPtr<ID3D11RenderTargetView>& GetRenderTargetView() { return m_renderTargetView; }

private:
	bool InitDirect3D();
	bool CreateRenderTargetView();
	bool CreateRasterizerState();
	void CreateViewPort();
	void SetViewPort();
	bool CreateDepthStencilView();
	bool CreateDepthStencilState();
	void CreateFilters();
protected:
	ComPtr<ID3D11ShaderResourceView> m_shaderResourceView;
	ComPtr<ID3D11RenderTargetView> m_renderTargetView;
	ComPtr<ID3D11RasterizerState> m_solidRasterizerState;
	ComPtr<ID3D11RasterizerState> m_wireRasterizerState;
	D3D11_VIEWPORT m_viewPort;

	ComPtr<ID3D11Texture2D> m_depthBuffer;
	ComPtr<ID3D11DepthStencilView> m_depthStencilView;
	ComPtr<ID3D11DepthStencilState> m_depthStencilState;
// SingleTon
	SINGLE(Core)
};

