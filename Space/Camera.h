#pragma once
using DirectX::SimpleMath::Matrix;
class Camera
{
public:
	Camera();
public:
	void Update(float dt);
	const Vector3& GetPos() { return m_pos; }
	Matrix GetViewRow();
private:
	void UpdatePos(float dt);
	void MoveViewDir(float dt);
	void MoveRightDir(float dt);
	void MoveUpDir(float dt);
	void CalDirection();
private:
	Vector3 m_pos = Vector3{ 0.0f,0.0f,-300.0f };
	Vector3 m_viewDir;
	Vector3 m_rightDir;
	Vector3 m_upDir = Vector3{ 0.0f,1.0f,0.0f };
	
	float m_fSpeed;
	float m_fPitch;
	float m_fYaw;
	float m_fRoll;

	bool m_bMoveDir = false;
};

