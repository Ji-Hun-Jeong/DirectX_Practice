#include "pch.h"
#include "TestScene.h"
#include "GeometryGenerator.h"
#include "Mesh.h"
#include "Sphere.h"
#include "Camera.h"
#include "Scene.h"
#include "SceneMgr.h"
#include "CubeMap.h"
#include "ModelLoader.h"
#include "Mirror.h"

TestScene::TestScene()
	: Scene()
{

}

void TestScene::Init()
{
	Scene::Init();
	m_pixelConstantData.light.lightPos = Vector3{ 0.0f,2.0f,-1.0f };
	m_pixelConstantData.rim.useRim = false;
	GETCAMERA()->SetPos(Vector3(0.0f, 1.0f, -1.0f));
	GETCAMERA()->SetSpeed(5.0f);
}

void TestScene::Enter()
{
}

void TestScene::Exit()
{
}

void TestScene::InitIBL()
{
	ReadCubeImage("Image/PBR/SkyBox/SampleSpecularHDR.dds", TEXTURE_TYPE::SPECULAR);
	ReadCubeImage("Image/PBR/SkyBox/SampleDiffuseHDR.dds", TEXTURE_TYPE::IRRADIANCE);
	ReadCubeImage("Image/PBR/SkyBox/SampleBrdf.dds", TEXTURE_TYPE::LUT);
}

void TestScene::InitMesh()
{
	ModelLoader::GetInst().Load("Image/Character/Sample/", "angel_armor.fbx");
	auto& obj = ModelLoader::GetInst().resultMesh;
	obj->ReadImage("Image/Character/Sample/angel_armor_albedo.jpg", TEXTURE_TYPE::ALBEDO, true);
	obj->ReadImage("Image/Character/Sample/angel_armor_metalness.jpg", TEXTURE_TYPE::METAL);
	obj->ReadImage("Image/Character/Sample/angel_armor_normal.jpg", TEXTURE_TYPE::NORMAL);
	obj->ReadImage("Image/Character/Sample/angel_armor_roughness.jpg", TEXTURE_TYPE::ROUGHNESS);
	m_vecObj.push_back(obj);

	MeshData md = GeometryGenerator::MakeSphere(0.1f, 30, 30);
	auto light = make_shared<Sphere>();
	light->Init(md, L"Basic", L"Basic");
	light->GetPixelConstantData().isLight = true;
	m_vecObj.push_back(light);

	MeshData sq = GeometryGenerator::MakeSquare();
	auto ground = make_shared<Object>();
	ground->Init(sq, L"Basic", L"Basic");
	ground->ReadImage("Image/Ground.png", TEXTURE_TYPE::ALBEDO, true);
	ground->m_rotation1 = Vector3(XM_PI / 2.0f, 0.0f, 0.0f);
	ground->m_scale = Vector3(5.0f);
	m_vecObj.push_back(ground);

	auto mirror = make_shared<Mirror>("Mirror", Vector3(2.0f, 1.6f, 0.0f), Vector3(0.0f, 90.0f, 0.0f), Vector3(0.0f), Vector3(1.0f, 1.66f, 1.0f));
	mirror->Init(sq, L"Basic", L"Basic");
	m_vecMirrors.push_back(mirror);

	mirror = make_shared<Mirror>("Mirror2", Vector3(0.0f, 1.6f, 2.0f), Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f), Vector3(1.0f, 1.66f, 1.0f));
	mirror->Init(sq, L"Basic", L"Basic");
	m_vecMirrors.push_back(mirror);
	
	m_focusObj = obj;
	m_focusObj->GetPixelConstantData().mat.selected = true;
}

void TestScene::InitCubeMap()
{
	MeshData sphereData = GeometryGenerator::MakeSphere(30.0f, 100, 100);
	std::reverse(sphereData.indices.begin(), sphereData.indices.end());
	auto sphereCube = make_shared<CubeMap>();
	sphereCube->Init(sphereData, L"CubeMap", L"CubeMap");
	sphereCube->ReadCubeImage("Image/PBR/SkyBox/SampleSpecularHDR.dds", TEXTURE_TYPE::SPECULAR);
	m_vecNonObj.push_back(sphereCube);
}

void TestScene::UpdateGUI()
{
	ImGui::Checkbox("DrawWireFrame", &SceneMgr::GetInst().m_drawWireFrame);
	ImGui::Checkbox("DrawNormal", &m_drawNormal);
	ImGui::SliderFloat("NormalSize", &m_normalSize, 0.0f, 2.0f);
	static bool useAlbedo = true;
	static bool useNormal = false;
	static bool useAO = false;
	static bool useHeight = false;
	static bool useMetallic = false;
	static bool useRoughness = false;
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
	ImGui::Checkbox("Use Metallic", &useMetallic);
	m_pixelConstantData.useMetallic = useMetallic;
	ImGui::Checkbox("Use Roughness", &useRoughness);
	m_pixelConstantData.useRoughness = useRoughness;

	ImGui::SliderFloat("Alpha", &m_fAlpha, 0.0f, 1.0f);
	ImGui::SliderFloat("Exposure", &m_pixelConstantData.exposure, 0.0f, 5.0f);
	ImGui::SliderFloat("Gamma", &m_pixelConstantData.gamma, 0.0f, 5.0f);
	ImGui::SliderFloat("BloomLightStrength", &m_pixelConstantData.bloom.bloomStrength, 0.0f, 1.0f);
	// ImGui::SliderFloat("LightStrength", &m_pixelConstantData.light.lightStrength.x, 0.0f, 1.0f);
	ImGui::SliderFloat3("LightPos", &m_pixelConstantData.light.lightPos.x, -5.0f, 5.0f);
	ImGui::SliderFloat("Metallic", &m_pixelConstantData.metallic, 0.0f, 1.0f);
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

void TestScene::Update(float dt)
{
	Scene::Update(dt);
}

