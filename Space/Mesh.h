#pragma once
class Mesh
{
public:
	Mesh();
	explicit Mesh(const string& strName, const Vector3& translation, const Vector3& rotation1,const Vector3& rotation2,const Vector3& scale);
public:
	virtual void Init(const MeshData& meshData);
	virtual void Update(float dt) final;
	virtual void Render(ComPtr<ID3D11DeviceContext>& context);
	virtual void DrawNormal(ComPtr<ID3D11DeviceContext>& context);
	virtual void ReadImage(const string& textureName, TEXTURE_TYPE textureType, bool useSRGB = false);
	bool IsCollision(MyRay ray);

protected:
	virtual void UpdateMeshConstantData(float dt);
	virtual void UpdateMaterialConstantData();
	virtual void UpdateCommonConstantData();

	float GetTic(float dt);

public:
	const ComPtr<ID3D11Buffer>& GetVertexBuffer() { return m_vertexBuffer; }
	const ComPtr<ID3D11Buffer>& GetIndexBuffer() { return m_indexBuffer; }

	const UINT& GetIndexCount() { return m_indexCount; }

	MeshConstData& GetMeshConstData() { return m_meshConstData; }
	MaterialConstData& GetMaterialConstData() { return m_materialConstData; }
	CommonConstData& GetCommonConstData() { return m_commonConstData; }
	ComPtr<ID3D11ShaderResourceView>& GetSRV(TEXTURE_TYPE textureType) { return m_arrSRV[(UINT)textureType]; }

	bool AttachMesh(const string& meshName, shared_ptr<Mesh> childMesh);

// D3D11 Member
protected:
	ComPtr<ID3D11Buffer> m_vertexBuffer;
	ComPtr<ID3D11Buffer> m_indexBuffer;

	UINT				 m_indexCount;

	ComPtr<ID3D11Texture2D> m_arrTexture[(UINT)TEXTURE_TYPE::END];
	ComPtr<ID3D11ShaderResourceView> m_arrSRV[(UINT)TEXTURE_TYPE::END];
// Constant Member
protected:
	ComPtr<ID3D11Buffer> m_meshConstBuffer;
	ComPtr<ID3D11Buffer> m_materialConstBuffer;
	ComPtr<ID3D11Buffer> m_commonConstBuffer;

	MeshConstData		m_meshConstData;
	MaterialConstData	 m_materialConstData;
	CommonConstData		m_commonConstData;
	
public:
	string  m_strName;

	vector<shared_ptr<Mesh>> m_vecObj;
	Mesh* m_ownerObj;

	Matrix m_prevTransformModel;

	Vector3 m_translation;
	Vector3 m_rotation1;
	Vector3 m_rotation2;
	Vector3 m_scale;

	bool m_bIsLight = false;
};

