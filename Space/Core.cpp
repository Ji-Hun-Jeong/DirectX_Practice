#include "pch.h"
#include "Core.h"
#include "D3DUtils.h"
#include "KeyMgr.h"
#include "SceneMgr.h"
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd,
	UINT msg,
	WPARAM wParam,
	LPARAM lParam);
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	return Core::GetInst().MsgProc(hWnd, message, wParam, lParam);
}
Core Core::m_inst;
bool Core::Init()
{
	if (!InitMainWindow())
		return false;
	SceneMgr::GetInst().Init(m_fWidth, m_fHeight);
	return true;
}

void Core::Update(float dt)
{
	KeyMgr::GetInst().Update();
	SceneMgr::GetInst().Update(dt);
}

void Core::Render()
{
	SceneMgr::GetInst().Render();
}

int Core::Progress()
{
	MSG msg = { 0 };
	while (WM_QUIT != msg.message)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			ImGui_ImplDX11_NewFrame(); // GUI ������ ����
			ImGui_ImplWin32_NewFrame();

			ImGui::NewFrame(); // � �͵��� ������ ���� ��� ����
			ImGui::Begin("Scene Control");

			// ImGui�� �������ִ� Framerate ���
			ImGui::Text("Average %.3f ms/frame (%.1f FPS)",
				1000.0f / ImGui::GetIO().Framerate,
				ImGui::GetIO().Framerate);

			this->Update(ImGui::GetIO().DeltaTime);
			this->Render();

			D3DUtils::GetInst().GetContext()->OMSetRenderTargets(1, SceneMgr::GetInst().GetBackBufferRTV().GetAddressOf(), nullptr);
			ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData()); // GUI ������
			D3DUtils::GetInst().GetSwapChain()->Present(1, 0);
		}
	}
	return 0;
}


bool Core::InitMainWindow()
{
	WNDCLASSEX wc = { sizeof(WNDCLASSEX),
					 CS_CLASSDC,
					 WndProc,
					 0L,
					 0L,
					 GetModuleHandle(NULL),
					 NULL,
					 NULL,
					 NULL,
					 NULL,
					 L"Space", // lpszClassName, L-string
					 NULL };
	if (!RegisterClassEx(&wc))
	{
		cout << "RegisterClassEx() failed." << endl;
		return false;
	}
	RECT wr = { 0, 0, LONG(m_fWidth), LONG(m_fHeight) };
	AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, false);
	m_mainWindow = CreateWindow(wc.lpszClassName, L"Space",
		WS_OVERLAPPEDWINDOW,
		100, // ������ ���� ����� x ��ǥ
		100, // ������ ���� ����� y ��ǥ
		wr.right - wr.left, // ������ ���� ���� �ػ�
		wr.bottom - wr.top, // ������ ���� ���� �ػ�
		NULL, NULL, wc.hInstance, NULL);

	if (!m_mainWindow)
	{
		cout << "CreateWindow() failed." << endl;
		return false;
	}

	ShowWindow(m_mainWindow, SW_SHOWDEFAULT);
	UpdateWindow(m_mainWindow);
	return true;
}

LRESULT Core::MsgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
		return true;
	switch (message)
	{
	case WM_SIZE:
		break;
	case WM_SYSCOMMAND:
		if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
			return 0;
		break;
	case WM_MOUSEMOVE:
		// cout << "Mouse " << LOWORD(lParam) << " " << HIWORD(lParam) << endl;
		break;
	case WM_LBUTTONDOWN:
		// cout << "WM_LBUTTONUP Left mouse button" << endl;
		break;
	case WM_RBUTTONUP:
		// cout << "WM_RBUTTONUP Right mouse button" << endl;
		break;
	case WM_KEYDOWN:
		// cout << "WM_KEYDOWN " << (int)wParam << endl;
		break;
	case WM_DESTROY:
		::PostQuitMessage(0);
		return 0;
	}

	return ::DefWindowProc(hWnd, message, wParam, lParam);
}

Core::Core()
	: m_fWidth(1280.0f)
	, m_fHeight(960.0f)
	, m_mainWindow(nullptr)
{
	
}

