#include "pch.h"
#include "Mesh.h"
#include "D3DUtils.h"
#include "Camera.h"
#include "Scene.h"
#include "SceneMgr.h"

Mesh::Mesh()
	: Mesh("", Vector3(0.0f), Vector3(0.0f), Vector3(0.0f), Vector3(1.0f))
{
}

Mesh::Mesh(const string& strName, const Vector3& translation, const Vector3& rotation1, const Vector3& rotation2, const Vector3& scale)
	: m_strName(strName)
	, m_ownerObj(nullptr)
	, m_indexCount(0)
	, m_translation(translation)
	, m_rotation1(rotation1* XM_PI / 180.0f)
	, m_rotation2(rotation2* XM_PI / 180.0f)
	, m_scale(scale)
{
}
void Mesh::Init(const MeshData& meshData)
{
	D3DUtils::GetInst().CreateVertexBuffer<Vertex>(meshData.vertices, m_vertexBuffer);
	D3DUtils::GetInst().CreateIndexBuffer<uint32_t>(meshData.indices, m_indexBuffer);
	m_indexCount = UINT(meshData.indices.size());

	D3DUtils::GetInst().CreateConstantBuffer<MeshConstData>(m_meshConstData, m_meshConstBuffer);
	D3DUtils::GetInst().CreateConstantBuffer<MaterialConstData>(m_materialConstData, m_materialConstBuffer);
	D3DUtils::GetInst().CreateConstantBuffer<CommonConstData>(m_commonConstData, m_commonConstBuffer);
	// CreateGeometryShader(L"Basic", m_geometryShader);
}
void Mesh::Update(float dt)
{
	this->UpdateMeshConstantData(dt);
	this->UpdateMaterialConstantData();
	this->UpdateCommonConstantData();
	D3DUtils::GetInst().UpdateBuffer<MeshConstData>(m_meshConstData, m_meshConstBuffer);
	D3DUtils::GetInst().UpdateBuffer<MaterialConstData>(m_materialConstData, m_materialConstBuffer);
	D3DUtils::GetInst().UpdateBuffer<CommonConstData>(m_commonConstData, m_commonConstBuffer);

	for (shared_ptr<Mesh>& childObj : m_vecObj)
	{
		childObj->Update(dt);
	}
}

bool Mesh::IsCollision(MyRay ray)
{
	return false;
}

void Mesh::UpdateMeshConstantData(float dt)
{
	float time = GetTic(dt);

	Quaternion rotateX = Quaternion::CreateFromAxisAngle(Vector3{ 1.0f,0.0f,0.0f }, m_rotation1.x);
	Quaternion rotateY = Quaternion::CreateFromAxisAngle(Vector3{ 0.0f,1.0f,0.0f }, m_rotation1.y);
	Quaternion rotateZ = Quaternion::CreateFromAxisAngle(Vector3{ 0.0f,0.0f,1.0f }, m_rotation1.z);

	m_meshConstData.world =
		Matrix::CreateScale(m_scale)
		* Matrix::CreateFromQuaternion(rotateY)
		* Matrix::CreateFromQuaternion(rotateX)
		* Matrix::CreateFromQuaternion(rotateZ)
		* Matrix::CreateTranslation(m_translation)
		* Matrix::CreateRotationX(m_rotation2.x)
		* Matrix::CreateRotationY(m_rotation2.y * time)
		* Matrix::CreateRotationZ(m_rotation2.z);

	if (m_ownerObj)
		m_meshConstData.world *= m_ownerObj->m_prevTransformModel;

	m_prevTransformModel = m_meshConstData.world;

	m_meshConstData.worldIT = m_meshConstData.world;
	m_meshConstData.worldIT.Translation(Vector3(0.0f));
	m_meshConstData.worldIT = m_meshConstData.worldIT.Invert().Transpose();

	m_meshConstData.world = m_meshConstData.world.Transpose();
	m_meshConstData.worldIT = m_meshConstData.worldIT.Transpose();
}

