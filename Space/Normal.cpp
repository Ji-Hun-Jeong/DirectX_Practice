#include "pch.h"
#include "Normal.h"
#include "Mesh.h"

Normal::Normal(shared_ptr<Mesh> order)
	: Mesh(Vector3(0.0f),Vector3(0.0f),Vector3(0.0f),Vector3(0.0f), D3D_PRIMITIVE_TOPOLOGY_LINELIST)
	, m_pOrder(order)
{}

void Normal::UpdateVertexConstantData(float dt)
{
	m_vertexConstantData = m_pOrder->GetVertexConstantData();
}

