#pragma once
#include "AnimateScene.h"
class SpriteScene :
    public AnimateScene
{
public:
    SpriteScene(SceneMgr* pOwner);
public:
    virtual void Update(float dt);
    virtual void Render(ComPtr<ID3D11DeviceContext>& context, bool drawWireFrame);
private:
    void DissipateDensity(ComPtr<ID3D11DeviceContext>& context);
    void AdvectParticles(ComPtr<ID3D11DeviceContext>& context);
    void DrawSprites(ComPtr<ID3D11DeviceContext>& context) override;
private:

};

