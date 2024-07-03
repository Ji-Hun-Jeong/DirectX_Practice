#pragma once
#include "Scene.h"
class TestScene :
    public Scene
{
public:
	TestScene();
public:
	void Init() override;
	void Enter() override;
	void Exit() override;
	void InitMesh() override;
	void InitCubeMap() override;
	void UpdateGUI() override;
private:

};

