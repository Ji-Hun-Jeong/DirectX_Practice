#include "pch.h"
#include "SceneMgr.h"
#include "Scene.h"
#include "D3DUtils.h"
#include "Core.h"
#include "ImageFilter.h"
#include "SpaceScene.h"
#include "TestScene.h"
#include "KeyMgr.h"
#include "PostProcess.h"
SceneMgr SceneMgr::m_inst;
SceneMgr::SceneMgr()
	: m_arrScene{}
	, m_curScene(nullptr)
	, m_postProcess(nullptr)
{
	m_arrScene[(UINT)SCENE_TYPE::SPACE] = make_shared<SpaceScene>();
	m_arrScene[(UINT)SCENE_TYPE::TEST] = make_shared<TestScene>();

	m_curScene = m_arrScene[(UINT)SCENE_TYPE::TEST];
}

bool SceneMgr::Init(float width, float height)
{
	m_fWidth = width;
	m_fHeight = height;
	if (!InitDirect3D())
		return false;
	if (!InitGUI())
		return false;
	for (auto& scene : m_arrScene)
	{
		if (scene)
			scene->Init();
	}
	D3D11_SHADER_RESOURCE_VIEW_DESC desc;
	m_notMsaaSRV->GetDesc(&desc);
	m_postProcess = make_shared<PostProcess>(m_fWidth, m_fHeight, 2, m_notMsaaSRV, m_renderTargetView);
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
	if (m_curScene)
		m_curScene->UpdateGUI();
	ImGui::SetWindowPos(ImVec2(0.0f, 0.0f));
	ImGui::End();
	ImGui::Render(); // 렌더링할 것들 기록 끝
	if (m_curScene)
		m_curScene->Update(dt);
	if (m_postProcess)
		m_postProcess->Update(dt);
	if (KEYCHECK(B1, TAP))
		ChangeCurScene(SCENE_TYPE::SPACE);
	else if (KEYCHECK(B2, TAP))
		ChangeCurScene(SCENE_TYPE::TEST);
}

