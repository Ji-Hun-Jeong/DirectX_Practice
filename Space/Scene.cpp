#include "pch.h"
#include "Scene.h"
#include "Object.h"
#include "NonObject.h"
#include "D3DUtils.h"
#include "Camera.h"
#include "GeometryGenerator.h"
#include "CubeMap.h"
#include "KeyMgr.h"
#include "SceneMgr.h"
#include "Mirror.h"

Scene::Scene()
	: m_camera(make_shared<Camera>())
{
	
}

void Scene::Init()
{
	D3DUtils::GetInst().CreateConstantBuffer<Matrix>(m_viewProj, m_viewProjBuffer);
	InitIBL();
	InitMesh();
	InitCubeMap();
}

void Scene::Update(float dt)
{
	m_camera->Update(dt);
	m_pixelConstantData.eyePos = m_camera->GetPos();
	m_viewProj = m_camera->m_view * m_camera->m_projection;
	
	if (KEYCHECK(LBUTTON, TAP))
	{
		CalcPickingObject();
	}

	for (auto& mirror : m_vecMirrors)
	{
		if (mirror)
			mirror->Update(dt);
	}

	for (auto& obj : m_vecObj)
	{
		if (obj)
			obj->Update(dt);
	}

	for (auto& nonObj : m_vecNonObj)
	{
		if (nonObj)
			nonObj->Update(dt);
	}

	m_viewProj = m_viewProj.Transpose();
	D3DUtils::GetInst().UpdateBuffer(m_viewProjBuffer, m_viewProj);
}

void Scene::UpdateGUI()
{
	ImGui::Checkbox("DrawWireFrame", &SceneMgr::GetInst().m_drawWireFrame);
	ImGui::Checkbox("DrawNormal", &m_drawNormal);
	static bool useRim = m_pixelConstantData.rim.useRim;
	ImGui::Checkbox("Use Rim", &useRim);
	m_pixelConstantData.rim.useRim = useRim;
	ImGui::SliderFloat("NormalSize", &m_normalSize, 0.0f, 100.0f);
	ImGui::SliderFloat("Rim Strength", &m_pixelConstantData.rim.rimStrength, 0.0f, 20.0f);
	ImGui::SliderFloat("Rim Power", &m_pixelConstantData.rim.rimPower, 0.0f, 5.0f);
	ImGui::SliderFloat("Threshold", &m_pixelConstantData.bloom.threshold, 0.0f, 1.0f);
	ImGui::SliderFloat("BloomLightStrength", &m_pixelConstantData.bloom.bloomStrength, 0.0f, 1.0f);
	ImGui::SliderFloat("LightStrength", &m_pixelConstantData.light.lightStrength.x, 0.0f, 1.0f);
	ImGui::SliderFloat3("LightPos", &m_pixelConstantData.light.lightPos.x, -1000.0f, 1000.0f);
	ImGui::SliderFloat("fallOfStart", &m_pixelConstantData.light.fallOfStart, 0.0f, 1000.0f);
	ImGui::SliderFloat("fallOfEnd", &m_pixelConstantData.light.fallOfEnd, 0.0f, 1000.0f);
	if (m_focusObj)
	{
		ImGui::SliderFloat3("Ambient", &m_focusObj->GetPixelConstantData().mat.ambient.x, 0.0f, 1.0f);
		ImGui::SliderFloat3("Diffuse", &m_focusObj->GetPixelConstantData().mat.diffuse.x, 0.0f, 1.0f);
		ImGui::SliderFloat3("Specular", &m_focusObj->GetPixelConstantData().mat.specular.x, 0.0f, 1.0f);
		ImGui::SliderFloat("Shiness", &m_focusObj->GetPixelConstantData().mat.shiness, 0.0f, 10.0f);
	}
}

void Scene::Render()
{
	ComPtr<ID3D11DeviceContext>& context = D3DUtils::GetInst().GetContext();
	context->OMSetDepthStencilState(SceneMgr::GetInst().GetDSS(DSS_TYPE::BASIC).Get(), 0);
	for (auto& obj : m_vecObj)
	{
		if (obj)
			obj->Render(context, m_viewProjBuffer);
	}
	
	for (auto& nonObj : m_vecNonObj)
	{
		if (nonObj)
			nonObj->Render(context, m_viewProjBuffer);
	}
	
	context->ClearDepthStencilView(SceneMgr::GetInst().GetDepthStencilView().Get()
		, D3D11_CLEAR_STENCIL, 1.0f, 0);
	for (int i = 0; i < m_vecMirrors.size(); ++i)
	{
		if (!m_vecMirrors[i]) continue;
		context->OMSetDepthStencilState(SceneMgr::GetInst().GetDSS(DSS_TYPE::MASK).Get(), i + 1);
		m_vecMirrors[i]->Render(context, m_viewProjBuffer);
	}
	for (int i = 0; i < m_vecMirrors.size(); ++i)
	{
		if (!m_vecMirrors[i]) continue;
		this->RenderMirror(context, m_vecMirrors[i], i + 1);
	}
}

