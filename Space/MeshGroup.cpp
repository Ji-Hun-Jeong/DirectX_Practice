#include "pch.h"
#include "MeshGroup.h"
#include "Mesh.h"
#include "GeometryGenerator.h"


MeshGroup::MeshGroup(const Vector3& translation, const Vector3& rotation1, const Vector3& rotation2, const Vector3& scale)
	: m_translation(translation)
	, m_rotation1(rotation1)
	, m_rotation2(rotation2)
	, m_scale(scale)
{
	m_rotation1 = m_rotation1 * XM_PI / 180.0f;
	m_rotation2 = m_rotation2 * XM_PI / 180.0f;
}

void MeshGroup::AddMesh(shared_ptr<Mesh> mesh)
{
	m_vecMeshes.push_back(mesh);
}

void MeshGroup::Update(float dt, bool updateNormal)
{
	for (auto& mesh : m_vecMeshes)
	{
		if (mesh)
			mesh->Update(dt);
	}
}

void MeshGroup::Render(ID3D11DeviceContext* context, bool drawNormal)
{
	for (auto& mesh : m_vecMeshes)
	{
		if (mesh)
			mesh->Render(context, drawNormal);
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
