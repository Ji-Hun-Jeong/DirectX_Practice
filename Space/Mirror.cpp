#include "pch.h"
#include "Mirror.h"
#include "Scene.h"
#include "SceneMgr.h"
#include "D3DUtils.h"
#include "RenderScene.h"

Mirror::Mirror()
{
}

Mirror::Mirror(const string& strName, const Vector3& translation, const Vector3& rotation1, const Vector3& rotation2, const Vector3& scale)
	: Mesh(strName, translation, rotation1, rotation2, scale)
{

}

void Mirror::Init(const MeshData& meshData)
{
	Mesh::Init(meshData);
}

void Mirror::UpdateMeshConstantData(float dt)
{
	Mesh::UpdateMeshConstantData(dt);
	m_normalDir = Vector3::Transform(Vector3(0.0f, 0.0f, -1.0f)
		, Matrix::CreateRotationY(m_rotation1.y)
		* Matrix::CreateRotationX(m_rotation1.x));
	m_mirrorPlane = DirectX::SimpleMath::Plane(m_translation, m_normalDir);

	Matrix reflection = Matrix::CreateReflection(m_mirrorPlane);
	m_mirrorViewProj = reflection * ((RenderScene*)GETCURSCENE().get())->m_globalCD.viewProj;
	m_mirrorViewProj = m_mirrorViewProj.Transpose();
}

void Mirror::UpdateMaterialConstantData()
{
	Mesh::UpdateMaterialConstantData();
	m_materialConstData.albedoFactor = Vector3(0.0f);
}