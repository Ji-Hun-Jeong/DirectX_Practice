#include "pch.h"
#include "NonObject.h"

NonObject::NonObject()
	: Mesh()
{
}

NonObject::NonObject(const Vector3& translation, const Vector3& rotation1, const Vector3& rotation2, const Vector3& scale, D3D11_PRIMITIVE_TOPOLOGY topology)
	: Mesh(translation, rotation1, rotation2, scale, topology)
{
}
