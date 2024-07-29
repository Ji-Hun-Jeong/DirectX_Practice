#pragma once
#include "Scene.h"
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

};

