// pch.h: 미리 컴파일된 헤더 파일입니다.
// 아래 나열된 파일은 한 번만 컴파일되었으며, 향후 빌드에 대한 빌드 성능을 향상합니다.
// 코드 컴파일 및 여러 코드 검색 기능을 포함하여 IntelliSense 성능에도 영향을 미칩니다.
// 그러나 여기에 나열된 파일은 빌드 간 업데이트되는 경우 모두 다시 컴파일됩니다.
// 여기에 자주 업데이트할 파일을 추가하지 마세요. 그러면 성능이 저하됩니다.

#ifndef PCH_H
#define PCH_H

// 여기에 미리 컴파일하려는 헤더 추가
#include "framework.h"
// D3D11
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#include <d3d11.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <directxtk/SimpleMath.h>
#include <DirectXTex.h>
#include <wrl.h>
#include <directxtk/DDSTextureLoader.h>
#include <DirectXTexEXR.h>
#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>

// Default
#include "Define.h"
#include <iostream>
#include <Windows.h>
#include <vector>
#include <string>
#include <memory>
#include <map>
#include <algorithm>

// using
using std::cout;
using std::endl;
using std::vector;
using std::wstring;
using std::string;
using std::vector;
using std::map;
using std::shared_ptr;
using std::make_shared;
using std::reverse;
using namespace DirectX;
using namespace DirectX::SimpleMath;
using Microsoft::WRL::ComPtr;

#include "Enum.h"
#include "Struct.h"

#endif //PCH_H

/*
태 	  수 	    금 	   지 	    화 	  목	 토  	  천 	 해	   달
자전속도
0.1	0.00015	0.00001	 0.023	0.012	0.63	0.5	0.129	0.134	0.00001

공전속도
0	1	0.73	0.62	0.5	0.27	0.2	0.14	0.11	  1.023

부피
109	0.38	 0.94	    1	  0.53	10.97	 9.14	3.98	 3.86	    ?

자전 기울기
7.25	0.01	2.64	23.44	25.19	3.12	26.73	82.23	28.33	  6.69

거리
0	2	3.6	5.16	7.84	26.86	49.4	99.1	155.68
*/