#pragma once
#include "Scene.h"
#include "IndirectArgsBuffer.h"
#include "Texture2D.h"
#include "StructuredBuffer.h"
class Texture2D;
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
    void DrawSprites(ComPtr<ID3D11DeviceContext>& context);

protected:
    shared_ptr<IndirectArgsBuffer<IndirectArgs>> m_indirectArgsBuffer;
    shared_ptr<StructuredBuffer<Particle>> m_particle;
    shared_ptr<Texture2D> m_stagingBuffer;

};

