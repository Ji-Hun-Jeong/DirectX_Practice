#pragma once
#include "Mesh.h"
class DirArrow :
    public Mesh
{
public:
    explicit DirArrow(const Vector3& translation, const Vector3& rotation1, const Vector3& rotation2, const Vector3& scale);
public:
    virtual void Init(const MeshData& meshData) override;
    virtual void UpdateMeshConstantData(float dt) override;
    void ReadyToRender(ComPtr<ID3D11DeviceContext>& context);
private:
    DirArrowConstantData m_dirArrowConstantData;
};

