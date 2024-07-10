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

TestScene::TestScene()
	: Scene()
{

}

void TestScene::Init()
{
	Scene::Init();
	m_pixelConstantData.light.lightPos = Vector3{ 0.0f,-2.4f,-5.0f };
	m_pixelConstantData.rim.useRim = false;
	GETCAMERA()->SetPos(Vector3(0.0f, 0.0f, -3.0f));
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
	// 포맷구조 변경 큐브맵 텍스쳐 변경
	/*MeshData sphereData = GeometryGenerator::MakeSphere(1.0f, 100, 100);
	auto sphere = make_shared<Sphere>("sphere", Vector3(0.0f, 1.0f, 1.0f), Vector3(0.0f), Vector3(0.0f), Vector3(1.0f));
	sphere->Init(sphereData, L"Basic", L"Basic");
	sphere->ReadCubeImage("Image/PBR/SkyBox/SampleSpecularHDR.dds", TEXTURE_TYPE::SPECULAR);
	sphere->ReadCubeImage("Image/PBR/SkyBox/SampleDiffuseHDR.dds", TEXTURE_TYPE::IRRADIANCE);
	sphere->ReadImage("Image/PBR/Metal/worn-painted-metal_albedo.png", TEXTURE_TYPE::ALBEDO, true);
	sphere->ReadImage("Image/PBR/Metal/worn-painted-metal_normal-dx.png", TEXTURE_TYPE::NORMAL);
	sphere->ReadImage("Image/PBR/Metal/worn-painted-metal_ao.png", TEXTURE_TYPE::AO);
	sphere->ReadImage("Image/PBR/Metal/worn-painted-metal_metallic.png", TEXTURE_TYPE::METAL);
	sphere->ReadImage("Image/PBR/Metal/worn-painted-metal_roughness.png", TEXTURE_TYPE::ROUGHNESS);
	sphere->ReadCubeImage("Image/PBR/SkyBox/SampleBrdf.dds", TEXTURE_TYPE::LUT);
	sphere->ReadImage("Image/PBR/Metal/worn-painted-metal_height.png", TEXTURE_TYPE::HEIGHT);
	m_vecObj.push_back(sphere);*/
	ModelLoader::GetInst().Load("Image/Character/Sample/", "angel_armor.fbx");
	auto& obj = ModelLoader::GetInst().resultMesh;
	obj->ReadCubeImage("Image/PBR/SkyBox/SampleSpecularHDR.dds", TEXTURE_TYPE::SPECULAR);
	obj->ReadCubeImage("Image/PBR/SkyBox/SampleDiffuseHDR.dds", TEXTURE_TYPE::IRRADIANCE);
	obj->ReadCubeImage("Image/PBR/SkyBox/SampleBrdf.dds", TEXTURE_TYPE::LUT);
	obj->ReadImage("Image/Character/Sample/angel_armor_albedo.jpg", TEXTURE_TYPE::ALBEDO, true);
	obj->ReadImage("Image/Character/Sample/angel_armor_metalness.jpg", TEXTURE_TYPE::METAL);
	obj->ReadImage("Image/Character/Sample/angel_armor_normal.jpg", TEXTURE_TYPE::NORMAL);
	obj->ReadImage("Image/Character/Sample/angel_armor_roughness.jpg", TEXTURE_TYPE::ROUGHNESS);
	m_vecObj.push_back(obj);

	m_focusObj = obj;
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
	static bool useAlbedo = false;
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

	ImGui::SliderFloat("Exposure", &m_pixelConstantData.exposure, 0.0f, 5.0f);
	ImGui::SliderFloat("Gamma", &m_pixelConstantData.gamma, 0.0f, 5.0f);
	// ImGui::SliderFloat("Threshold", &m_pixelConstantData.bloom.threshold, 0.0f, 1.0f);
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
