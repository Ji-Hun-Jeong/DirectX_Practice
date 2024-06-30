#include "pch.h"
#include "SceneMgr.h"
#include "Scene.h"
#include "D3DUtils.h"
#include "Core.h"
#include "ImageFilter.h"
#include "SpaceScene.h"
#include "TestScene.h"
#include "KeyMgr.h"
SceneMgr SceneMgr::m_inst;
SceneMgr::SceneMgr()
	: m_arrScene{}
	, m_curScene(nullptr)
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
	CreateFilters();
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
	for (auto& filter : m_filters)
	{
		if (filter)
			filter->Update(dt);
	}
	if (KEYCHECK(B1, TAP))
		ChangeCurScene(SCENE_TYPE::SPACE);
	else if (KEYCHECK(B2, TAP))
		ChangeCurScene(SCENE_TYPE::TEST);
}

void SceneMgr::Render()
{
	ComPtr<ID3D11DeviceContext> context = D3DUtils::GetInst().GetContext();
	float clearColor[4] = { 0.0f,0.0f,0.0f,1.0f };
	context->ClearRenderTargetView(m_renderTargetView.Get(), clearColor);
	context->ClearDepthStencilView(m_depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	context->OMSetRenderTargets(1, m_renderTargetView.GetAddressOf(), m_depthStencilView.Get());
	context->OMSetDepthStencilState(m_depthStencilState.Get(), 0);

	if (m_drawWireFrame)
		context->RSSetState(m_wireRasterizerState.Get());
	else
		context->RSSetState(m_solidRasterizerState.Get());
	context->RSSetViewports(1, &m_viewPort);

	if (m_curScene)
		m_curScene->Render();

	for (auto& filter : m_filters)
	{
		if (filter)
			filter->Render(context.Get());
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
	if (!D3DUtils::GetInst().CreateDeviceAndSwapChain(m_iNumOfMultiSamplingLevel))
		return false;
	if (!CreateRenderTargetView())
		return false;
	D3D11_RENDER_TARGET_VIEW_DESC desc;
	m_renderTargetView->GetDesc(&desc);
	if (!CreateRasterizerState())
		return false;
	CreateViewPort();
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
	ComPtr<ID3D11Texture2D> swapChainBackBuffer;
	swapChain->GetBuffer(0, IID_PPV_ARGS(swapChainBackBuffer.GetAddressOf()));
	HRESULT result = device->CreateShaderResourceView(swapChainBackBuffer.Get(), nullptr, m_shaderResourceView.GetAddressOf());
	if (FAILED(result))
		assert(0);
	return D3DUtils::GetInst().CreateRenderTargetView(swapChainBackBuffer.Get(), nullptr, m_renderTargetView);
}

bool SceneMgr::CreateRasterizerState()
{
	D3D11_RASTERIZER_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.FillMode = D3D11_FILL_SOLID;	// WireFrame
	desc.CullMode = D3D11_CULL_BACK;	// None, Front
	desc.FrontCounterClockwise = false;
	desc.DepthClipEnable = true;

	bool result = D3DUtils::GetInst().CreateRasterizerState(&desc, m_solidRasterizerState);
	if (!result)
		return false;

	desc.FillMode = D3D11_FILL_WIREFRAME;
	result = D3DUtils::GetInst().CreateRasterizerState(&desc, m_wireRasterizerState);
	if (!result)
		return false;
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
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; // 깊이에 24비트, 스텐실에 8비트 지원 32비트 z-버퍼 형식입니다.
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
	desc.DepthEnable = true;
	desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	return D3DUtils::GetInst().CreateDepthStencilState(&desc, m_depthStencilState);
}

void SceneMgr::CreateFilters()
{
	GETCURSCENE()->m_pixelConstantData.bloom.dx = 1.0f / m_fWidth;
	GETCURSCENE()->m_pixelConstantData.bloom.dy = 1.0f / m_fHeight;

	auto copyFilter = make_shared<ImageFilter>(m_fWidth, m_fHeight, L"Copy", L"Copy");
	copyFilter->SetShaderResourceViews({ m_shaderResourceView.Get() });
	m_filters.push_back(copyFilter);

	// blur
	// DownSampling으로 Aliasing 현상을 줄임
	int down = 64;
	for (int height = 2; height <= down; height *= 2)
	{
		auto downFilter = make_shared<ImageFilter>(m_fWidth / height, m_fHeight / height, L"Copy", L"Copy");
		downFilter->SetShaderResourceViews({ m_filters.back()->GetShaderResourceView().Get() });
		m_filters.push_back(downFilter);
	}

	// Blur처리를 하면서 원본 크기까지 UpSampling
	for (int height = down; height >= 1; height /= 2)
	{
		for (int i = 0; i < 5; ++i)
		{
			auto blurXFilter = make_shared<ImageFilter>(m_fWidth / height, m_fHeight / height, L"Copy", L"BlurX");
			blurXFilter->SetShaderResourceViews({ m_filters.back()->GetShaderResourceView().Get() });
			m_filters.push_back(blurXFilter);

			auto blurYFilter = make_shared<ImageFilter>(m_fWidth / height, m_fHeight / height, L"Copy", L"BlurY");
			blurYFilter->SetShaderResourceViews({ m_filters.back()->GetShaderResourceView().Get() });
			m_filters.push_back(blurYFilter);
		}
	}

	// 결과를 Bloom
	auto bloomFilter = make_shared<ImageFilter>(m_fWidth, m_fHeight, L"Copy", L"Bloom");
	bloomFilter->SetShaderResourceViews({ m_filters.back()->GetShaderResourceView().Get() });
	m_filters.push_back(bloomFilter);

	// 원본영상에 합침
	auto combineFilter = make_shared<ImageFilter>(m_fWidth, m_fHeight, L"Copy", L"Combine");
	combineFilter->SetShaderResourceViews(
		{
			copyFilter->GetShaderResourceView().Get(),
			m_filters.back()->GetShaderResourceView().Get() }
	);
	combineFilter->SetRenderTargetViews({ m_renderTargetView.Get() });
	m_filters.push_back(combineFilter);
}
