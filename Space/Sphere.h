#pragma once
#include "Object.h"
class Sphere :
    public Object
{
public:
    Sphere();
    explicit Sphere(const string& strName, const Vector3& translation, const Vector3& rotation1, const Vector3& rotation2, const Vector3& scale, D3D11_PRIMITIVE_TOPOLOGY topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
private:
    float m_fRadius;
};

