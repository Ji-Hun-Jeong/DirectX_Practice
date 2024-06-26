#pragma once
using DirectX::SimpleMath::Matrix;
class Camera
{
public:
	Camera();
public:
	void Update(float dt);
	const Vector3& GetPos() const { return m_pos; }
	const Vector3& GetViewDir() const { return m_viewDir; }
	const Vector3& GetRightDir() const { return m_rightDir; }
	const Vector3& GetUpDir() const { return m_upDir; }
	void CalcViewRow();
	void CalcProjectionRow();
	Matrix GetArrowViewRow();

	Matrix m_view;
	Matrix m_projection;
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

