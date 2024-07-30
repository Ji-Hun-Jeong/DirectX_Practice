#pragma once
#include "Scene.h"
class Texture2D;
class RenderScene :
    public Scene
{
public:
	RenderScene(SceneMgr* owner);
public:
	void Init() override;
	void Update(float dt) override;
	void Enter() override;
	void Exit() override;
	void UpdateGUI() override;
	virtual void Render(ComPtr<ID3D11DeviceContext>& context, bool drawWireFrame);
	void RenderLightView(ComPtr<ID3D11DeviceContext>& context);
	void RenderDepthOnly(ComPtr<ID3D11DeviceContext>& context, ComPtr<ID3D11DepthStencilView>& dsv);
	void RenderObject(ComPtr<ID3D11DeviceContext>& context, bool drawWireFrame);
	void RenderMirrorWorld(ComPtr<ID3D11DeviceContext>& context, shared_ptr<Mirror>& mirror, bool drawWireFrame);

protected:
	void InitMesh() override;
	void InitSkyBox() override;
	void InitIBL() override;

	void CalcPickingObject();
	void UpdateGlobalCD();
	void UpdateGlobalBuffer(const Matrix& viewProj);
	void CreateRenderBuffer();

public:
	GlobalConstData m_globalCD;
	ComPtr<ID3D11Buffer> m_globalConstBuffer;

	MaterialConstData m_materialCD;
	CommonConstData m_commonCD;
	bool m_drawNormal = false;
	float m_fAlpha = 1.0f;

private:
	vector<shared_ptr<Mirror>> m_vecMirrors;

	shared_ptr<Texture2D> m_msaaTexture;
	shared_ptr<Texture2D> m_notMsaaTexture;
	shared_ptr<Texture2D> m_postEffectsTexture;

	ComPtr<ID3D11Texture2D> m_depthOnlyBuffer;
	ComPtr<ID3D11DepthStencilView> m_depthOnlyDSV;
	ComPtr<ID3D11ShaderResourceView> m_depthOnlySRV;

};

