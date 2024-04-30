#include "pch.h"
#include "MeshGroup.h"
#include "Mesh.h"
#include "Normal.h"
#include "GeometryGenerator.h"


MeshGroup::MeshGroup(const Vector3& translation, const Vector3& rotation1, const Vector3& rotation2, const Vector3& scale)
	: m_translation(translation)
	, m_rotation1(rotation1)
	, m_rotation2(rotation2)
	, m_scale(scale)
{
}

void MeshGroup::AddMesh(const string& name, const MeshData& meshData, const wstring& vertexShaderPrefix, const wstring& pixelShaderPrefix)
{
	auto mesh = make_shared<Mesh>(m_translation, m_rotation1, m_rotation2, m_scale);
	mesh->Init(name, meshData, vertexShaderPrefix, pixelShaderPrefix);
	m_vecMeshes.push_back(mesh);

	MeshData normalData = GeometryGenerator::MakeNormal(meshData);
	auto normal = make_shared<Normal>(mesh);
	normal->Init(name + "Normal", normalData, L"Normal", L"Normal");
	m_vecNormals.push_back(normal);
}

void MeshGroup::Update(float dt, bool updateNormal)
{
	for (auto& mesh : m_vecMeshes)
	{
		if (mesh)
			mesh->Update(dt);
	}
	if (updateNormal)
	{
		for (auto& normal : m_vecNormals)
		{
			if (normal)
				normal->Update(dt);
		}
	}
}

void MeshGroup::Render(ID3D11DeviceContext* context, bool drawNormal)
{
	for (auto& mesh : m_vecMeshes)
	{
		if (mesh)
			mesh->Render(context);
	}
	if (drawNormal)
	{
		for (auto& normal : m_vecNormals)
		{
			if (normal)
				normal->Render(context);
		}
	}
}

shared_ptr<Mesh>& MeshGroup::GetMesh(const string& name)
{
	for (auto& mesh : m_vecMeshes)
	{
		if (!mesh)
			continue;
		if (mesh->GetName() == name)
			return mesh;
	}
	assert(0);
}
