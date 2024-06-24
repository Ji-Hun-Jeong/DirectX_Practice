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
