#include "pch.h"
#include "KeyMgr.h"
#include "Core.h"
KeyMgr KeyMgr::m_inst;

UINT g_keyValue[(UINT)KEY_TYPE::END] =
{
	0x51, 0x57, 0x45, 0x41, 0x53, 0x44, 0x46,
	MK_LBUTTON, MK_RBUTTON, VK_ESCAPE,
	0x31, 0x32, 0x33, 0x34, 0x35, 0x36, VK_SPACE
};
KeyMgr::KeyMgr()
	: m_arrKey{}
	, m_cursorPos{}
{
}
void KeyMgr::Update()
{
	KeyUpdate();
	MouseUpdate();
	if (KEYCHECK(ESC, TAP))
		exit(0);
}

Vector2 KeyMgr::GetMouseNDCPos()
{
	const float aspect = Core::GetInst().GetAspect();
	const float screenWidth = Core::GetInst().m_fWidth;
	const float screenHeight = Core::GetInst().m_fHeight;
	float x = (float)m_cursorPos.x;
	float y = (float)m_cursorPos.y;
	x = (x * 2 * aspect) / screenWidth - aspect;
	y = -(y * 2.0f) / screenHeight + 1.0f;
	x /= aspect;
	return Vector2(x, y);
}

Vector2 KeyMgr::GetMouseScreenPos()
{
	return Vector2(float(m_cursorPos.x), float(m_cursorPos.y));
}

void KeyMgr::KeyUpdate()
{
	for (UINT i = 0; i < (UINT)KEY_TYPE::END; ++i)
	{
		if (GetAsyncKeyState(g_keyValue[i]) & 0x8000)	// ����
		{
			if (m_arrKey[i].isPrev)	// �������� ����
				m_arrKey[i].keyState = KEY_STATE::HOLD;
			else
				m_arrKey[i].keyState = KEY_STATE::TAP;
			m_arrKey[i].isPrev = true;
		}
		else
		{
			if (m_arrKey[i].isPrev)	// ������ ����
				m_arrKey[i].keyState = KEY_STATE::AWAY;
			else
				m_arrKey[i].keyState = KEY_STATE::NONE;
			m_arrKey[i].isPrev = false;
		}
	}
}

void KeyMgr::MouseUpdate()
{
	GetCursorPos(&m_cursorPos);
	ScreenToClient(Core::GetInst().GetMainWindow(), &m_cursorPos);
	if (KEYCHECK(LBUTTON, NONE))
		return;
}
