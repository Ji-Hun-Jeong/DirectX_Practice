#pragma once
struct KeyInfo
{
	bool isPrev;
	KEY_STATE keyState;
};
class KeyMgr
{
// Default
public:
	void Update();
	
private:
	void KeyUpdate();
	void MouseUpdate();

// Key
private:
	KeyInfo m_arrKey[(UINT)KEY_TYPE::END];
public:
	bool KeyCheck(KEY_TYPE keyType, KEY_STATE keyState) { return m_arrKey[(UINT)keyType].keyState == keyState; }

// CursorPos
private:
	POINT m_cursorPos;
	Vector2 m_prevCursorPos;
	Vector2 m_curCursorPos;
public:
	Vector2 GetMouseNDCPos();
	Vector2 GetMouseScreenPos();

SINGLE(KeyMgr)
};

