#include "pch.h"
#include "Light.h"
#include "D3DUtils.h"
#include "Core.h"
#include "KeyMgr.h"

Light::Light()
	: Light("", Vector3(0.0f, 2.0f, -2.0f), Vector3(0.0f), 1.0f)
{
}

Light::Light(const string& strName, const Vector3& pos, const Vector3& viewPos, float radius)
	: Mesh(strName, pos, Vector3(0.0f), Vector3(0.0f), Vector3(radius))
	, m_viewPos(viewPos)
	, m_fRadius(radius)
{

}

void Light::Init(const MeshData& meshData)
{
	Mesh::Init(meshData);
	
	D3DUtils::GetInst().CreateDepthOnlyResources(Core::GetInst().m_fWidth, Core::GetInst().m_fHeight, m_depthBuffer, m_DSV, m_SRV);
}

void Light::UpdateMeshConstantData(float dt)
{
	float time = GetTic(dt);
	Mesh::UpdateMeshConstantData(dt);

	Vector3 pos = m_translation;
	m_viewDir = m_viewPos - pos;
	m_viewDir.Normalize();
	Vector3 firstViewDir = Vector3{ 0.0f,0.0f,1.0f };

	Quaternion rotation = Quaternion::FromToRotation(firstViewDir, m_viewDir);

	m_view = Matrix::CreateTranslation(-pos) *
		Matrix::CreateFromQuaternion(rotation).Invert();

	m_proj = XMMatrixPerspectiveFovLH(XMConvertToRadians(m_angleY),
		Core::GetInst().GetAspect(), m_nearZ, m_farZ);

	m_viewProj = m_view * m_proj;

	m_view = m_view.Transpose();
	m_proj = m_proj.Transpose();
	m_viewProj = m_viewProj.Transpose();
}

void Light::UpdateMaterialConstantData()
{
	Mesh::UpdateMaterialConstantData();
	m_materialConstData.isLight = true;
}