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
	// 여기서도 y축으로 시점이 가는순간 짐벌락 발생
	Matrix result = Matrix::CreateTranslation(-m_pos) *
		Matrix::CreateRotationY(-m_fYaw) *
		Matrix::CreateRotationX(m_fPitch);
	return result;
}

Matrix Camera::GetArrowViewRow()
{
	Matrix result =	Matrix::CreateRotationY(m_fYaw) *
		Matrix::CreateRotationX(-m_fPitch);
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
	if (KEYCHECK(E, HOLD))
		MoveUpDir(dt);
	if (KEYCHECK(Q, HOLD))
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
	Vector2 cursorPos = KeyMgr::GetInst().GetMousePos();
	m_fYaw = cursorPos.x * XM_PI;
	m_fPitch = cursorPos.y * XM_PIDIV2;

	m_viewDir = Vector3::Transform(Vector3{ 0.0f,0.0f,1.0f },
		Matrix::CreateRotationX(-m_fPitch) *
		Matrix::CreateRotationY(m_fYaw));

	// 짐벌락 현상때문에 이게 맞아
	// 아.. 드디어 이해했습니다 짐벌락
	m_upDir = Vector3::Transform(Vector3{ 0.0f, 1.0f, 0.0f },
		Matrix::CreateRotationX(-m_fPitch) *
		Matrix::CreateRotationY(m_fYaw));

	m_rightDir = m_upDir.Cross(m_viewDir);
}
