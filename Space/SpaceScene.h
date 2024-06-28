#pragma once
#include "Scene.h"
class SpaceScene :
    public Scene
{
public:
	void Init() override;
	void InitMesh() override;
	void Enter() override;
	void Exit() override;
	void Update(float dt) override;
private:

};

