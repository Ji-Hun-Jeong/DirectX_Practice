#pragma once
#include "Scene.h"
template <typename Particle> class StructuredBuffer;
class Texture2D;
class AnimateScene :
    public Scene
{
public:
    AnimateScene(SceneMgr* owner);
public:
    virtual void Enter();
    virtual void Exit();
    virtual void Update(float dt);
    virtual void Render(ComPtr<ID3D11DeviceContext>& context, bool drawWireFrame);
    virtual void UpdateGUI();

protected:
    virtual void InitMesh();
    virtual void InitIBL();
    virtual void InitSkyBox();

private:
    void ComputeShaderBarrier(ComPtr<ID3D11DeviceContext>& context);
    void DissipateDensity(ComPtr<ID3D11DeviceContext>& context);
    void AdvectParticles(ComPtr<ID3D11DeviceContext>& context);
    void DrawSprites(ComPtr<ID3D11DeviceContext>& context);

private:
    shared_ptr<StructuredBuffer<Particle>> m_particle;
    shared_ptr<Texture2D> m_stagingBuffer;
};

