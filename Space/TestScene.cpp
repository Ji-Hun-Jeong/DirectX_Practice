#include "pch.h"
#include "TestScene.h"
#include "GeometryGenerator.h"
#include "Mesh.h"
#include "Sphere.h"
#include "Camera.h"
#include "Scene.h"
#include "SceneMgr.h"

TestScene::TestScene()
	: Scene()
{

}

void TestScene::Init()
{
	Scene::Init();
	m_pixelConstantData.light.lightPos = Vector3{ 0.0f,0.0f,0.0f };
	m_pixelConstantData.rim.useRim = false;
	GETCAMERA()->SetPos(Vector3(0.0f,0.0f,-1.0f));
	GETCAMERA()->SetSpeed(10.0f);
}

void TestScene::Enter()
{
}

void TestScene::Exit()
{
}

void TestScene::InitMesh()
{
	MeshData squareData = GeometryGenerator::MakeSquare();
	auto square = make_shared<Object>("square", Vector3(0.0f,-1.0f,1.0f), Vector3(90.0f,0.0f,0.0f), Vector3(0.0f), Vector3(3.0f));
	square->Init(squareData, L"Basic", L"Basic");
	square->ReadImage("Image/PBR/Bricks/Bricks075A_1K_Color.png", TEXTURE_TYPE::ALBEDO);
	square->ReadImage("Image/PBR/Bricks/Bricks075A_1K_NormalDX.png", TEXTURE_TYPE::NORMAL);
	square->ReadImage("Image/PBR/Bricks/Bricks075A_1K_AmbientOcclusion.png", TEXTURE_TYPE::AO);
	square->ReadImage("Image/PBR/Bricks/Bricks075A_1K_Displacement.png", TEXTURE_TYPE::HEIGHT);
	square->GetPixelConstantData().mat.diffuse = Vector3(0.7f);
	square->GetPixelConstantData().mat.specular = Vector3(0.7f);
	m_vecObj.push_back(square);

	m_focusObj = square;
}

void TestScene::UpdateGUI()
{
	ImGui::Checkbox("DrawWireFrame", &SceneMgr::GetInst().m_drawWireFrame);
	ImGui::Checkbox("DrawNormal", &m_drawNormal);
	ImGui::SliderFloat("NormalSize", &m_normalSize, 0.0f, 2.0f);
	static bool useAlbedo = false;
	static bool useNormal = false;
	static bool useAO = false;
	static bool useHeight = false;
	ImGui::Checkbox("Use Albedo", &useAlbedo);
	m_pixelConstantData.useAlbedo = useAlbedo;
	ImGui::Checkbox("Use Normal", &useNormal);
	m_pixelConstantData.useNormal = useNormal;
	ImGui::Checkbox("Use AO", &useAO);
	m_pixelConstantData.useAO = useAO;
	if (m_focusObj)
	{
		ImGui::Checkbox("Use Height", &useHeight);
		m_useHeight = useHeight;
		ImGui::SliderFloat("Height Scale", &m_heightScale, 0.0f, 2.0f);
	}
	
	// ImGui::SliderFloat("Threshold", &m_pixelConstantData.bloom.threshold, 0.0f, 1.0f);
	// ImGui::SliderFloat("BloomLightStrength", &m_pixelConstantData.bloom.bloomStrength, 0.0f, 1.0f);
	// ImGui::SliderFloat("LightStrength", &m_pixelConstantData.light.lightStrength.x, 0.0f, 1.0f);
	ImGui::SliderFloat3("LightPos", &m_pixelConstantData.light.lightPos.x, -2.0f, 2.0f);
	// if (m_focusObj)
	// {
	// 	ImGui::SliderFloat3("Ambient", &m_focusObj->GetPixelConstantData().mat.ambient.x, 0.0f, 1.0f);
	// 	ImGui::SliderFloat3("Diffuse", &m_focusObj->GetPixelConstantData().mat.diffuse.x, 0.0f, 1.0f);
	// 	ImGui::SliderFloat3("Specular", &m_focusObj->GetPixelConstantData().mat.specular.x, 0.0f, 1.0f);
	// 	ImGui::SliderFloat("Shiness", &m_focusObj->GetPixelConstantData().mat.shiness, 0.0f, 10.0f);
	// }
	// ImGui::SliderFloat("fallOfStart", &m_pixelConstantData.light.fallOfStart, 0.0f, 5.0f);
	// ImGui::SliderFloat("fallOfEnd", &m_pixelConstantData.light.fallOfEnd, 0.0f, 10.0f);
}
