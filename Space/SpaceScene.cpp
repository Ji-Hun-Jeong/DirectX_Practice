#include "pch.h"
#include "SpaceScene.h"
#include "GeometryGenerator.h"
#include "DirArrow.h"

SpaceScene::SpaceScene(SceneMgr* owner)
	: RenderScene(owner)
{
}

void SpaceScene::Enter()
{

}

void SpaceScene::Exit()
{

}

void SpaceScene::Update(float dt)
{
	RenderScene::Update(dt);
}

void SpaceScene::UpdateGUI()
{
}



void SpaceScene::InitMesh()
{
	return;
	MeshData solarData = GeometryGenerator::MakeSphere(109, 30, 30);
	auto solar = make_shared<Mesh>("Solar", Vector3(0.0f), Vector3{ 0.0f,0.01f,0.0f }, Vector3{ 0.0f,0.0f,0.0f }, Vector3(1.0f));
	solar->Init(solarData);
	solar->ReadImage("image/Solar.jpg", TEXTURE_TYPE::ALBEDO);
	// solar->GetPixelConstantData().isSun = 1;
	m_vecMesh.push_back(solar);

	MeshData mercuryData = GeometryGenerator::MakeSphere(0.38f, 30, 30);
	auto mercury = make_shared<Mesh>("Mercury", Vector3(111.0f, 0.0f, 0.0f), Vector3{ 0.0f,0.000015f,0.000614f }, Vector3{ 0.0f,0.1f,0.0f }, Vector3(1.0f));
	mercury->Init(mercuryData);
	mercury->ReadImage("image/Mercury.jpg", TEXTURE_TYPE::ALBEDO);
	m_vecMesh.push_back(mercury);

	MeshData venusData = GeometryGenerator::MakeSphere(0.94f, 30, 30);
	auto venus = make_shared<Mesh>("Venus", Vector3(112.6f, 0.0f, 0.0f), Vector3{ 0.0f,0.000001f,3.096f }, Vector3{ 0.0f,0.073f,0.0f }, Vector3(1.0f));
	venus->Init(venusData);
	venus->ReadImage("image/Venus.jpg", TEXTURE_TYPE::ALBEDO);
	m_vecMesh.push_back(venus);

	MeshData earthData = GeometryGenerator::MakeSphere(1.0f, 30, 30);
	auto earth = make_shared<Mesh>("Earth", Vector3(114.16f, 0.0f, 0.0f), Vector3{ 0.0f,0.0023f,0.4091f }, Vector3{ 0.0f,0.062f,0.0f }, Vector3(1.0f));
	earth->Init(earthData);
	earth->ReadImage("image/earth.jpg", TEXTURE_TYPE::ALBEDO);
	m_vecMesh.push_back(earth);

	MeshData marsData = GeometryGenerator::MakeSphere(0.53f, 30, 30);
	auto mars = make_shared<Mesh>("Mars", Vector3(116.84f, 0.0f, 0.0f), Vector3{ 0.0f,0.0012f,0.4396f }, Vector3{ 0.0f,0.05f,0.0f }, Vector3(1.0f));
	mars->Init(marsData);
	mars->ReadImage("image/Mars.jpg", TEXTURE_TYPE::ALBEDO);
	m_vecMesh.push_back(mars);

	MeshData jupiterData = GeometryGenerator::MakeSphere(10.97f, 30, 30);
	auto jupiter = make_shared<Mesh>("Jupiter", Vector3(135.86f, 0.0f, 0.0f), Vector3{ 0.0f,0.063f,0.0546f }, Vector3{ 0.0f,0.027f,0.0f }, Vector3(1.0f));
	jupiter->Init(jupiterData);
	jupiter->ReadImage("image/Jupiter.jpg", TEXTURE_TYPE::ALBEDO);
	m_vecMesh.push_back(jupiter);

	MeshData saturnData = GeometryGenerator::MakeSphere(9.14f, 100, 100);
	auto saturn = make_shared<Mesh>("Saturn", Vector3(158.4f, 0.0f, 0.0f), Vector3{ 0.0f,0.05f,0.4665f }, Vector3{ 0.0f,0.02f,0.0f }, Vector3(1.0f));
	saturn->Init(saturnData);
	saturn->ReadImage("image/Saturn.jpg", TEXTURE_TYPE::ALBEDO);

	MeshData saturnRingData = GeometryGenerator::MakeDisc(10.0f, 20.0f, 50);
	auto saturnRing1 = make_shared<Mesh>("SaturnRing1", Vector3(0.0f), Vector3{ -45.0f,0.5f,0.0f }, Vector3(0.0f), Vector3(1.0f));
	saturnRing1->Init(saturnRingData);
	saturnRing1->ReadImage("image/SaturnRing.jpg", TEXTURE_TYPE::ALBEDO);
	saturn->AttachMesh("Saturn", saturnRing1);
	auto saturnRing2 = make_shared<Mesh>("SaturnRing2", Vector3{ 0.0f,-0.01f,0.0f }, Vector3{ 135.0f,0.5f,0.0f }, Vector3(0.0f), Vector3(1.0f));
	saturnRing2->Init(saturnRingData);
	saturnRing2->ReadImage("image/SaturnRing.jpg", TEXTURE_TYPE::ALBEDO);
	saturn->AttachMesh("Saturn", saturnRing2);
	m_vecMesh.push_back(saturn);

	MeshData uranusData = GeometryGenerator::MakeSphere(3.98f, 30, 30);
	auto uranus = make_shared<Mesh>("Uranus", Vector3(208.1f, 0.0f, 0.0f), Vector3{ 0.0f,0.0129f,1.7064f }, Vector3{ 0.0f,0.014f,0.0f }, Vector3(1.0f));
	uranus->Init(uranusData);
	uranus->ReadImage("image/Uranus.jpg", TEXTURE_TYPE::ALBEDO);
	m_vecMesh.push_back(uranus);

	MeshData neptuneData = GeometryGenerator::MakeSphere(3.86f, 30, 30);
	auto neptune = make_shared<Mesh>("Neptune", Vector3(264.68f, 0.0f, 0.0f), Vector3{ 0.0f,0.0134f,0.4943f }, Vector3{ 0.0f,0.011f,0.0f }, Vector3(1.0f));
	neptune->Init(neptuneData);
	neptune->ReadImage("image/Neptune.jpg", TEXTURE_TYPE::ALBEDO);
	m_vecMesh.push_back(neptune);

	MeshData groundData = GeometryGenerator::MakeSquare();
	auto ground = make_shared<Mesh>("Ground", Vector3{ 0.0f,-200.0f,0.0f }, Vector3{ 90.0f,0.0f,0.0f }, Vector3(0.0f), Vector3(200.0f));
	ground->Init(groundData);
	ground->ReadImage("image/earth.jpg", TEXTURE_TYPE::ALBEDO);
	m_vecMesh.push_back(ground);

	/*MeshData arrowData = GeometryGenerator::MakeTriangle();
	auto dirArrowMesh = make_shared<DirArrow>(Vector3{ 0.9f,-0.7f,1.0f }, Vector3(0.0f), Vector3(0.0f), Vector3(1.0f));
	dirArrowMesh->Init(arrowData, L"DirArrow", L"DirArrow");
	m_vecNonObj.push_back(dirArrowMesh);*/

	m_focusObj = solar;
}

void SpaceScene::InitIBL()
{
}

void SpaceScene::InitSkyBox()
{
}
