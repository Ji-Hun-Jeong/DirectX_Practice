#pragma once
#include "Mesh.h"
class Light
    : public Mesh
{
public:
    Light();
    explicit Light(const string& strName, const Vector3& pos, const Vector3& viewPos, float radius);
public:
    void Init(const MeshData& meshData) override;
    void UpdateMeshConstantData(float dt) override;
    void UpdateMaterialConstantData() override;

    const Vector3& GetPos() { return m_translation; }
    const Vector3& GetLightDir() { return m_viewDir; }
    const Matrix& GetView() { return m_view; }
    const Matrix& GetProj() { return m_proj; }
    const Matrix& GetViewProj() { return m_viewProj; }

    ComPtr<ID3D11DepthStencilView>& GetLightViewDSV() { return m_DSV; }
    ComPtr<ID3D11ShaderResourceView>& GetLightViewSRV() { return m_SRV; }

    float m_fSpotFactor = 1.0f;
    float m_fRadius;
private:
    ComPtr<ID3D11Texture2D> m_depthBuffer;
    ComPtr<ID3D11DepthStencilView> m_DSV;
    ComPtr<ID3D11ShaderResourceView> m_SRV;

private:
    Vector3 m_viewPos = Vector3(0.0f);
    Vector3 m_viewDir;

    Matrix m_view;
    Matrix m_proj;
    Matrix m_viewProj;

    float m_angleY = 120.0f;
    float m_nearZ = 0.01f;
    float m_farZ = 100.0f;
};