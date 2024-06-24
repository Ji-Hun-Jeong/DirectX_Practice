#include "pch.h"
#include "Mesh.h"
#include "D3DUtils.h"
#include "Core.h"
#include "Camera.h"

Mesh::Mesh()
	: Mesh(Vector3(0.0f), Vector3(0.0f), Vector3(0.0f), Vector3(1.0f), D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST)
{
}

Mesh::Mesh(const Vector3& translation, const Vector3& rotation1, const Vector3& rotation2, const Vector3& scale, D3D11_PRIMITIVE_TOPOLOGY topology)
	: m_indexCount(0)
	, m_translation(translation)
	, m_rotation1(rotation1* XM_PI / 180.0f)
	, m_rotation2(rotation2* XM_PI / 180.0f)
	, m_scale(scale)
	, m_topology(topology)
{
}
void Mesh::Init(const MeshData& meshData, const wstring& vertexShaderPrefix, const wstring& pixelShaderPrefix)
{
	D3DUtils::GetInst().CreateVertexBuffer<Vertex>(meshData.vertices, m_vertexBuffer);
	m_indexCount = UINT(meshData.indices.size());
	D3DUtils::GetInst().CreateIndexBuffer<uint32_t>(meshData.indices, m_indexBuffer);
	D3DUtils::GetInst().CreateConstantBuffer<VertexConstantData>(m_vertexConstantData, m_vertexConstantBuffer);
	D3DUtils::GetInst().CreateConstantBuffer<PixelConstantData>(m_pixelConstantData, m_pixelConstantBuffer);
	D3DUtils::GetInst().CreateSamplerState(m_samplerState);
	if (!meshData.textureName.empty())
	{
		// ShaderResourceView
		ReadImage(meshData.textureName);
	}

	CreateVertexShaderAndInputLayout(vertexShaderPrefix, m_vertexShader);
	CreatePixelShader(pixelShaderPrefix, m_pixelShader);
	// CreateGeometryShader(L"Basic", m_geometryShader);
}
void Mesh::Update(float dt)
{
	this->UpdateVertexConstantData(dt);
	this->UpdatePixelConstantData();
	D3DUtils::GetInst().UpdateBuffer<VertexConstantData>(m_vertexConstantBuffer, m_vertexConstantData);
	D3DUtils::GetInst().UpdateBuffer<PixelConstantData>(m_pixelConstantBuffer, m_pixelConstantData);
}

void Mesh::UpdateVertexConstantData(float dt)
{
	float time = GetTic(dt);
	Core& core = Core::GetInst();

	Quaternion rotateX = Quaternion::CreateFromAxisAngle(Vector3{ 1.0f,0.0f,0.0f }, m_rotation1.x);
	Quaternion rotateY = Quaternion::CreateFromAxisAngle(Vector3{ 0.0f,1.0f,0.0f }, m_rotation1.y * time);
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

	m_vertexConstantData.invTranspose = m_vertexConstantData.model;
	m_vertexConstantData.invTranspose.Translation(Vector3(0.0f));
	m_vertexConstantData.invTranspose = m_vertexConstantData.invTranspose.Invert().Transpose();

	m_vertexConstantData.model = m_vertexConstantData.model.Transpose();
	m_vertexConstantData.invTranspose = m_vertexConstantData.invTranspose.Transpose();

	m_vertexConstantData.view = Core::GetInst().GetCamera()->GetViewRow();
	m_vertexConstantData.view = m_vertexConstantData.view.Transpose();

	float angleY = core.GetAngleY();
	float aspect = core.GetAspect();
	float nearZ = core.GetNearZ();
	float farZ = core.GetFarZ();
	m_vertexConstantData.projection = XMMatrixPerspectiveFovLH(angleY, aspect, nearZ, farZ);
	m_vertexConstantData.projection = m_vertexConstantData.projection.Transpose();
}

void Mesh::UpdatePixelConstantData()
{
	m_pixelConstantData = Core::GetInst().m_pixelConstantData;
	//m_pixelConstantData.mat = Core::GetInst().m_allMat;
	//m_pixelConstantData.eyePos = Core::GetInst().GetCamera()->GetPos();
}

void Mesh::Render(ID3D11DeviceContext* context)
{
	ReadyToRender(context);
	context->DrawIndexed(m_indexCount, 0, 0);
}

void Mesh::ReadyToRender(ID3D11DeviceContext* context)
{
	Core& core = Core::GetInst();
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	context->OMSetRenderTargets(1, core.GetRenderTargetView().GetAddressOf(), core.GetDepthStencilView().Get());
	context->IASetPrimitiveTopology(m_topology);
	context->IASetVertexBuffers(0, 1, m_vertexBuffer.GetAddressOf(), &stride, &offset);
	context->IASetInputLayout(m_inputLayout.Get());
	context->IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

	context->VSSetShader(m_vertexShader.Get(), nullptr, 0);
	context->VSSetConstantBuffers(0, 1, m_vertexConstantBuffer.GetAddressOf());

	context->GSSetShader(m_geometryShader.Get(), nullptr, 0);
	context->GSSetConstantBuffers(0, 1, m_vertexConstantBuffer.GetAddressOf());

	context->PSSetShader(m_pixelShader.Get(), nullptr, 0);
	context->PSSetConstantBuffers(0, 1, m_pixelConstantBuffer.GetAddressOf());
	context->PSSetSamplers(0, 1, m_samplerState.GetAddressOf());
	context->PSSetShaderResources(0, (UINT)m_vecShaderResourceViews.size(), m_vecShaderResourceViews.data()->GetAddressOf());
}



void Mesh::CreateVertexShaderAndInputLayout(const wstring& hlslPrefix, ComPtr<ID3D11VertexShader>& vertexShader)
{
	vector<D3D11_INPUT_ELEMENT_DESC> inputElements =
	{
		{"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 4 * 3, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 4 * 3 + 4 * 3, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};
	D3DUtils::GetInst().CreateVertexShaderAndInputLayout(hlslPrefix, vertexShader, inputElements, m_inputLayout);
}

void Mesh::CreatePixelShader(const wstring& hlslPrefix, ComPtr<ID3D11PixelShader>& pixelShader)
{
	D3DUtils::GetInst().CreatePixelShader(hlslPrefix, pixelShader);
}

void Mesh::CreateGeometryShader(const wstring& hlslPrefix, ComPtr<ID3D11GeometryShader>& geometryShader)
{
	D3DUtils::GetInst().CreateGeometryShader(hlslPrefix, geometryShader);
}

void Mesh::ReadImage(const string& textureName)
{
	ComPtr<ID3D11Texture2D> texture;
	ComPtr<ID3D11ShaderResourceView> shaderResourceView;
	D3DUtils::GetInst().ReadImage(textureName, texture, shaderResourceView);
	m_vecShaderResourceViews.push_back(shaderResourceView);
}

float Mesh::GetTic(float dt)
{
	static float time = 0.0f;
	time += dt;
	if (time < 0)
		time = 0;
	return time;
}

