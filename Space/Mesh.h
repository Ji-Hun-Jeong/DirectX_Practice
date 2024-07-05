#pragma once
class Mesh
{
public:
	Mesh();
	explicit Mesh(const Vector3& translation, const Vector3& rotation1,const Vector3& rotation2,const Vector3& scale, D3D11_PRIMITIVE_TOPOLOGY topology= D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
public:
	virtual void Init(const MeshData& meshData, const wstring& vertexShaderPrefix, const wstring& pixelShaderPrefix);
	virtual void Update(float dt);
	virtual void Render(ID3D11DeviceContext* context);
	virtual void ReadImage(const string& textureName, TEXTURE_TYPE textureType, bool useSRGB = false);
	void ReadCubeImage(const string& fileName, TEXTURE_TYPE textureType);
protected:
	virtual void UpdateVertexConstantData(float dt);
	virtual void UpdatePixelConstantData();
	virtual void ReadyToRender(ID3D11DeviceContext* context);

	void CreateVertexShaderAndInputLayout(const wstring& hlslPrefix, ComPtr<ID3D11VertexShader>& vertexShader);
	void CreatePixelShader(const wstring& hlslPrefix, ComPtr<ID3D11PixelShader>& pixelShader);
	void CreateGeometryShader(const wstring& hlslPrefix, ComPtr<ID3D11GeometryShader>& geometryShader);
	float GetTic(float dt);

public:
	const D3D11_PRIMITIVE_TOPOLOGY& GetPrimitiveTopology() { return m_topology; }
	const ComPtr<ID3D11Buffer>& GetVertexBuffer() { return m_vertexBuffer; }
	const ComPtr<ID3D11Buffer>& GetIndexBuffer() { return m_indexBuffer; }
	const ComPtr<ID3D11InputLayout>& GetInputLayout() { return m_inputLayout; }

	const ComPtr<ID3D11VertexShader>& GetVertexShader() { return m_vertexShader; }
	const UINT& GetIndexCount() { return m_indexCount; }

	const ComPtr<ID3D11PixelShader>& GetPixelShader() { return m_pixelShader; }
	const ComPtr<ID3D11Buffer>& GetPixelConstantBuffer() { return m_pixelConstantBuffer; }
	const ComPtr<ID3D11SamplerState>& GetSamplerState() { return m_samplerState; }

	const VertexConstantData& GetVertexConstantData() { return m_vertexConstantData; }
	PixelConstantData& GetPixelConstantData() { return m_pixelConstantData; }

// D3D11 Member
protected:
	ComPtr<ID3D11Buffer> m_vertexBuffer;
	ComPtr<ID3D11Buffer> m_indexBuffer;

	ComPtr<ID3D11InputLayout> m_inputLayout;
	UINT				 m_indexCount;

	ComPtr<ID3D11VertexShader> m_vertexShader;
	ComPtr<ID3D11PixelShader> m_pixelShader;
	ComPtr<ID3D11GeometryShader> m_geometryShader;
	ComPtr<ID3D11HullShader> m_hullShader;
	ComPtr<ID3D11DomainShader> m_domainShader;

	ComPtr<ID3D11Texture2D> m_arrTexture[(UINT)TEXTURE_TYPE::END];
	ComPtr<ID3D11ShaderResourceView> m_arrSRV[(UINT)TEXTURE_TYPE::END];
	ComPtr<ID3D11SamplerState> m_samplerState;
	ComPtr<ID3D11SamplerState> m_clampSampler;

	D3D11_PRIMITIVE_TOPOLOGY m_topology;

// Constant Member
protected:
	ComPtr<ID3D11Buffer> m_vertexConstantBuffer;
	ComPtr<ID3D11Buffer> m_pixelConstantBuffer;

	VertexConstantData	 m_vertexConstantData;
	PixelConstantData	 m_pixelConstantData;

public:
	Vector3 m_translation;
	Vector3 m_rotation1;
	Vector3 m_rotation2;
	Vector3 m_scale;
};

