#include "pch.h"
#include "Mirror.h"
#include "Scene.h"
#include "SceneMgr.h"
#include "D3DUtils.h"

Mirror::Mirror()
{
}

Mirror::Mirror(const string& strName, const Vector3& translation, const Vector3& rotation1, const Vector3& rotation2, const Vector3& scale, D3D11_PRIMITIVE_TOPOLOGY topology)
	: Object(strName, translation, rotation1, rotation2, scale, topology)
{
	m_normalDir = Vector3::Transform(Vector3(0.0f, 0.0f, -1.0f)
		, Matrix::CreateRotationY(m_rotation1.y)
		* Matrix::CreateRotationX(m_rotation1.x));
	m_mirrorPlane = DirectX::SimpleMath::Plane(m_translation, m_normalDir);
}

void Mirror::Init(const MeshData& meshData, const wstring& vertexShaderPrefix, const wstring& pixelShaderPrefix)
{
	Object::Init(meshData, vertexShaderPrefix, pixelShaderPrefix);
	D3DUtils::GetInst().CreateConstantBuffer<Matrix>(m_mirrorViewProj, m_viewProjBuffer);
}

void Mirror::Update(float dt)
{
	Object::Update(dt);
	Matrix reflection = Matrix::CreateReflection(m_mirrorPlane);
	m_mirrorViewProj = reflection * GETCURSCENE()->m_viewProj;
	m_mirrorViewProj = m_mirrorViewProj.Transpose();
	D3DUtils::GetInst().UpdateBuffer<Matrix>(m_viewProjBuffer, m_mirrorViewProj);
}
