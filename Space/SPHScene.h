#pragma once
#include "AnimateScene.h"
class SPHScene :
    public AnimateScene
{
public:
    SPHScene(SceneMgr* pOwner);
public:
    virtual void Update(float dt) override;
    virtual void Render(ComPtr<ID3D11DeviceContext>& context, bool drawWireFrame) override;
    virtual void Enter() override;

private:
    float m_radius;
    float m_mass;
    float m_pressureCoeff;
    float m_density0;
    float m_viscosity;

    struct OrderConst
    {
        int i;
        Vector3 dummy;
    }m_orderConst;

    Texture2D m_density;
    ComPtr<ID3D11Buffer> m_orderBuffer;
};

