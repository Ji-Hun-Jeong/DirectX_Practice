#include "pch.h"
#include "DirArrow.h"
#include "D3DUtils.h"
#include "Camera.h"
#include "Core.h"
#include "Scene.h"
#include "SceneMgr.h"

DirArrow::DirArrow(const Vector3& translation, const Vector3& rotation1, const Vector3& rotation2, const Vector3& scale, D3D11_PRIMITIVE_TOPOLOGY topology)
	: NonObject(translation, rotation1, rotation2, scale, topology)
{
	
}

void DirArrow::Init(const MeshData& meshData, const wstring& vertexShaderPrefix, const wstring& pixelShaderPrefix)
{
	Mesh::Init(meshData,vertexShaderPrefix,pixelShaderPrefix);
	CreateGeometryShader(L"DirArrow", m_geometryShader);
	D3DUtils::GetInst().CreateConstantBuffer<DirArrowConstantData>(m_dirArrowConstantData, m_vertexConstantBuffer);
}

void DirArrow::Update(float dt)
{
	this->UpdateVertexConstantData(dt);
	D3DUtils::GetInst().UpdateBuffer<DirArrowConstantData>(m_vertexConstantBuffer, m_dirArrowConstantData);
}

void DirArrow::UpdateVertexConstantData(float dt)
{
	//Mesh::UpdateVertexConstantData(dt);
	Core& core = Core::GetInst();
	auto& camera = GETCAMERA();
	m_dirArrowConstantData.model = Matrix::CreateTranslation(m_translation);
	m_dirArrowConstantData.model = m_dirArrowConstantData.model.Transpose();

	m_dirArrowConstantData.view = Matrix();
	m_dirArrowConstantData.view = m_dirArrowConstantData.view.Transpose();

	float angleY = camera->GetAngleY();
	float aspect = core.GetAspect();
	float nearZ = camera->GetNearZ();
	float farZ = camera->GetFarZ();

	m_dirArrowConstantData.projection = XMMatrixOrthographicLH(2 * aspect, 2, nearZ, farZ);
	m_dirArrowConstantData.projection = m_dirArrowConstantData.projection.Transpose();

	m_dirArrowConstantData.viewDir = camera->GetViewDir();
	m_dirArrowConstantData.rightDir = camera->GetRightDir();
	m_dirArrowConstantData.upDir = camera->GetUpDir();
}

void DirArrow::ReadyToRender(ID3D11DeviceContext* context)
{
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	context->IASetVertexBuffers(0, 1, m_vertexBuffer.GetAddressOf(), &stride, &offset);
	context->IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT_R8G8B8A8_UINT, 0);
	context->IASetPrimitiveTopology(m_topology);
	context->IASetInputLayout(m_inputLayout.Get());

	context->VSSetShader(m_vertexShader.Get(), nullptr, 0);
	context->VSSetConstantBuffers(0, 1, m_vertexConstantBuffer.GetAddressOf());
	
	context->GSSetShader(m_geometryShader.Get(), nullptr, 0);
	context->GSSetConstantBuffers(0, 1, m_vertexConstantBuffer.GetAddressOf());

	context->PSSetShader(m_pixelShader.Get(), nullptr, 0);
}
