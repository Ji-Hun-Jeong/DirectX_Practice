#pragma once
#include "Mesh.h"
class Mirror :
    public Mesh
{
public:
    Mirror();
    explicit Mirror(const string& strName, const Vector3& translation, const Vector3& rotation1, const Vector3& rotation2, const Vector3& scale);
public:
    virtual void Init(const MeshData& meshData) override;
    virtual void UpdateMeshConstantData(float dt) override;
    virtual void UpdateMaterialConstantData() override;
    const Matrix& GetMirrorViewProj() { return m_mirrorViewProj; }

private:
    DirectX::SimpleMath::Plane m_mirrorPlane;
    Matrix m_mirrorViewProj;
    
    Vector3 m_normalDir;
};

