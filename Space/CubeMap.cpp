#include "pch.h"
#include "CubeMap.h"
#include "D3DUtils.h"

CubeMap::CubeMap(const Vector3& translation, const Vector3& rotation1, const Vector3& rotation2, const Vector3& scale)
	: Mesh(translation, rotation1, rotation2, scale, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST)
{
	
}

CubeMap::CubeMap()
	: Mesh(Vector3(0.0f),Vector3(0.0f),Vector3(0.0f), Vector3(0.0f), D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST)
{
}

void CubeMap::UpdateVertexConstantData(float dt)
{
	Mesh::UpdateVertexConstantData(dt);
	m_vertexConstantData.model = Matrix();
	m_vertexConstantData.view = Matrix();
}

void CubeMap::ReadyToRender(ID3D11DeviceContext* context)
{
	Mesh::ReadyToRender(context);
	context->PSSetShaderResources(0, 1, m_cubeMapResource.GetAddressOf());
}

void CubeMap::ReadImage(const string& textureName)
{
	wstring fileName = wstring().assign(textureName.begin(), textureName.end());
	ComPtr<ID3D11Texture2D> texture;
	ComPtr<ID3D11Device> device = D3DUtils::GetInst().GetDevice();
	CreateDDSTextureFromFileEx(device.Get(), fileName.c_str(), 0
		, D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE
		, 0, D3D11_RESOURCE_MISC_TEXTURECUBE, DDS_LOADER_FLAGS(false), (ID3D11Resource**)texture.GetAddressOf()
		, m_cubeMapResource.GetAddressOf(), nullptr);
}
