#include "pch.h"
#include "CubeMap.h"
#include "D3DUtils.h"

CubeMap::CubeMap(const Vector3& translation, const Vector3& rotation1, const Vector3& rotation2, const Vector3& scale)
	: NonObject(translation, rotation1, rotation2, scale, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST)
{
	
}

CubeMap::CubeMap()
	: NonObject(Vector3(0.0f),Vector3(0.0f),Vector3(0.0f), Vector3(0.0f), D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST)
{
	
}

void CubeMap::UpdateVertexConstantData(float dt)
{
	Mesh::UpdateVertexConstantData(dt);
	m_vertexConstantData.model = Matrix();
}

void CubeMap::ReadyToRender(ComPtr<ID3D11DeviceContext>& context, const ComPtr<ID3D11Buffer>& viewProjBuffer)
{
	Mesh::ReadyToRender(context, viewProjBuffer);
	context->PSSetShaderResources(0, (UINT)TEXTURE_TYPE::IRRADIANCE - (UINT)TEXTURE_TYPE::SPECULAR + 1
		, m_arrSRV[(UINT)TEXTURE_TYPE::SPECULAR].GetAddressOf());
}