#pragma once
#include "Scene.h"
class SpaceScene :
    public Scene
{
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

