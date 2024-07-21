#include "pch.h"
#include "TestScene.h"
#include "GeometryGenerator.h"
#include "Mesh.h"
#include "Camera.h"
#include "Scene.h"
#include "SceneMgr.h"
#include "ModelLoader.h"
#include "Mirror.h"
#include "D3DUtils.h"
#include "GraphicsCommons.h"
#include "Light.h"

TestScene::TestScene()
	: Scene()
{

}

void TestScene::Enter()
{
	ComPtr<ID3D11DeviceContext>& context = D3DUtils::GetInst().GetContext();
	context->VSSetSamplers(0, Graphics::g_vecSamplers.size(), Graphics::g_vecSamplers.data());
	context->GSSetSamplers(0, Graphics::g_vecSamplers.size(), Graphics::g_vecSamplers.data());
	context->PSSetSamplers(0, Graphics::g_vecSamplers.size(), Graphics::g_vecSamplers.data());

	context->VSSetConstantBuffers(0, 1, m_globalConstBuffer.GetAddressOf());
	context->GSSetConstantBuffers(0, 1, m_globalConstBuffer.GetAddressOf());
	context->PSSetConstantBuffers(0, 1, m_globalConstBuffer.GetAddressOf());
	context->PSSetShaderResources(10, (UINT)IBL_TYPE::END, m_iblSRV[(UINT)IBL_TYPE::SPECULAR].GetAddressOf());

	GETCAMERA()->SetPos(Vector3(0.0f, 1.0f, -1.0f));
	GETCAMERA()->SetSpeed(5.0f);
}

void TestScene::Exit()
{
}

void TestScene::InitIBL()
{
	ReadCubeImage("Image/PBR/SkyBox/SampleSpecularHDR.dds", IBL_TYPE::SPECULAR);
	ReadCubeImage("Image/PBR/SkyBox/SampleDiffuseHDR.dds", IBL_TYPE::IRRADIANCE);
	ReadCubeImage("Image/PBR/SkyBox/SampleBrdf.dds", IBL_TYPE::LUT);

}

void TestScene::InitMesh()
{
	/*ModelLoader::GetInst().Load("Image/Character/Sample/", "angel_armor.fbx");
	auto& obj = ModelLoader::GetInst().resultMesh;
	obj->ReadImage("Image/Character/Sample/angel_armor_albedo.jpg", TEXTURE_TYPE::ALBEDO, true);
	obj->ReadImage("Image/Character/Sample/angel_armor_metalness.jpg", TEXTURE_TYPE::METAL);
	obj->ReadImage("Image/Character/Sample/angel_armor_normal.jpg", TEXTURE_TYPE::NORMAL);
	obj->ReadImage("Image/Character/Sample/angel_armor_e.jpg", TEXTURE_TYPE::EMISSIVE);
	obj->ReadImage("Image/Character/Sample/angel_armor_roughness.jpg", TEXTURE_TYPE::ROUGHNESS);
	m_vecMesh.push_back(obj);*/

	MeshData md = GeometryGenerator::MakeSphere(0.1f, 30, 30);
	auto light = make_shared<Light>();
	light->Init(md);
	m_vecMesh.push_back(light);
	m_vecLights.push_back(light);

	/*auto l = make_shared<Mesh>();
	l->Init(md);
	l->m_translation = Vector3(0.0f, 2.0f, 4.0f);
	m_vecMesh.push_back(l);*/

	MeshData sq = GeometryGenerator::MakeSquare();
	auto ground = make_shared<Mesh>();
	ground->Init(sq);
	ground->ReadImage("Image/Ground.png", TEXTURE_TYPE::ALBEDO, true);
	ground->m_rotation1 = Vector3(XM_PI / 2.0f, 0.0f, 0.0f);
	ground->m_scale = Vector3(5.0f);
	m_vecMesh.push_back(ground);


	auto mirror = make_shared<Mirror>("Mirror", Vector3(2.0f, 1.6f, 0.0f), Vector3(0.0f, 90.0f, 0.0f), Vector3(0.0f), Vector3(1.0f, 1.66f, 1.0f));
	mirror->Init(sq);
	m_vecMirrors.push_back(mirror);

	mirror = make_shared<Mirror>("Mirror2", Vector3(0.0f, 1.6f, 2.0f), Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f), Vector3(1.0f, 1.66f, 1.0f));
	mirror->Init(sq);
	m_vecMirrors.push_back(mirror);

	m_focusObj = light;

}

