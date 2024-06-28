#pragma once
#include "Scene.h"
class TestScene :
    public Scene
{
public:
	void Init() override;
	void Enter() override;
	void Exit() override;
	void InitMesh() override;
private:

};

