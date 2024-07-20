#pragma once
class Camera;
class Mirror;
class Mesh;
class PostProcess;
class Light;
class Scene
{
public:
	Scene();
public:
	virtual void Init() final;
	virtual void Enter() = 0;
	virtual void Exit() = 0;
	virtual void Update(float dt);
	virtual void UpdateGUI() = 0;
	virtual void RenderDepthOnly(ComPtr<ID3D11DeviceContext>& context, ComPtr<ID3D11DepthStencilView>& dsv);
	virtual void Render(ComPtr<ID3D11DeviceContext>& context, bool drawWireFrame);
	virtual void RenderLightView(ComPtr<ID3D11DeviceContext>& context);
	
	shared_ptr<Camera>& GetCamera() { return m_camera; }
	ComPtr<ID3D11ShaderResourceView>& GetIBLSRV(TEXTURE_TYPE textureType) { return m_iblSRV[(UINT)textureType]; }
	ComPtr<ID3D11Texture2D>& GetIBLTexture(TEXTURE_TYPE textureType) { return m_iblTexture[(UINT)textureType]; }

protected:
	virtual void InitMesh() = 0;
	virtual void InitIBL() = 0;
	virtual void InitSkyBox() = 0;
	void ReadCubeImage(const string& fileName, IBL_TYPE textureType);
	void RenderMirrorWorld(ComPtr<ID3D11DeviceContext>& context, shared_ptr<Mirror>& mirror, bool drawWireFrame);
	void CalcPickingObject();
	void UpdateGlobalCD();
	void UpdateGlobalBuffer(const Matrix& viewProj);

public:
	GlobalConstData m_globalCD;
	ComPtr<ID3D11Buffer> m_globalConstBuffer;

	MaterialConstData m_materialCD;
	CommonConstData m_commonCD;
	bool m_drawNormal = false;
	float m_fAlpha = 1.0f;
	
	vector<shared_ptr<Light>> m_vecLights;
protected:
	shared_ptr<Camera> m_camera;
	shared_ptr<Mesh> m_focusObj;
	
	vector<shared_ptr<Mesh>> m_vecMesh;
	shared_ptr<Mesh> m_skybox;
	vector<shared_ptr<Mirror>> m_vecMirrors;

	ComPtr<ID3D11Texture2D> m_iblTexture[(UINT)IBL_TYPE::END];
	ComPtr<ID3D11ShaderResourceView> m_iblSRV[(UINT)IBL_TYPE::END];
};