void TestScene::InitSkyBox()
{
	MeshData sphereData = GeometryGenerator::MakeSphere(30.0f, 100, 100);
	std::reverse(sphereData.indices.begin(), sphereData.indices.end());
	m_skybox = make_shared<Mesh>();
	m_skybox->Init(sphereData);
}

void TestScene::UpdateGUI()
{
	ImGui::Checkbox("DrawWireFrame", &SceneMgr::GetInst().m_drawWireFrame);
	ImGui::RadioButton("Render", &m_commonCD.mode, 0);
	ImGui::SameLine();
	ImGui::RadioButton("Depth", &m_commonCD.mode, 1);
	ImGui::SliderFloat("Depth Strength", &m_commonCD.depthStrength, 0.0f, 1.0f);
	ImGui::SliderFloat("Fog Strength", &m_commonCD.fogStrength, 0.0f, 1.0f);
	ImGui::CheckboxFlags("Use Albedo", &m_materialCD.useAlbedo, 1);
	ImGui::SliderFloat3("AlbedoFactor", &m_materialCD.albedoFactor.x, 0.0f, 1.0f);
	ImGui::Checkbox("DrawNormal", &m_drawNormal);
	ImGui::CheckboxFlags("Use Normal", &m_commonCD.useNormal, 1);
	ImGui::SliderFloat("NormalSize", &m_commonCD.normalSize, 0.0f, 2.0f);
	ImGui::CheckboxFlags("Use AO", &m_materialCD.useAO, 1);
	ImGui::CheckboxFlags("Use Emissive", &m_materialCD.useEmissive, 1);
	ImGui::SliderFloat3("EmissionFactor", &m_materialCD.emissionFactor.x, 0.0f, 1.0f);
	ImGui::CheckboxFlags("Use Height", &m_commonCD.useHeight, 1);
	ImGui::SliderFloat("Height Scale", &m_commonCD.heightScale, 0.0f, 2.0f);

	ImGui::CheckboxFlags("Use Metallic", &m_materialCD.useMetallic, 1);
	ImGui::SliderFloat("Metallic", &m_materialCD.metallicFactor, 0.0f, 1.0f);
	ImGui::CheckboxFlags("Use Roughness", &m_materialCD.useRoughness, 1);
	ImGui::SliderFloat("Roughness", &m_materialCD.roughnessFactor, 0.0f, 1.0f);

	ImGui::SliderFloat("Alpha", &m_fAlpha, 0.0f, 1.0f);
	ImGui::SliderFloat("Exposure", &m_globalCD.exposure, 0.0f, 5.0f);
	ImGui::SliderFloat("Gamma", &m_globalCD.gamma, 0.0f, 5.0f);
	ImGui::SliderFloat("AmbientFactor", &m_materialCD.ambientFactor, 0.0f, 1.0f);

	ImGui::SliderFloat3("LightStrength", &m_globalCD.light.lightStrength.x, 0.0f, 20.0f);
	if (m_vecLights[0])
	{
		ImGui::SliderFloat3("LightPos", &m_vecLights[0]->m_translation.x, -5.0f, 5.0f);
		ImGui::SliderFloat("SpotFactor", &m_vecLights[0]->m_fSpotFactor, 0.0f, 10.0f);
		ImGui::SliderFloat("Light Radius", &m_vecLights[0]->m_fRadius, 0.0f, 10.0f);
	}
	ImGui::SliderFloat3("HaloFactor", &m_globalCD.light.haloFactor.x, 0.0f, 10.0f);
	

	// ImGui::SliderFloat("fallOfStart", &m_pixelConstantData.light.fallOfStart, 0.0f, 5.0f);
	// ImGui::SliderFloat("fallOfEnd", &m_pixelConstantData.light.fallOfEnd, 0.0f, 10.0f);
}