void SceneMgr::Render()
{
	ComPtr<ID3D11DeviceContext> context = D3DUtils::GetInst().GetContext();
	float clearColor[4] = { 0.0f,0.0f,0.0f,1.0f };
	context->ClearRenderTargetView(m_msaaRTV.Get(), clearColor);
	context->ClearDepthStencilView(m_depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	context->OMSetRenderTargets(1, m_msaaRTV.GetAddressOf(), m_depthStencilView.Get());

	context->RSSetViewports(1, &m_viewPort);

	if (m_drawWireFrame)
		context->RSSetState(m_arrRSS[(UINT)RSS_TYPE::WIRE].Get());
	else
		context->RSSetState(m_arrRSS[(UINT)RSS_TYPE::SOLID].Get());
	
	if (m_curScene)
		m_curScene->Render();

	// 현재까지는 MSAA가 지원되는 RTV에 렌더링하였고 이제는 그걸 MSAA지원 안되는 RTV에 복사
	context->ResolveSubresource(m_notMsaaTexture.Get(), 0, m_msaaTexture.Get(), 0
		, DXGI_FORMAT_R16G16B16A16_FLOAT);

	if (m_postProcess)
		m_postProcess->Render(context);
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
	if (!CreateRasterizerState())
		return false;
	CreateRenderBuffer();
	CreateViewPort();
	CreateBlendState();
	SetViewPort();
	if (!CreateDepthStencilView())
		return false;
	if (!CreateDepthStencilState())
		return false;
	return true;
}
bool SceneMgr::CreateRenderTargetView()
{
	ComPtr<IDXGISwapChain>& swapChain = D3DUtils::GetInst().GetSwapChain();
	ComPtr<ID3D11Device>& device = D3DUtils::GetInst().GetDevice();
	swapChain->GetBuffer(0, IID_PPV_ARGS(m_swapChainBackBuffer.GetAddressOf()));
	HRESULT result = device->CreateShaderResourceView(m_swapChainBackBuffer.Get(), nullptr, m_shaderResourceView.GetAddressOf());
	CHECKRESULT(result);
	return D3DUtils::GetInst().CreateRenderTargetView(m_swapChainBackBuffer.Get(), nullptr, m_renderTargetView);
}

bool SceneMgr::CreateRasterizerState()
{
	D3D11_RASTERIZER_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.FillMode = D3D11_FILL_SOLID;	// WireFrame
	desc.CullMode = D3D11_CULL_BACK;	// None, Front
	desc.FrontCounterClockwise = false;
	desc.DepthClipEnable = true;
	desc.MultisampleEnable = true;

	D3DUtils::GetInst().CreateRasterizerState(&desc, m_arrRSS[(UINT)RSS_TYPE::SOLID]);
	
	desc.FrontCounterClockwise = true;
	D3DUtils::GetInst().CreateRasterizerState(&desc, m_arrRSS[(UINT)RSS_TYPE::SOLIDCCW]);

	desc.FillMode = D3D11_FILL_WIREFRAME;
	D3DUtils::GetInst().CreateRasterizerState(&desc, m_arrRSS[(UINT)RSS_TYPE::WIRECCW]);

	desc.FrontCounterClockwise = false;
	D3DUtils::GetInst().CreateRasterizerState(&desc, m_arrRSS[(UINT)RSS_TYPE::WIRE]);

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

void SceneMgr::CreateRenderBuffer()
{
	ComPtr<ID3D11Device>& device = D3DUtils::GetInst().GetDevice();
	CHECKRESULT(device
		->CheckMultisampleQualityLevels(DXGI_FORMAT_R16G16B16A16_FLOAT, 4, &m_iNumOfMultiSamplingLevel));

	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.Width = m_fWidth;
	desc.Height = m_fHeight;
	desc.MipLevels = desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	if (m_iNumOfMultiSamplingLevel)
	{
		desc.SampleDesc.Count = 4;
		desc.SampleDesc.Quality = m_iNumOfMultiSamplingLevel - 1;
	}
	else
	{
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
	}
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;
	CHECKRESULT(device->CreateTexture2D(&desc, nullptr, m_msaaTexture.GetAddressOf()));
	CHECKRESULT(device->CreateShaderResourceView(m_msaaTexture.Get(), nullptr, m_msaaSRV.GetAddressOf()));
	CHECKRESULT(device->CreateRenderTargetView(m_msaaTexture.Get(), nullptr, m_msaaRTV.GetAddressOf()));

	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	CHECKRESULT(device->CreateTexture2D(&desc, nullptr, m_notMsaaTexture.GetAddressOf()));
	CHECKRESULT(device->CreateShaderResourceView(m_notMsaaTexture.Get(), nullptr, m_notMsaaSRV.GetAddressOf()));
	CHECKRESULT(device->CreateRenderTargetView(m_notMsaaTexture.Get(), nullptr, m_notMsaaRTV.GetAddressOf()));
}

void SceneMgr::SetViewPort()
{
	D3DUtils::GetInst().SetViewPort(&m_viewPort);
}

void SceneMgr::CreateBlendState()
{
	D3D11_BLEND_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.AlphaToCoverageEnable = true; // MSAA
	desc.IndependentBlendEnable = false;
	// 개별 RenderTarget에 대해서 설정 (최대 8개)
	desc.RenderTarget[0].BlendEnable = true;
	desc.RenderTarget[0].SrcBlend = D3D11_BLEND_INV_BLEND_FACTOR;
	desc.RenderTarget[0].DestBlend = D3D11_BLEND_BLEND_FACTOR;
	desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;

	desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
	desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;

	// 필요하면 RGBA 각각에 대해서도 조절 가능
	desc.RenderTarget[0].RenderTargetWriteMask =
		D3D11_COLOR_WRITE_ENABLE_ALL;

	D3DUtils::GetInst().CreateBlendState(&desc, m_blendState);
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

	return D3DUtils::GetInst().CreateDepthStencilView(&depthBufferDesc, m_depthBuffer, nullptr, m_depthStencilView);
}

bool SceneMgr::CreateDepthStencilState()
{
	D3D11_DEPTH_STENCIL_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	{
		desc.DepthEnable = true;
		desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		desc.DepthFunc = D3D11_COMPARISON_LESS;
		desc.StencilEnable = false; // Stencil 불필요
		desc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
		desc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
		// 앞면에 대해서 어떻게 작동할지 설정
		desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;	// stencil fail
		desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;	// stencil success depth fail
		desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;	// stnencil success depth success
		desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		// 뒷면에 대해 어떻게 작동할지 설정 (뒷면도 그릴 경우)
		desc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		desc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
		desc.BackFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
		desc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	}
	D3DUtils::GetInst().CreateDepthStencilState(&desc, m_arrDSS[(UINT)DSS_TYPE::BASIC]);

	{
		desc.DepthFunc = D3D11_COMPARISON_LESS;
		desc.StencilEnable = true;
		desc.StencilReadMask = 0xFF;
		desc.StencilWriteMask = 0xFF;
		desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
		desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;	// 모든 비교를 다 통과하면 stencil을 업데이트
		desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;	// OMSetDepthStencilState에서 설정한 ref값을 항상 넣어줌
	}
	D3DUtils::GetInst().CreateDepthStencilState(&desc, m_arrDSS[(UINT)DSS_TYPE::MASK]);

	{
		desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;	// 거울을 그자리에 다시 그려야 하기 때문
		desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
		desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		desc.FrontFace.StencilFunc = D3D11_COMPARISON_EQUAL;	// OMSetDepthStencilState에서 설정한 ref값과 같은놈만 그림
	}
	D3DUtils::GetInst().CreateDepthStencilState(&desc, m_arrDSS[(UINT)DSS_TYPE::DRAWMASK]);
	
	return true;
}
