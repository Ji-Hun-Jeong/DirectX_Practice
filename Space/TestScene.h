#pragma once
#include "Scene.h"
class TestScene :
    public Scene
{
public:
	TestScene();
public:
	void Enter() override;
	void Exit() override;
	void UpdateGUI() override;
protected:
	void InitMesh() override;
	void InitSkyBox() override;
	void InitIBL() override;

private:

};

