#pragma once
#include "Mesh.h"
class NonObject :
    public Mesh
{
public:
    NonObject();
    explicit NonObject(const Vector3& translation, const Vector3& rotation1, const Vector3& rotation2, const Vector3& scale, D3D11_PRIMITIVE_TOPOLOGY topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
};

