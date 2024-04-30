#include "pch.h"
#include "KeyMgr.h"
#include "Core.h"
KeyMgr KeyMgr::m_inst;

UINT g_keyValue[(UINT)KEY_TYPE::END] =
{
	0x51, 0x57, 0x45, 0x41, 0x53, 0x44,0x46, MK_LBUTTON, MK_RBUTTON, VK_ESCAPE
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
}

Vector2 KeyMgr::GetMousePos()
{
	const float aspect = Core::GetInst().GetAspect();
	const float screenWidth = Core::GetInst().GetScreenWidth();
	const float screenHeight = Core::GetInst().GetScreenHeight();
	float x = (float)m_cursorPos.x;
	float y = (float)m_cursorPos.y;
	x = (x * 2 * aspect) / screenWidth - aspect;
	y = -(y * 2.0f) / screenHeight + 1.0f;
	return Vector2(x, y);
}

void KeyMgr::KeyUpdate()
{
	for (UINT i = 0; i < (UINT)KEY_TYPE::END; ++i)
	{
		if (GetAsyncKeyState(g_keyValue[i]) & 0x8000)	// 눌림
		{
			if (m_arrKey[i].isPrev)	// 이전에도 눌림
				m_arrKey[i].keyState = KEY_STATE::HOLD;
			else
				m_arrKey[i].keyState = KEY_STATE::TAP;
			m_arrKey[i].isPrev = true;
		}
		else
		{
			if (m_arrKey[i].isPrev)	// 이전에 눌림
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
	if (KEYCHECK(LBUTTON, TAP))
	{
		m_prevCursorPos = GetMousePos();
		m_curCursorPos = m_prevCursorPos;
	}
	else if (KEYCHECK(LBUTTON, HOLD))
	{
		m_curCursorPos = GetMousePos();
	}
	else if (KEYCHECK(LBUTTON, AWAY))
	{
		m_curCursorPos = GetMousePos();
		m_prevCursorPos = m_curCursorPos;
	}
	m_curDiffCursorPos = m_curCursorPos - m_prevCursorPos;
}
