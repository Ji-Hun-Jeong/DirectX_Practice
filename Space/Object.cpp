#include "pch.h"
#include "Object.h"
#include "D3DUtils.h"
#include "Camera.h"
#include "SceneMgr.h"
#include "Scene.h"

Object::Object()
	: Mesh()
	, m_normalTopology(D3D_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_POINTLIST)
	, m_ownerObj(nullptr)
{
}

Object::Object(const string& strName, const Vector3& translation, const Vector3& rotation1, const Vector3& rotation2, const Vector3& scale, D3D11_PRIMITIVE_TOPOLOGY topology)
	: Mesh(translation, rotation1, rotation2, scale, topology)
	, m_strName(strName)
	, m_normalTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_POINTLIST)
	, m_ownerObj(nullptr)
{
}

void Object::Init(const MeshData& meshData, const wstring& vertexShaderPrefix, const wstring& pixelShaderPrefix)
{
	Mesh::Init(meshData, vertexShaderPrefix, pixelShaderPrefix);

	m_arrSRV[(UINT)TEXTURE_TYPE::SPECULAR] = GETCURSCENE()->GetIBLSRV(TEXTURE_TYPE::SPECULAR);
	m_arrTexture[(UINT)TEXTURE_TYPE::SPECULAR] = GETCURSCENE()->GetIBLTexture(TEXTURE_TYPE::SPECULAR);

	m_arrSRV[(UINT)TEXTURE_TYPE::IRRADIANCE] = GETCURSCENE()->GetIBLSRV(TEXTURE_TYPE::IRRADIANCE);
	m_arrTexture[(UINT)TEXTURE_TYPE::IRRADIANCE] = GETCURSCENE()->GetIBLTexture(TEXTURE_TYPE::IRRADIANCE);

	m_arrSRV[(UINT)TEXTURE_TYPE::LUT] = GETCURSCENE()->GetIBLSRV(TEXTURE_TYPE::LUT);
	m_arrTexture[(UINT)TEXTURE_TYPE::LUT] = GETCURSCENE()->GetIBLTexture(TEXTURE_TYPE::LUT);

	D3DUtils::GetInst().CreateConstantBuffer<NormalConstantData>(m_normalConstantData, m_normalConstantBuffer);
}

bool Object::IsCollision(const MyRay& ray)
{
	return false;
}

void Object::Update(float dt)
{
	Mesh::Update(dt);
	this->UpdateNormalConstantData();
	D3DUtils::GetInst().UpdateBuffer<NormalConstantData>(m_normalConstantBuffer, m_normalConstantData);
	for (shared_ptr<Object>& childObj : m_vecObj)
	{
		childObj->Update(dt);
	}
}

void Object::Render(ComPtr<ID3D11DeviceContext>& context, const ComPtr<ID3D11Buffer>& viewProjBuffer)
{
	Mesh::Render(context, viewProjBuffer);
	for (shared_ptr<Object>& childObj : m_vecObj)
		childObj->Render(context, viewProjBuffer);
}


void Object::DrawNormal(ComPtr<ID3D11DeviceContext>& context, const ComPtr<ID3D11Buffer>& viewProjBuffer)
{
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	context->IASetVertexBuffers(0, 1, m_vertexBuffer.GetAddressOf(), &stride, &offset);
	context->IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

	vector<ComPtr<ID3D11Buffer>> vertexCB =
	{ m_normalConstantBuffer ,viewProjBuffer };
	context->VSSetConstantBuffers(0, vertexCB.size(), vertexCB.data()->GetAddressOf());
	context->VSSetShaderResources(0, 1, m_arrSRV[(UINT)TEXTURE_TYPE::NORMAL].GetAddressOf());
	context->VSSetSamplers(0, 1, m_samplerState.GetAddressOf());

	context->GSSetConstantBuffers(0, vertexCB.size(), vertexCB.data()->GetAddressOf());

	context->DrawIndexed(m_indexCount, 0, 0);
}

void Object::UpdateVertexConstantData(float dt)
{
	float time = GetTic(dt);
	Quaternion rotateX = Quaternion::CreateFromAxisAngle(Vector3{ 1.0f,0.0f,0.0f }, m_rotation1.x);
	Quaternion rotateY = Quaternion::CreateFromAxisAngle(Vector3{ 0.0f,1.0f,0.0f }, m_rotation1.y);
	Quaternion rotateZ = Quaternion::CreateFromAxisAngle(Vector3{ 0.0f,0.0f,1.0f }, m_rotation1.z);
	m_vertexConstantData.model =
		Matrix::CreateScale(m_scale)
		* Matrix::CreateFromQuaternion(rotateY)
		* Matrix::CreateFromQuaternion(rotateX)
		* Matrix::CreateFromQuaternion(rotateZ)
		* Matrix::CreateTranslation(m_translation)
		* Matrix::CreateRotationX(m_rotation2.x * time)
		* Matrix::CreateRotationY(m_rotation2.y * time)
		* Matrix::CreateRotationZ(m_rotation2.z * time);

	if (m_ownerObj)
	{
		m_vertexConstantData.model *= m_ownerObj->m_prevTransformModel;
	}
	m_prevTransformModel = m_vertexConstantData.model;

	if (m_pixelConstantData.isLight)
	{
		m_vertexConstantData.model = Matrix::CreateTranslation(m_pixelConstantData.light.lightPos);
	}

	m_vertexConstantData.invTranspose = m_vertexConstantData.model;
	m_vertexConstantData.invTranspose.Translation(Vector3(0.0f));
	m_vertexConstantData.invTranspose = m_vertexConstantData.invTranspose.Invert().Transpose();

	m_vertexConstantData.model = m_vertexConstantData.model.Transpose();
	m_vertexConstantData.invTranspose = m_vertexConstantData.invTranspose.Transpose();

	m_vertexConstantData.useHeight = GETCURSCENE()->m_useHeight;
	m_vertexConstantData.heightScale = GETCURSCENE()->m_heightScale;
}

void Object::UpdatePixelConstantData()
{
	Mesh::UpdatePixelConstantData();
}

void Object::UpdateNormalConstantData()
{
	m_normalConstantData.model = m_vertexConstantData.model;
	m_normalConstantData.invTranspose = m_vertexConstantData.invTranspose;

	m_normalConstantData.normalSize = GETCURSCENE()->m_normalSize;
	m_normalConstantData.useNormal = GETCURSCENE()->m_pixelConstantData.useNormal;
}

bool Object::AttachObject(const string& meshName, shared_ptr<Object> childObj)
{
	if (m_strName == meshName)
	{
		m_vecObj.push_back(childObj);
		childObj->m_ownerObj = this;
		return true;
	}
	else
	{
		bool findChild = false;
		for (auto& mesh : m_vecObj)
		{
			findChild = mesh->AttachObject(meshName, childObj);
			if (findChild)
				break;
		}
		return findChild;
	}
}
