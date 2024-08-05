#pragma once
#include "AnimateScene.h"
class SPHScene :
    public AnimateScene
{
public:
    SPHScene(SceneMgr* pOwner);
public:
    virtual void Update(float dt);
    virtual void Render(ComPtr<ID3D11DeviceContext>& context, bool drawWireFrame);

private:
    float m_radius;
    float m_mass;
    float m_pressureCoeff;
    float m_density0;
    float m_viscosity;
};

