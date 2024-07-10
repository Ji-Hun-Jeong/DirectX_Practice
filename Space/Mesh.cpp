#include "pch.h"
#include "Mesh.h"
#include "D3DUtils.h"
#include "Camera.h"
#include "Scene.h"
#include "SceneMgr.h"

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
	D3DUtils::GetInst().CreateSamplerState(m_samplerState, false);
	D3DUtils::GetInst().CreateSamplerState(m_clampSampler, true);

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

	m_vertexConstantData.view = GETCAMERA()->m_view;
	m_vertexConstantData.view = m_vertexConstantData.view.Transpose();

	m_vertexConstantData.projection = GETCAMERA()->m_projection;
	m_vertexConstantData.projection = m_vertexConstantData.projection.Transpose();

	m_vertexConstantData.heightScale = GETCURSCENE()->m_heightScale;
	m_vertexConstantData.useHeight = GETCURSCENE()->m_useHeight;
}

void Mesh::UpdatePixelConstantData()
{
	shared_ptr<Scene>& curScene = GETCURSCENE();
	m_pixelConstantData.bloom = curScene->m_pixelConstantData.bloom;
	m_pixelConstantData.eyePos = curScene->m_pixelConstantData.eyePos;
	m_pixelConstantData.light = curScene->m_pixelConstantData.light;
	m_pixelConstantData.rim = curScene->m_pixelConstantData.rim;
	m_pixelConstantData.useAlbedo = curScene->m_pixelConstantData.useAlbedo;
	m_pixelConstantData.useNormal = curScene->m_pixelConstantData.useNormal;
	m_pixelConstantData.useAO = curScene->m_pixelConstantData.useAO;
	m_pixelConstantData.useMetallic = curScene->m_pixelConstantData.useMetallic;
	m_pixelConstantData.useRoughness = curScene->m_pixelConstantData.useRoughness;
	m_pixelConstantData.exposure = curScene->m_pixelConstantData.exposure;
	m_pixelConstantData.gamma = curScene->m_pixelConstantData.gamma;
	m_pixelConstantData.metallic = curScene->m_pixelConstantData.metallic;
}

void Mesh::Render(ID3D11DeviceContext* context)
{
	ReadyToRender(context);
	context->DrawIndexed(m_indexCount, 0, 0);
}

void Mesh::ReadyToRender(ID3D11DeviceContext* context)
{
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	context->OMSetRenderTargets(1, SceneMgr::GetInst().GetRenderTargetView().GetAddressOf(), SceneMgr::GetInst().GetDepthStencilView().Get());
	context->IASetPrimitiveTopology(m_topology);
	context->IASetVertexBuffers(0, 1, m_vertexBuffer.GetAddressOf(), &stride, &offset);
	context->IASetInputLayout(m_inputLayout.Get());
	context->IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

	context->VSSetShader(m_vertexShader.Get(), nullptr, 0);
	context->VSSetConstantBuffers(0, 1, m_vertexConstantBuffer.GetAddressOf());
	context->VSSetShaderResources(0, 1, m_arrSRV[(UINT)TEXTURE_TYPE::HEIGHT].GetAddressOf());
	context->VSSetSamplers(0, 1, m_samplerState.GetAddressOf());

	context->GSSetShader(m_geometryShader.Get(), nullptr, 0);
	context->GSSetConstantBuffers(0, 1, m_vertexConstantBuffer.GetAddressOf());

	context->PSSetShader(m_pixelShader.Get(), nullptr, 0);
	context->PSSetConstantBuffers(0, 1, m_pixelConstantBuffer.GetAddressOf());
	static ID3D11SamplerState* samplerStates[2] = { m_samplerState.Get(),m_clampSampler.Get() };
	context->PSSetSamplers(0, 2, samplerStates);
	context->PSSetShaderResources(0, (UINT)TEXTURE_TYPE::HEIGHT, m_arrSRV[(UINT)TEXTURE_TYPE::SPECULAR].GetAddressOf());
}



void Mesh::CreateVertexShaderAndInputLayout(const wstring& hlslPrefix, ComPtr<ID3D11VertexShader>& vertexShader)
{
	vector<D3D11_INPUT_ELEMENT_DESC> inputElements =
	{
		{"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 4 * 3, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 4 * 3 + 4 * 3, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 4 * 3 + 4 * 3 + 4 * 2, D3D11_INPUT_PER_VERTEX_DATA, 0}
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

void Mesh::ReadCubeImage(const string& fileName, TEXTURE_TYPE textureType)
{
	ComPtr<ID3D11Texture2D> texture;
	ComPtr<ID3D11ShaderResourceView> shaderResourceView;
	D3DUtils::GetInst().ReadCubeImage(fileName, texture, shaderResourceView);

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

