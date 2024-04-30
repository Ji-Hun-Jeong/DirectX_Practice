#pragma once
#define SINGLE(type) private:\
						static type m_inst;\
						type();\
					 public:\
						static type& GetInst() { return m_inst; }

#define KEYCHECK(type,state) KeyMgr::GetInst().KeyCheck(KEY_TYPE::type, KEY_STATE::state) 
#define MOUSEPOS() KeyMgr::GetInst().GetMousePos();