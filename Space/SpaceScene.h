#pragma once
#include "RenderScene.h"
class SpaceScene :
    public RenderScene
{
public:
	SpaceScene(SceneMgr* owner);
public:
	void InitMesh() override;
	void InitIBL() override;
	void InitSkyBox() override;
	void Enter() override;
	void Exit() override;
	void Update(float dt) override;
	void UpdateGUI() override;
private:

};

