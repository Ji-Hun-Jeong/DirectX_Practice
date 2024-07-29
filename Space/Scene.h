#pragma once
class Camera;
class Mirror;
class Mesh;
class PostProcess;
class Light;
class SceneMgr;
class Scene
{
public:
	Scene(SceneMgr* owner);
public:
	virtual void Init();
	virtual void Enter() = 0;
	virtual void Exit() = 0;
	virtual void Update(float dt) = 0;
	virtual void UpdateGUI() = 0;
	
	virtual void Render(ComPtr<ID3D11DeviceContext>& context, bool drawWireFrame) = 0;
	
	shared_ptr<Camera>& GetCamera() { return m_camera; }
	ComPtr<ID3D11ShaderResourceView>& GetIBLSRV(TEXTURE_TYPE textureType) { return m_iblSRV[(UINT)textureType]; }
	ComPtr<ID3D11Texture2D>& GetIBLTexture(TEXTURE_TYPE textureType) { return m_iblTexture[(UINT)textureType]; }

protected:
	virtual void InitMesh() = 0;
	virtual void InitIBL() = 0;
	virtual void InitSkyBox() = 0;
	void ReadCubeImage(const string& fileName, IBL_TYPE textureType);

public:
	vector<shared_ptr<Light>> m_vecLights;

protected:
	shared_ptr<PostProcess> m_postProcess;
	shared_ptr<Mesh> m_postEffects;
	shared_ptr<Camera> m_camera;
	shared_ptr<Mesh> m_focusObj;
	
	vector<shared_ptr<Mesh>> m_vecMesh;
	shared_ptr<Mesh> m_skybox;

	ComPtr<ID3D11Texture2D> m_iblTexture[(UINT)IBL_TYPE::END];
	ComPtr<ID3D11ShaderResourceView> m_iblSRV[(UINT)IBL_TYPE::END];

	SceneMgr* m_pOwner;
};