void Mesh::UpdateMaterialConstantData()
{
	shared_ptr<Scene>& curScene = GETCURSCENE();
	m_materialConstData = curScene->m_materialCD;
	if (!m_arrSRV[(UINT)TEXTURE_TYPE::ALBEDO])
		m_materialConstData.useAlbedo = false;
	if(!m_arrSRV[(UINT)TEXTURE_TYPE::AO])
		m_materialConstData.useAO = false;
	if (!m_arrSRV[(UINT)TEXTURE_TYPE::EMISSIVE])
		m_materialConstData.useEmissive = false;
	if (!m_arrSRV[(UINT)TEXTURE_TYPE::METAL])
		m_materialConstData.useMetallic = false;
	if (!m_arrSRV[(UINT)TEXTURE_TYPE::ROUGHNESS])
		m_materialConstData.useRoughness = false;
}

void Mesh::UpdateCommonConstantData()
{
	m_commonConstData = GETCURSCENE()->m_commonCD;
	if (!m_arrSRV[(UINT)TEXTURE_TYPE::HEIGHT])
		m_commonConstData.useHeight = false;
	if (!m_arrSRV[(UINT)TEXTURE_TYPE::NORMAL])
		m_commonConstData.useNormal = false;
}

void Mesh::Render(ComPtr<ID3D11DeviceContext>& context)
{
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	context->IASetVertexBuffers(0, 1, m_vertexBuffer.GetAddressOf(), &stride, &offset);
	context->IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

	context->VSSetConstantBuffers(1, 1, m_commonConstBuffer.GetAddressOf());
	context->VSSetConstantBuffers(2, 1, m_meshConstBuffer.GetAddressOf());
	context->VSSetShaderResources(0, 1, m_arrSRV[(UINT)TEXTURE_TYPE::HEIGHT].GetAddressOf());

	context->GSSetConstantBuffers(1, 1, m_commonConstBuffer.GetAddressOf());
	context->GSSetConstantBuffers(2, 1, m_meshConstBuffer.GetAddressOf());

	context->PSSetConstantBuffers(1, 1, m_commonConstBuffer.GetAddressOf());
	context->PSSetConstantBuffers(2, 1, m_materialConstBuffer.GetAddressOf());
	context->PSSetShaderResources(0, (UINT)TEXTURE_TYPE::END - 1, m_arrSRV[(UINT)TEXTURE_TYPE::ALBEDO].GetAddressOf());
	context->DrawIndexed(m_indexCount, 0, 0);

	for (shared_ptr<Mesh>& childObj : m_vecObj)
		childObj->Render(context);
}

void Mesh::DrawNormal(ComPtr<ID3D11DeviceContext>& context)
{
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	context->IASetVertexBuffers(0, 1, m_vertexBuffer.GetAddressOf(), &stride, &offset);
	context->IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

	context->VSSetConstantBuffers(1, 1, m_commonConstBuffer.GetAddressOf());
	context->VSSetConstantBuffers(2, 1, m_meshConstBuffer.GetAddressOf());
	
	context->GSSetShaderResources(0, 1, m_arrSRV[(UINT)TEXTURE_TYPE::NORMAL].GetAddressOf());
	context->GSSetConstantBuffers(1, 1, m_commonConstBuffer.GetAddressOf());
	context->GSSetConstantBuffers(2, 1, m_meshConstBuffer.GetAddressOf());

	context->DrawIndexed(m_indexCount, 0, 0);
}


void Mesh::ReadImage(const string& textureName, TEXTURE_TYPE textureType, bool useSRGB)
{
	if (textureName == "")
		return;
	ComPtr<ID3D11Texture2D> texture;
	ComPtr<ID3D11ShaderResourceView> shaderResourceView;
	D3DUtils::GetInst().ReadImage(textureName, useSRGB, texture, shaderResourceView);

	m_arrTexture[(UINT)textureType] = texture;
	m_arrSRV[(UINT)textureType] = shaderResourceView;
}


float Mesh::GetTic(float dt)
{
	static float time = 0.0f;
	time += dt;
	if (time < 0)
		time = 0;
	return time;
}

bool Mesh::AttachMesh(const string& meshName, shared_ptr<Mesh> childMesh)
{
	if (m_strName == meshName)
	{
		m_vecObj.push_back(childMesh);
		childMesh->m_ownerObj = this;
		return true;
	}
	else
	{
		bool findChild = false;
		for (auto& mesh : m_vecObj)
		{
			findChild = mesh->AttachMesh(meshName, childMesh);
			if (findChild)
				break;
		}
		return findChild;
	}

}

