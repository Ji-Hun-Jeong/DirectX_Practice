#include "pch.h"
#include "Camera.h"
#include "KeyMgr.h"
#include "Core.h"

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
	CalcViewRow();
	CalcProjectionRow();
}

void Camera::CalcViewRow()
{
	// 여기서도 y축으로 시점이 가는순간 짐벌락 발생
	m_view = Matrix::CreateTranslation(-m_pos) *
		Matrix::CreateRotationY(-m_fYaw) *
		Matrix::CreateRotationX(m_fPitch);
}

void Camera::CalcProjectionRow()
{
	Core& core = Core::GetInst();
	float angleY = core.GetAngleY();
	float aspect = core.GetAspect();
	float nearZ = core.GetNearZ();
	float farZ = core.GetFarZ();
	m_projection = XMMatrixPerspectiveFovLH(angleY, aspect, nearZ, farZ);
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
	Vector2 cursorPos = KeyMgr::GetInst().GetMouseNDCPos();
	m_fYaw = cursorPos.x * XM_PI;
	m_fPitch = cursorPos.y * XM_PIDIV2;

	m_viewDir = Vector3::Transform(Vector3{ 0.0f,0.0f,1.0f },
		Matrix::CreateRotationX(-m_fPitch) *
		Matrix::CreateRotationY(m_fYaw));

	// Gimbal Lock
	Quaternion rotateX = Quaternion::CreateFromAxisAngle(Vector3{ 1.0f,0.0f,0.0f }, -m_fPitch);
	Quaternion rotateY = Quaternion::CreateFromAxisAngle(Vector3{ 0.0f,1.0f,0.0f }, m_fYaw);
	m_upDir = Vector3::Transform(Vector3{ 0.0f, 1.0f, 0.0f },
		Matrix::CreateFromQuaternion(rotateX) *
		Matrix::CreateFromQuaternion(rotateY));
	
	m_rightDir = m_upDir.Cross(m_viewDir);
}
