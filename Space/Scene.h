#pragma once
class Object;
class NonObject;
class Camera;
class ImageFilter;
class CubeMap;
class Scene
{
public:
	Scene();
public:
	virtual void Init();
	virtual void Enter() = 0;
	virtual void Exit() = 0;
	virtual void Update(float dt);
	virtual void UpdateGUI();
	virtual void Render();
	shared_ptr<Camera>& GetCamera() { return m_camera; }

protected:
	virtual void InitMesh() = 0;
	virtual void InitCubeMap();

	void CalcPickingObject();

public:
	PixelConstantData m_pixelConstantData;

	bool m_drawNormal = false;
	float m_normalSize = 1.0f;
		
protected:
	shared_ptr<Camera> m_camera;
	shared_ptr<CubeMap> m_cubeMap;
	shared_ptr<Object> m_focusObj;

	vector<shared_ptr<Object>> m_vecObj;
	vector<shared_ptr<NonObject>> m_vecNonObj;
	vector<shared_ptr<ImageFilter>> m_filters;

};

