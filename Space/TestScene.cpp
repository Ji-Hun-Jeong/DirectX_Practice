#include "pch.h"
#include "TestScene.h"
#include "GeometryGenerator.h"
#include "Mesh.h"
#include "Sphere.h"

void TestScene::Init()
{
	Scene::Init();
	m_pixelConstantData.light.lightPos = Vector3{ 0.0f,250.0f,0.0f };
}

void TestScene::Enter()
{
}

void TestScene::Exit()
{
}

void TestScene::InitMesh()
{
	MeshData sphereData = GeometryGenerator::MakeSphere(50.0f, 30, 30, "image/PBR/Brick/brick-wall_albedo.png");
	auto sphere = make_shared<Sphere>("sphere", Vector3(0.0f), Vector3(0.0f), Vector3(0.0f), Vector3(1.0f));
	sphere->Init(sphereData, L"Basic", L"Basic");
	m_vecObj.push_back(sphere);
}
