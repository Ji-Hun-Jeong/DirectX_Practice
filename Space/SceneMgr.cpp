#include "pch.h"
#include "SceneMgr.h"
#include "Scene.h"
#include "D3DUtils.h"
#include "Core.h"
#include "GraphicsCommons.h"
#include "Light.h"
#include "KeyMgr.h"

#include "SpaceScene.h"
#include "RenderScene.h"
#include "AnimateScene.h"
#include "SpriteScene.h"
#include "SPHScene.h"
#include "GridSimulation.h"
#include "CurlNoise.h"

SceneMgr SceneMgr::m_inst;
SceneMgr::SceneMgr()
	: m_arrScene{}
	, m_curScene(nullptr)
	, m_fWidth(0.0f)
	, m_fHeight(0.0f)
	, m_viewPort{}
	, m_iNumOfMultiSamplingLevel(0)
{
	
}

bool SceneMgr::Init(float width, float height)
{
	m_fWidth = width;
	m_fHeight = height;
	if (!InitDirect3D())
		return false;
	if (!InitGUI())
		return false;

	Graphics::InitCommonStates();

	m_arrScene[(UINT)SCENE_TYPE::SPACE] = make_shared<SpaceScene>(this);
	m_arrScene[(UINT)SCENE_TYPE::RENDER] = make_shared<RenderScene>(this);
	m_arrScene[(UINT)SCENE_TYPE::SPRITE] = make_shared<SpriteScene>(this);
	m_arrScene[(UINT)SCENE_TYPE::SPH] = make_shared<SPHScene>(this);
	m_arrScene[(UINT)SCENE_TYPE::GRIDSIMULATION] = make_shared<GridSimulation>(this);
	m_arrScene[(UINT)SCENE_TYPE::CURLNOISE] = make_shared<CurlNoise>(this);
	m_curScene = m_arrScene[(UINT)SCENE_TYPE::CURLNOISE];

	for (auto& scene : m_arrScene)
	{
		if (scene)
			scene->Init();
	}

	m_curScene->Enter();

	return true;
}

bool SceneMgr::InitGUI()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	(void)io;
	io.DisplaySize = ImVec2(float(m_fWidth), float(m_fHeight));
	ImGui::StyleColorsLight();

	// Setup Platform/Renderer backends

	if (!ImGui_ImplDX11_Init(D3DUtils::GetInst().GetDevice().Get(), D3DUtils::GetInst().GetContext().Get())) {
		return false;
	}

	if (!ImGui_ImplWin32_Init(Core::GetInst().GetMainWindow())) {
		return false;
	}

	return true;
}

void SceneMgr::Update(float dt)
{
	m_curScene->UpdateGUI();

	ImGui::SetWindowPos(ImVec2(0.0f, 0.0f));
	ImGui::End();
	ImGui::Render(); // 렌더링할 것들 기록 끝

	m_curScene->Update(dt);

	// m_postProcess->Update(dt);
	if (KEYCHECK(B1, TAP));
		//ChangeCurScene(SCENE_TYPE::SPACE);

	else if (KEYCHECK(B2, TAP))
		ChangeCurScene(SCENE_TYPE::RENDER);

	else if (KEYCHECK(B3, TAP))
		ChangeCurScene(SCENE_TYPE::SPRITE);

	else if (KEYCHECK(B4, TAP))
		ChangeCurScene(SCENE_TYPE::SPH);

	else if (KEYCHECK(B5, TAP))
		ChangeCurScene(SCENE_TYPE::GRIDSIMULATION);

	else if (KEYCHECK(B6, TAP))
		ChangeCurScene(SCENE_TYPE::CURLNOISE);
}

void SceneMgr::Render()
{
	ComPtr<ID3D11DeviceContext> context = D3DUtils::GetInst().GetContext();
	m_curScene->Render(context, m_drawWireFrame);
}

void SceneMgr::ChangeCurScene(SCENE_TYPE scene)
{
	m_curScene->Exit();
	m_curScene = m_arrScene[(UINT)scene];
	m_curScene->Enter();
}

bool SceneMgr::InitDirect3D()
{
	if (!D3DUtils::GetInst().CreateDeviceAndSwapChain())
		return false;
	if (!CreateRenderTargetView())
		return false;
	CreateViewPort();
	SetViewPort();
	if (!CreateDepthStencilView())
		return false;
	return true;
}
bool SceneMgr::CreateRenderTargetView()
{
	ComPtr<IDXGISwapChain>& swapChain = D3DUtils::GetInst().GetSwapChain();
	ComPtr<ID3D11Device>& device = D3DUtils::GetInst().GetDevice();

	CHECKRESULT(device
		->CheckMultisampleQualityLevels(DXGI_FORMAT_R16G16B16A16_FLOAT, 4, &m_iNumOfMultiSamplingLevel));

	swapChain->GetBuffer(0, IID_PPV_ARGS(m_swapChainBackBuffer.GetTexture().GetAddressOf()));
	HRESULT result = device->CreateShaderResourceView(m_swapChainBackBuffer.GetTexture().Get(), nullptr, m_swapChainBackBuffer.GetSRV().GetAddressOf());
	CHECKRESULT(result);
	D3DUtils::GetInst().CreateRenderTargetView(m_swapChainBackBuffer.GetTexture().Get(), nullptr, m_swapChainBackBuffer.GetRTV());
	return true;
}

void SceneMgr::CreateViewPort()
{
	ZeroMemory(&m_viewPort, sizeof(m_viewPort));
	m_viewPort.TopLeftX = 0.0f;
	m_viewPort.TopLeftY = 0.0f;
	m_viewPort.Width = m_fWidth;
	m_viewPort.Height = m_fHeight;
	m_viewPort.MinDepth = 0.0f;
	m_viewPort.MaxDepth = 1.0f;

}

void SceneMgr::SetViewPort()
{
	D3DUtils::GetInst().SetViewPort(&m_viewPort);
}

bool SceneMgr::CreateDepthStencilView()
{
	// DepthBuffer
	D3D11_TEXTURE2D_DESC depthBufferDesc;
	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));
	depthBufferDesc.Width = UINT(m_fWidth);
	depthBufferDesc.Height = UINT(m_fHeight);
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	// 깊이에 24비트, 스텐실에 8비트 지원 32비트 z-버퍼 형식
	// 24비트를 깊이에 사용하지만 결국 0~1 사이로 정규화 하기에 UNORM
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	if (m_iNumOfMultiSamplingLevel > 0)
	{
		depthBufferDesc.SampleDesc.Count = 4;
		depthBufferDesc.SampleDesc.Quality = m_iNumOfMultiSamplingLevel - 1;
	}
	else
	{
		depthBufferDesc.SampleDesc.Count = 1;
		depthBufferDesc.SampleDesc.Quality = 0;
	}
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;

	D3DUtils::GetInst().CreateDepthStencilView(&depthBufferDesc, m_useMSAADepthBuffer, nullptr, m_useMSAADSV);

	return true;
}
