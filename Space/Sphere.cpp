#include "pch.h"
#include "Sphere.h"

Sphere::Sphere()
	: Object()
	, m_fRadius(0.0f)
{
}

Sphere::Sphere(const string& strName, const Vector3& translation, const Vector3& rotation1, const Vector3& rotation2, const Vector3& scale, D3D11_PRIMITIVE_TOPOLOGY topology)
	: Object(strName, translation, rotation1, rotation2, scale, topology)
	, m_fRadius(0.0f)
{
}

void Sphere::Init(const MeshData& meshData, const wstring& vertexShaderPrefix, const wstring& pixelShaderPrefix)
{
	Object::Init(meshData, vertexShaderPrefix, pixelShaderPrefix);
	m_fRadius = (meshData.vertices[0].position - Vector3(0.0f)).Length();
}

bool Sphere::IsCollision(const MyRay& ray)
{
	Vector3 center = Vector3::Transform(Vector3(0.0f), m_vertexConstantData.model.Transpose());
	Vector3 oc = ray.startPos - center;
	float b = ray.rayDir.Dot(oc);
	float c = oc.Dot(oc) - m_fRadius * m_fRadius;
	if (b * b - c < 0)
		return false;
	return true;
}