void Scene::InitIBL()
{
}

void Scene::ReadCubeImage(const string& fileName, TEXTURE_TYPE textureType)
{
	if (fileName == "")
		return;
	ComPtr<ID3D11Texture2D> texture;
	ComPtr<ID3D11ShaderResourceView> shaderResourceView;
	D3DUtils::GetInst().ReadCubeImage(fileName, texture, shaderResourceView);

	m_iblTexture[(UINT)textureType] = texture;
	m_iblSRV[(UINT)textureType] = shaderResourceView;
}

void Scene::RenderMirror(ComPtr<ID3D11DeviceContext>& context, shared_ptr<Mirror>& mirror, UINT maskNum)
{
	SceneMgr& smgr = SceneMgr::GetInst();
	// 여기서 거울을 그릴때마다 depth를 초기화를 해주기 때문에 거울을 하나씩 렌더링 하면 
	// 거울 안쪽세상이 현재 세상보다 위에 그려지는 것
	
	// context->OMSetDepthStencilState(SceneMgr::GetInst().GetDSS(DSS_TYPE::MASK).Get(), i + 1);
	// m_vecMirrors[i]->Render(context.Get(), m_viewProjBuffer); <- 이렇게 하면 안됌
	
	// 반드시 모든 거울을 스텐실처리를 하고 하나의 depthstencilview 위에서 전부 그려야한다.
	context->ClearDepthStencilView(smgr.GetDepthStencilView().Get(),
		D3D11_CLEAR_DEPTH, 1.0f, 0);
	context->OMSetDepthStencilState(smgr.GetDSS(DSS_TYPE::DRAWMASK).Get(), maskNum);
	context->RSSetState(smgr.GetRSS(RSS_TYPE::SOLIDCCW).Get());

	ComPtr<ID3D11Buffer>& viewProjBuffer = mirror->GetViewProjBuffer();

	for (auto& obj : m_vecObj)
	{
		if (obj)
			obj->Render(context, viewProjBuffer);
	}
	for (auto& nonObj : m_vecNonObj)
	{
		if (nonObj)
			nonObj->Render(context, viewProjBuffer);
	}

	float alpha[4] = { m_fAlpha,m_fAlpha ,m_fAlpha ,m_fAlpha };
	context->RSSetState(smgr.GetRSS(RSS_TYPE::SOLID).Get());
	context->OMSetBlendState(SceneMgr::GetInst().GetBlendState().Get(), alpha, 0xFF);
	for (auto& mirror : m_vecMirrors)
	{
		if (!mirror) continue;
		mirror->Render(context, m_viewProjBuffer);
	}
	context->OMSetBlendState(nullptr, nullptr, 0xff);
}

void Scene::InitCubeMap()
{

}

void Scene::CalcPickingObject()
{
	if (!m_focusObj)
		return;
	Vector2 mouseNDCPos = KeyMgr::GetInst().GetMouseNDCPos();
	Vector3 nearPlane = Vector3(mouseNDCPos.x, mouseNDCPos.y, 0.0f);
	Vector3 farPlane = Vector3(mouseNDCPos.x, mouseNDCPos.y, 1.0f);
	Matrix inverseProjView = (m_camera->m_view * m_camera->m_projection).Invert();
	Vector3 rayStartPos = Vector3::Transform(nearPlane
		, inverseProjView);
	Vector3 rayFinishPos = Vector3::Transform(farPlane
		, inverseProjView);
	Vector3 rayDir = rayFinishPos - rayStartPos;
	rayDir.Normalize();
	MyRay ray{ rayStartPos,rayDir };
	for (auto& obj : m_vecObj)
	{
		if (!obj) continue;
		bool isCollision = false;
		isCollision = obj->IsCollision(ray);
		if (isCollision)
		{
			m_focusObj->GetPixelConstantData().mat.selected = false;
			obj->GetPixelConstantData().mat.selected = true;
			m_focusObj = obj;
		}
	}
}


