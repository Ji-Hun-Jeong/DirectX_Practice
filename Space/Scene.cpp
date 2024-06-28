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

Scene::Scene()
	: m_camera(make_shared<Camera>())
{

}

void Scene::Init()
{
	InitMesh();
}

void Scene::Update(float dt)
{
	m_camera->Update(dt);
	m_pixelConstantData.eyePos = m_camera->GetPos();

	if (KEYCHECK(LBUTTON, TAP))
	{
		CalcPickingObject();
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
}

void Scene::UpdateGUI()
{
	ImGui::Checkbox("DrawWireFrame", &SceneMgr::GetInst().m_drawWireFrame);
	ImGui::Checkbox("DrawNormal", &m_drawNormal);
	ImGui::Checkbox("Use Rim", &m_pixelConstantData.rim.useRim);
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
	for (auto& obj : m_vecObj)
	{
		if (obj)
			obj->Render(context.Get());
	}
	for (auto& nonObj : m_vecNonObj)
	{
		if (nonObj)
			nonObj->Render(context.Get());
	}
}

void Scene::InitCubeMap()
{
	MeshData cubeMapData = GeometryGenerator::MakeSphere(1000.0f, 30, 30, "image/space.dds");
	std::reverse(cubeMapData.indices.begin(), cubeMapData.indices.end());
	auto cubeMap = make_shared<CubeMap>();
	cubeMap->Init(cubeMapData, L"CubeMap", L"CubeMap");
	m_vecNonObj.push_back(cubeMap);
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


