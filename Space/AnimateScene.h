#pragma once
#include "Scene.h"
#include "IndirectArgsBuffer.h"
#include "Texture2D.h"
#include "StructuredBuffer.h"
class AnimateScene :
    public Scene
{
public:
    AnimateScene(SceneMgr* owner);
public:
    virtual void Enter();
    virtual void Exit() {}
    virtual void Update(float dt) = 0;
    virtual void Render(ComPtr<ID3D11DeviceContext>& context, bool drawWireFrame) = 0;
    virtual void UpdateGUI() {}

protected:
    virtual void InitMesh() {}
    virtual void InitIBL() {}
    virtual void InitSkyBox() {}
    void CreateIndirectArgsBuffer(UINT count);
    void ComputeShaderBarrier(ComPtr<ID3D11DeviceContext>& context);
    virtual void DrawSprites(ComPtr<ID3D11DeviceContext>& context);

protected:
    shared_ptr<IndirectArgsBuffer<IndirectArgs>> m_indirectArgsBuffer;
    StructuredBuffer<Particle> m_particle;
    Texture2D m_stagingBuffer;

    const Vector3 m_gravity = Vector3(0.0f, -9.8f, 0.0f);
};

