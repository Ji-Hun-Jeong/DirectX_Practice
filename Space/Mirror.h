#pragma once
#include "Object.h"
class Mirror :
    public Object
{
public:
    Mirror();
    explicit Mirror(const string& strName, const Vector3& translation, const Vector3& rotation1, const Vector3& rotation2, const Vector3& scale, D3D11_PRIMITIVE_TOPOLOGY topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
public:
    virtual void Init(const MeshData& meshData, const wstring& vertexShaderPrefix, const wstring& pixelShaderPrefix) override;
    virtual void Update(float dt) override;

    ComPtr<ID3D11Buffer>& GetViewProjBuffer() { return m_viewProjBuffer; }
private:
    DirectX::SimpleMath::Plane m_mirrorPlane;
    Matrix m_mirrorViewProj;
    ComPtr<ID3D11Buffer> m_viewProjBuffer;
    Vector3 m_normalDir;
};

