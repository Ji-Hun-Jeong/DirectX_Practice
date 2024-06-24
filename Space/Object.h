#pragma once
#include "Mesh.h"
class Object :
    public Mesh
{
public:
    Object();
    explicit Object(const string& strName, const Vector3& translation, const Vector3& rotation1, const Vector3& rotation2, const Vector3& scale, D3D11_PRIMITIVE_TOPOLOGY topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    virtual bool IsCollision(const MyRay& ray);
    virtual void Init(const MeshData& meshData, const wstring& vertexShaderPrefix, const wstring& pixelShaderPrefix) override;
    virtual void Update(float dt) override;
    virtual void Render(ID3D11DeviceContext* context) override;
    virtual void DrawNormal(ID3D11DeviceContext* context);
    virtual void UpdateVertexConstantData(float dt) override;
    virtual void UpdatePixelConstantData() override;
    virtual void UpdateNormalConstantData();
    bool AttachObject(const string& meshName, shared_ptr<Object> childObj);

    const string& GetName() { return m_strName; }
protected:
    string				 m_strName;
    vector<shared_ptr<Object>> m_vecObj;
    Object* m_ownerObj;

    ComPtr<ID3D11VertexShader> m_normalVertexShader;
    ComPtr<ID3D11PixelShader> m_normalPixelShader;
    ComPtr<ID3D11GeometryShader> m_normalGeometryShader;
    D3D11_PRIMITIVE_TOPOLOGY m_normalTopology;
    ComPtr<ID3D11Buffer> m_normalConstantBuffer;
    NormalConstantData   m_normalConstantData;

    Matrix m_prevTransformModel;
private:

};

