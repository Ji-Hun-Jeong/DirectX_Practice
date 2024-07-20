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
#include "GraphicsCommons.h"
#include "GraphicsPSO.h"
#include "Mesh.h"
#include "GeometryGenerator.h"
#include "Light.h"
SceneMgr SceneMgr::m_inst;
SceneMgr::SceneMgr()
	: m_arrScene{}
	, m_curScene(nullptr)
	, m_fWidth(0.0f)
	, m_fHeight(0.0f)
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

	Graphics::InitCommonStates();
	m_postProcess = make_shared<PostProcess>(m_fWidth, m_fHeight, 2, m_postEffectsSRV, m_renderTargetView);
	MeshData squareData = GeometryGenerator::MakeSquare();
	m_postEffects = make_shared<Mesh>();
	m_postEffects->Init(squareData);
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
	m_postEffects->Update(dt);

	// m_postProcess->Update(dt);
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
	context->ClearRenderTargetView(m_notMsaaRTV.Get(), clearColor);

	context->RSSetViewports(1, &m_viewPort);
	{
		m_curScene->RenderDepthOnly(context, m_depthOnlyDSV);
		m_curScene->RenderLightView(context);
	}

	{
		context->ClearDepthStencilView(m_useMSAADSV.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
		context->OMSetRenderTargets(1, m_msaaRTV.GetAddressOf(), m_useMSAADSV.Get());

		m_curScene->Render(context, m_drawWireFrame);
	}

	// 현재까지는 MSAA가 지원되는 RTV에 렌더링하였고 이제는 그걸 MSAA지원 안되는 RTV에 복사
	context->ResolveSubresource(m_notMsaaTexture.Get(), 0, m_msaaTexture.Get(), 0
		, DXGI_FORMAT_R16G16B16A16_FLOAT);

	{
		Graphics::g_postEffectsPSO.Setting();
		vector<ID3D11ShaderResourceView*> srv =
		{ m_notMsaaSRV.Get(), m_depthOnlySRV.Get()/*GETCURSCENE()->m_vecLights[0]->GetLightViewSRV().Get()*/};
		context->PSSetShaderResources(20, srv.size(), srv.data());
		context->OMSetRenderTargets(1, m_postEffectsRTV.GetAddressOf(), nullptr); // DSV사용 x
		m_postEffects->Render(context);
	}

	{
		Graphics::g_postProcessPSO.Setting();
		m_postProcess->Render(context);
	}
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
	CreateRenderBuffer();
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
	swapChain->GetBuffer(0, IID_PPV_ARGS(m_swapChainBackBuffer.GetAddressOf()));
	HRESULT result = device->CreateShaderResourceView(m_swapChainBackBuffer.Get(), nullptr, m_shaderResourceView.GetAddressOf());
	CHECKRESULT(result);
	return D3DUtils::GetInst().CreateRenderTargetView(m_swapChainBackBuffer.Get(), nullptr, m_renderTargetView);
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

	CHECKRESULT(device->CreateTexture2D(&desc, nullptr, m_postEffectsTexture.GetAddressOf()));
	CHECKRESULT(device->CreateShaderResourceView(m_postEffectsTexture.Get(), nullptr, m_postEffectsSRV.GetAddressOf()));
	CHECKRESULT(device->CreateRenderTargetView(m_postEffectsTexture.Get(), nullptr, m_postEffectsRTV.GetAddressOf()));
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

	D3DUtils::GetInst().CreateDepthOnlyResources(m_fWidth, m_fHeight, m_depthOnlyBuffer, m_depthOnlyDSV, m_depthOnlySRV);

	return true;
}
