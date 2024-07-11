#pragma once
class Object;
class NonObject;
class Camera;
class ImageFilter;
class CubeMap;
class Mirror;
class Scene
{
public:
	Scene();
public:
	virtual void Init();
	virtual void Enter() = 0;
	virtual void Exit() = 0;
	virtual void Update(float dt);
	virtual void UpdateGUI();
	virtual void Render();
	
	shared_ptr<Camera>& GetCamera() { return m_camera; }
	ComPtr<ID3D11ShaderResourceView>& GetIBLSRV(TEXTURE_TYPE textureType) { return m_iblSRV[(UINT)textureType]; }
	ComPtr<ID3D11Texture2D>& GetIBLTexture(TEXTURE_TYPE textureType) { return m_iblTexture[(UINT)textureType]; }
protected:
	virtual void InitMesh() = 0;
	virtual void InitCubeMap();
	virtual void InitIBL();
	void ReadCubeImage(const string& fileName, TEXTURE_TYPE textureType);
	void RenderMirror(ComPtr<ID3D11DeviceContext>& context, shared_ptr<Mirror>& mirror
		, UINT maskNum);
	void CalcPickingObject();

public:
	PixelConstantData m_pixelConstantData;
	bool m_useHeight = false;
	float m_heightScale = 1.0f;
	bool m_drawNormal = false;
	float m_normalSize = 1.0f;

	ComPtr<ID3D11Buffer> m_viewProjBuffer;
	Matrix				 m_viewProj;

protected:
	shared_ptr<Camera> m_camera;
	shared_ptr<Object> m_focusObj;

	vector<shared_ptr<Object>> m_vecObj;
	vector<shared_ptr<NonObject>> m_vecNonObj;
	vector<shared_ptr<ImageFilter>> m_filters;
	vector<shared_ptr<Mirror>> m_vecMirrors;

	ComPtr<ID3D11Texture2D> m_iblTexture[(UINT)TEXTURE_TYPE::LUT + 1];
	ComPtr<ID3D11ShaderResourceView> m_iblSRV[(UINT)TEXTURE_TYPE::LUT + 1];	
};

