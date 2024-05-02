#pragma once
class Mesh
{
public:
	Mesh();
	Mesh(const Vector3& translation, const Vector3& rotation1,const Vector3& rotation2,const Vector3& scale,D3D11_PRIMITIVE_TOPOLOGY topology= D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	void Init(const string& name, const MeshData& meshData, const wstring& vertexShaderPrefix, const wstring& pixelShaderPrefix);
	virtual void Update(float dt);
	virtual void Render(ID3D11DeviceContext* context, bool drawNormal);
protected:
	virtual void UpdateVertexConstantData(float dt);
	virtual void UpdateNormalConstantData();
	virtual void UpdatePixelConstantData();
	virtual void ReadyToRender(ID3D11DeviceContext* context);
	virtual void DrawNormal(ID3D11DeviceContext* context);
	void CreateVertexShaderAndInputLayout(const wstring& hlslPrefix, ComPtr<ID3D11VertexShader>& vertexShader);
	void CreatePixelShader(const wstring& hlslPrefix, ComPtr<ID3D11PixelShader>& pixelShader);
	void CreateGeometryShader(const wstring& hlslPrefix, ComPtr<ID3D11GeometryShader>& geometryShader);
	virtual void ReadImage(const string& textureName);

// D3D11 Member
protected:
	string				 m_strName;
	ComPtr<ID3D11Buffer> m_vertexBuffer;
	ComPtr<ID3D11Buffer> m_indexBuffer;

	ComPtr<ID3D11InputLayout> m_inputLayout;
	UINT				 m_indexCount;

	ComPtr<ID3D11VertexShader> m_vertexShader;
	ComPtr<ID3D11PixelShader> m_pixelShader;
	ComPtr<ID3D11GeometryShader> m_geometryShader;

	ComPtr<ID3D11VertexShader> m_normalVertexShader;
	ComPtr<ID3D11PixelShader> m_normalPixelShader;
	ComPtr<ID3D11GeometryShader> m_normalGeometryShader;

	vector<ComPtr<ID3D11ShaderResourceView>> m_vecShaderResourceViews;
	ComPtr<ID3D11SamplerState> m_samplerState;

	D3D11_PRIMITIVE_TOPOLOGY m_topology;
	D3D11_PRIMITIVE_TOPOLOGY m_normalTopology;

// Constant Member
protected:
	ComPtr<ID3D11Buffer> m_vertexConstantBuffer;
	ComPtr<ID3D11Buffer> m_pixelConstantBuffer;
	ComPtr<ID3D11Buffer> m_normalConstantBuffer;

	VertexConstantData	 m_vertexConstantData;
	PixelConstantData	 m_pixelConstantData;
	NormalConstantData   m_normalConstantData;
public:
	Vector3 m_translation;
	Vector3 m_rotation1;
	Vector3 m_rotation2;
	Vector3 m_scale;

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

	const string& GetName() { return m_strName; }
	const VertexConstantData& GetVertexConstantData() { return m_vertexConstantData; }
	PixelConstantData& GetPixelConstantData() { return m_pixelConstantData; }
};

