#include "pch.h"
#include "Scene.h"
#include "D3DUtils.h"
#include "Camera.h"
#include "GeometryGenerator.h"
#include "KeyMgr.h"
#include "SceneMgr.h"
#include "Mirror.h"
#include "GraphicsCommons.h"
#include "GraphicsPSO.h"
#include "PostProcess.h"

Scene::Scene()
	: m_camera(make_shared<Camera>())
{

}

void Scene::Init()
{
	D3DUtils::GetInst().CreateConstantBuffer<GlobalConstData>(m_globalCD, m_globalConstBuffer);
	InitIBL();
	InitMesh();
	InitSkyBox();
}

void Scene::Update(float dt)
{
	m_camera->Update(dt);

	UpdateGlobalCD();

	if (KEYCHECK(LBUTTON, TAP))
		CalcPickingObject();

	for (auto& mirror : m_vecMirrors)
		mirror->Update(dt);

	for (auto& mesh : m_vecMesh)
		mesh->Update(dt);

	m_skybox->Update(dt);

	m_globalCD.view = m_globalCD.view.Transpose();
	m_globalCD.proj = m_globalCD.proj.Transpose();
	m_globalCD.viewProj = m_globalCD.viewProj.Transpose();
	m_globalCD.invProj = m_globalCD.invProj.Transpose();
	D3DUtils::GetInst().UpdateBuffer<GlobalConstData>(m_globalCD, m_globalConstBuffer);
}

void Scene::Render(ComPtr<ID3D11DeviceContext>& context, bool drawMirror, bool drawWireFrame)
{
	drawWireFrame ? Graphics::g_defaultWirePSO.Setting() : Graphics::g_defaultSolidPSO.Setting();
	for (auto& mesh : m_vecMesh)
		mesh->Render(context);

	drawWireFrame ? Graphics::g_skyBoxWirePSO.Setting() : Graphics::g_skyBoxSolidPSO.Setting();
	m_skybox->Render(context);

	// 노말 그리기
	if (GETCURSCENE()->m_drawNormal)
	{
		Graphics::g_normalPSO.Setting();
		for (auto& mesh : m_vecMesh)
			mesh->DrawNormal(context);
	}
	drawWireFrame ? Graphics::g_defaultWirePSO.Setting() : Graphics::g_defaultSolidPSO.Setting();
	context->ClearDepthStencilView(SceneMgr::GetInst().GetDepthStencilView().Get(), D3D11_CLEAR_STENCIL, 1.0f, 0);
	for (int i = 0; i < m_vecMirrors.size(); ++i)
	{
		GraphicsPSO::SetStencilRef(i + 1);
		Graphics::g_stencilMaskSolidPSO.Setting();
		m_vecMirrors[i]->Render(context);
	}
	if (drawMirror)
	{
		for (int i = 0; i < m_vecMirrors.size(); ++i)
		{
			GraphicsPSO::SetStencilRef(i + 1);
			this->RenderMirrorWorld(context, m_vecMirrors[i], drawWireFrame);
		}
	}
}

void Scene::RenderMirrorWorld(ComPtr<ID3D11DeviceContext>& context, shared_ptr<Mirror>& mirror, bool drawWireFrame)
{
	// 여기서 거울을 그릴때마다 depth를 초기화를 해주기 때문에 거울을 하나씩 렌더링 하면 
	// 거울 안쪽세상이 현재 세상보다 위에 그려지는 것

	// context->OMSetDepthStencilState(SceneMgr::GetInst().GetDSS(DSS_TYPE::MASK).Get(), i + 1);
	// m_vecMirrors[i]->Render(context.Get(), m_viewProjBuffer); <- 이렇게 하면 안됌

	// 반드시 모든 거울을 스텐실처리를 하고 하나의 depthstencilview 위에서 전부 그려야한다.
	context->ClearDepthStencilView(SceneMgr::GetInst().GetDepthStencilView().Get(),
		D3D11_CLEAR_DEPTH, 1.0f, 0);

	Matrix basicViewProj = m_globalCD.viewProj;
	drawWireFrame ? Graphics::g_drawMaskWirePSO.Setting() : Graphics::g_drawMaskSolidPSO.Setting();
	this->UpdateGlobalBuffer(mirror->GetMirrorViewProj());
	for (auto& mesh : m_vecMesh)
		mesh->Render(context);

	drawWireFrame ? Graphics::g_drawMaskSkyBoxWirePSO.Setting() : Graphics::g_drawMaskSkyBoxSolidPSO.Setting();
	m_skybox->Render(context);

	float alpha[4] = { m_fAlpha,m_fAlpha ,m_fAlpha ,m_fAlpha };
	GraphicsPSO::SetBlendFactor(alpha);
	drawWireFrame ? Graphics::g_blendWirePSO.Setting() : Graphics::g_blendSolidPSO.Setting();

	// 거울을 그리면서 블랜딩을 해준다.
	this->UpdateGlobalBuffer(basicViewProj);
	for (auto& mirror : m_vecMirrors)
		mirror->Render(context);
}

void Scene::ReadCubeImage(const string& fileName, IBL_TYPE textureType)
{
	if (fileName == "")
		return;
	ComPtr<ID3D11Texture2D> texture;
	ComPtr<ID3D11ShaderResourceView> shaderResourceView;
	D3DUtils::GetInst().ReadCubeImage(fileName, texture, shaderResourceView);

	m_iblTexture[(UINT)textureType] = texture;
	m_iblSRV[(UINT)textureType] = shaderResourceView;
}


void Scene::CalcPickingObject()
{
	if (!m_focusObj)
		return;
	Vector2 mouseNDCPos = KeyMgr::GetInst().GetMouseNDCPos();
	Vector3 nearPlane = Vector3(mouseNDCPos.x, mouseNDCPos.y, 0.0f);
	Vector3 farPlane = Vector3(mouseNDCPos.x, mouseNDCPos.y, 1.0f);
	Matrix inverseProjView = (m_globalCD.viewProj).Invert();
	Vector3 rayStartPos = Vector3::Transform(nearPlane
		, inverseProjView);
	Vector3 rayFinishPos = Vector3::Transform(farPlane
		, inverseProjView);
	Vector3 rayDir = rayFinishPos - rayStartPos;
	rayDir.Normalize();
	MyRay ray{ rayStartPos,rayDir };
	for (auto& obj : m_vecMesh)
	{
		bool isCollision = false;
		isCollision = obj->IsCollision(ray);
		if (isCollision)
		{
			// m_focusObj->GetPixelConstantData().mat.selected = false;
			// obj->GetPixelConstantData().mat.selected = true;
			m_focusObj = obj;
		}
	}
}

void Scene::UpdateGlobalCD()
{
	m_globalCD.eyePos = m_camera->GetPos();
	m_globalCD.view = m_camera->m_view;
	m_globalCD.proj = m_camera->m_projection;
	m_globalCD.invProj = m_globalCD.proj.Invert();
	m_globalCD.viewProj = m_globalCD.view * m_globalCD.proj;
	m_globalCD.strengthIBL = 1.0f;
}

void Scene::UpdateGlobalBuffer(const Matrix& viewProj)
{
	m_globalCD.viewProj = viewProj;
	D3DUtils::GetInst().UpdateBuffer<GlobalConstData>(m_globalCD, m_globalConstBuffer);
}
