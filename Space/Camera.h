#pragma once
using DirectX::SimpleMath::Matrix;
class Camera
{
public:
	Camera();
public:
	void Update(float dt);
	void CalcViewRow();
	void CalcProjectionRow();
	Matrix GetArrowViewRow();

	Matrix m_view;
	Matrix m_projection;

public:
	const Vector3& GetPos() const { return m_pos; }
	const Vector3& GetViewDir() const { return m_viewDir; }
	const Vector3& GetRightDir() const { return m_rightDir; }
	const Vector3& GetUpDir() const { return m_upDir; }
	float GetAngleY() const { return XMConvertToRadians(m_angleY); }
	float GetNearZ() const { return m_nearZ; }
	float GetFarZ() const { return m_farZ; }

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

	float m_angleY = 70.0f;
	float m_nearZ = 0.01f;
	float m_farZ = 10000.0f;

	bool m_bMoveDir = false;
};

