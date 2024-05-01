#pragma once
class CubeMap;
class MeshGroup;
class Camera;
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
	vector<shared_ptr<MeshGroup>> m_vecMeshGroups;
	shared_ptr<MeshGroup> m_focusMeshGroup;
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

	float m_angleY = 70.0f;
	float m_nearZ = 0.01f;
	float m_farZ = 10000.0f;
public:
	shared_ptr<Camera>& GetCamera() { return m_camera; }
	float GetScreenWidth() { return m_fWidth; }
	float GetScreenHeight() { return m_fHeight; }
	float GetAspect() { return m_fWidth / m_fHeight; }
	float GetAngleY() { return XMConvertToRadians(m_angleY); }
	float GetNearZ() { return m_nearZ; }
	float GetFarZ() { return m_farZ; }

// Direct3D
private:
	bool InitDirect3D();
	bool CreateRenderTargetView();
	bool CreateRasterizerState();
	void CreateViewPort();
	void SetViewPort();
	bool CreateDepthStencilView();
	bool CreateDepthStencilState();
protected:
	UINT m_iNumOfMultiSamplingLevel;
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

