#include "pch.h"
#include "AnimateScene.h"
#include "SceneMgr.h"

AnimateScene::AnimateScene(SceneMgr* owner)
	: Scene(owner)
{
}

void AnimateScene::Enter()
{
}

void AnimateScene::Exit()
{
}

void AnimateScene::Update(float dt)
{
}

void AnimateScene::Render(ComPtr<ID3D11DeviceContext>& context, bool drawWireFrame)
{
}

void AnimateScene::UpdateGUI()
{
}

void AnimateScene::InitMesh()
{
}

void AnimateScene::InitIBL()
{
}

void AnimateScene::InitSkyBox()
{
}
