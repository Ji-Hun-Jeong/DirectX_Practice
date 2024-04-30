#include "pch.h"
#include "Camera.h"
#include "KeyMgr.h"

Camera::Camera()
	: m_fSpeed(100.0f)
	, m_fPitch(0.0f)
	, m_fYaw(0.0f)
	, m_fRoll(0.0f)
{
}

void Camera::Update(float dt)
{
	if (KEYCHECK(F, TAP))
		m_bMoveDir = !m_bMoveDir;
	if (m_bMoveDir)
		CalDirection();
	UpdatePos(dt);
}

Matrix Camera::GetViewRow()
{
	Matrix result = Matrix::CreateTranslation(-m_pos) *
		Matrix::CreateRotationX(m_fPitch) *
		Matrix::CreateRotationY(-m_fYaw);
	return result;
}

void Camera::UpdatePos(float dt)
{
	if (KEYCHECK(W, HOLD))
		MoveViewDir(dt);
	if (KEYCHECK(S, HOLD))
		MoveViewDir(-dt);
	if (KEYCHECK(A, HOLD))
		MoveRightDir(-dt);
	if (KEYCHECK(D, HOLD))
		MoveRightDir(dt);
	if (KEYCHECK(Q, HOLD))
		MoveUpDir(dt);
	if (KEYCHECK(E, HOLD))
		MoveUpDir(-dt);
}

void Camera::MoveViewDir(float dt)
{
	m_pos += m_viewDir * m_fSpeed * dt;
}

void Camera::MoveRightDir(float dt)
{
	m_pos += m_rightDir * m_fSpeed * dt;
}

void Camera::MoveUpDir(float dt)
{
	m_pos += m_upDir * m_fSpeed * dt;
}

void Camera::CalDirection()
{
	Vector2 cursorDiff = KeyMgr::GetInst().GetMousePos();
	m_fYaw = cursorDiff.x * XM_PI;
	m_fPitch = cursorDiff.y * XM_PIDIV2;

	m_viewDir = Vector3::Transform(Vector3{ 0.0f,0.0f,1.0f },
		Matrix::CreateRotationY(m_fYaw)
		* Matrix::CreateRotationX(-m_fPitch));

	m_upDir = Vector3::Transform(Vector3{ 0.0f, 1.0f, 0.0f },
		Matrix::CreateRotationY(m_fYaw)
		* Matrix::CreateRotationX(-m_fPitch));

	m_rightDir = m_upDir.Cross(m_viewDir);
}
