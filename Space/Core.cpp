#include "pch.h"
#include "Core.h"
#include "D3DUtils.h"
#include "GeometryGenerator.h"
#include "Mesh.h"
#include "CubeMap.h"
#include "KeyMgr.h"
#include "Camera.h"
#include "ImageFilter.h"
#include "DirArrow.h"
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
	if (!InitDirect3D())
		return false;
	if (!InitGUI())
		return false;
	InitMesh();
	InitCubeMap();
	CreateFilters();
	return true;
}

void Core::InitMesh()
{
	MeshData solarData = GeometryGenerator::MakeSphere(109, 30, 30, "image/Solar.jpg");
	auto solar = make_shared<Mesh>(Vector3(0.0f), Vector3{ 0.0f,0.01f,0.0f }, Vector3{ 0.0f,0.0f,0.0f }, Vector3(1.0f));
	solar->Init("Solar", solarData, L"Basic", L"Basic");
	solar->GetPixelConstantData().isSun=1;
	m_vecMeshes.push_back(solar);

	MeshData mercuryData = GeometryGenerator::MakeSphere(0.38f, 30, 30, "image/Mercury.jpg");
	auto mercury = make_shared<Mesh>(Vector3(111.0f, 0.0f, 0.0f), Vector3{ 0.0f,0.000015f,0.000614f }, Vector3{ 0.0f,0.1f,0.0f }, Vector3(1.0f));
	mercury->Init("Mercury", mercuryData, L"Basic", L"Basic");
	m_vecMeshes.push_back(mercury);

	MeshData venusData = GeometryGenerator::MakeSphere(0.94f, 30, 30, "image/Venus.jpg");
	auto venus = make_shared<Mesh>(Vector3(112.6f, 0.0f, 0.0f), Vector3{ 0.0f,0.000001f,3.096f }, Vector3{ 0.0f,0.073f,0.0f }, Vector3(1.0f));
	venus->Init("Venus", venusData, L"Basic", L"Basic");
	m_vecMeshes.push_back(venus);

	MeshData earthData = GeometryGenerator::MakeSphere(1.0f, 30, 30, "image/earth.jpg");
	auto earth = make_shared<Mesh>(Vector3(114.16f, 0.0f, 0.0f), Vector3{ 0.0f,0.0023f,0.4091f }, Vector3{ 0.0f,0.062f,0.0f }, Vector3(1.0f));
	earth->Init("Earth", earthData, L"Basic", L"Basic");
	m_vecMeshes.push_back(earth);

	MeshData marsData = GeometryGenerator::MakeSphere(0.53f, 30, 30, "image/Mars.jpg");
	auto mars = make_shared<Mesh>(Vector3(116.84f, 0.0f, 0.0f), Vector3{ 0.0f,0.0012f,0.4396f }, Vector3{ 0.0f,0.05f,0.0f }, Vector3(1.0f));
	mars->Init("Mars", marsData, L"Basic", L"Basic");
	m_vecMeshes.push_back(mars);

	MeshData jupiterData = GeometryGenerator::MakeSphere(10.97f, 30, 30, "image/Jupiter.jpg");
	auto jupiter = make_shared<Mesh>(Vector3(135.86f, 0.0f, 0.0f), Vector3{ 0.0f,0.063f,0.0546f }, Vector3{ 0.0f,0.027f,0.0f }, Vector3(1.0f));
	jupiter->Init("Jupiter", jupiterData, L"Basic", L"Basic");
	m_vecMeshes.push_back(jupiter);

	MeshData saturnData = GeometryGenerator::MakeSphere(9.14f, 100, 100, "image/Saturn.jpg");
	auto saturn = make_shared<Mesh>(Vector3(158.4f, 0.0f, 0.0f), Vector3{ 0.0f,0.05f,0.4665f }, Vector3{ 0.0f,0.02f,0.0f }, Vector3(1.0f));
	saturn->Init("Saturn", saturnData, L"Basic", L"Basic");

	MeshData saturnRingData = GeometryGenerator::MakeDisc(10.0f, 20.0f, 50, "image/SaturnRing.jpg");
	auto saturnRing = make_shared<Mesh>(Vector3(0.0f),Vector3{-45.0f,0.5f,0.0f},Vector3(0.0f),Vector3(1.0f));
	saturnRing->Init("SaturnRing", saturnRingData, L"Basic", L"Basic");
	saturn->AttachMesh(saturnRing);
	m_vecMeshes.push_back(saturn);

	MeshData uranusData = GeometryGenerator::MakeSphere(3.98f, 30, 30, "image/Uranus.jpg");
	auto uranus = make_shared<Mesh>(Vector3(208.1f, 0.0f, 0.0f), Vector3{ 0.0f,0.0129f,1.7064f }, Vector3{ 0.0f,0.014f,0.0f }, Vector3(1.0f));
	uranus->Init("Uranus", uranusData, L"Basic", L"Basic");
	m_vecMeshes.push_back(uranus);

	MeshData neptuneData = GeometryGenerator::MakeSphere(3.86f, 30, 30, "image/Neptune.jpg");
	auto neptune = make_shared<Mesh>(Vector3(264.68f, 0.0f, 0.0f), Vector3{ 0.0f,0.0134f,0.4943f }, Vector3{ 0.0f,0.011f,0.0f }, Vector3(1.0f));
	neptune->Init("Neptune", neptuneData, L"Basic", L"Basic");
	m_vecMeshes.push_back(neptune);

	MeshData groundData = GeometryGenerator::MakeSquare();
	groundData.textureName = "image/Earth.jpg";
	auto ground = make_shared<Mesh>(Vector3{ 0.0f,-200.0f,0.0f }, Vector3{ 90.0f,0.0f,0.0f }, Vector3(0.0f), Vector3(200.0f));
	ground->Init("Ground", groundData, L"Basic", L"Basic");
	m_vecMeshes.push_back(ground);

	MeshData arrowData = GeometryGenerator::MakeTriangle();
	auto dirArrowMesh = make_shared<DirArrow>(Vector3{ 0.9f,-0.7f,1.0f }, Vector3(0.0f), Vector3(0.0f), Vector3(1.0f));
	dirArrowMesh->Init("DirArrow", arrowData, L"DirArrow", L"DirArrow");
	m_vecMeshes.push_back(dirArrowMesh);

	m_focusMesh = solar;
}

void Core::InitCubeMap()
{
	MeshData cubeMapData = GeometryGenerator::MakeSphere(1000.0f, 30, 30, "image/space.dds");
	std::reverse(cubeMapData.indices.begin(), cubeMapData.indices.end());
	auto cubeMap = make_shared<CubeMap>();
	cubeMap->Init("CubeMap", cubeMapData, L"CubeMap", L"CubeMap");
	m_cubeMap = cubeMap;
}

bool Core::InitGUI()
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

	if (!ImGui_ImplWin32_Init(m_mainWindow)) {
		return false;
	}

	return true;
}

void Core::Update(float dt)
{
	KeyMgr::GetInst().Update();
	if (KEYCHECK(ESC, TAP))
		exit(0);

	m_camera->Update(dt);
	m_pixelConstantData.eyePos = m_camera->GetPos();

	for (auto& meshGroup : m_vecMeshes)
	{
		if (meshGroup)
			meshGroup->Update(dt);
	}

	if (m_cubeMap)
		m_cubeMap->Update(dt);

	for (auto& filter : m_filters)
	{
		if (filter)
			filter->Update(dt);
	}
}

void Core::UpdateGUI()
{
	ImGui::Checkbox("DrawWireFrame", &m_drawWireFrame);
	ImGui::Checkbox("DrawNormal", &m_drawNormal);
	ImGui::Checkbox("Use Rim", &m_pixelConstantData.rim.useRim);
	ImGui::SliderFloat("NormalSize", &m_normalSize, 0.0f, 100.0f);
	ImGui::SliderFloat("Rim Strength", &m_pixelConstantData.rim.rimStrength, 0.0f, 15.0f);
	ImGui::SliderFloat("Rim Power", &m_pixelConstantData.rim.rimPower, 0.0f, 5.0f);
	ImGui::SliderFloat("Threshold", &m_pixelConstantData.bloom.threshold, 0.0f, 1.0f);
	ImGui::SliderFloat("BloomLightStrength", &m_pixelConstantData.bloom.bloomStrength, 0.0f, 1.0f);
	ImGui::SliderFloat3("Rotation", &m_focusMesh->m_rotation1.x, 0.0f, 3.14f);
	ImGui::SliderFloat("LightStrength", &m_pixelConstantData.light.lightStrength.x, 0.0f, 1.0f);
	ImGui::SliderFloat3("LightPos", &m_pixelConstantData.light.lightPos.x, -1000.0f, 1000.0f);
	ImGui::SliderFloat("fallOfStart", &m_pixelConstantData.light.fallOfStart, 0.0f, 1000.0f);
	ImGui::SliderFloat("fallOfEnd", &m_pixelConstantData.light.fallOfEnd, 0.0f, 1000.0f);
	ImGui::SliderFloat3("Ambient", &m_pixelConstantData.mat.ambient.x, 0.0f, 1.0f);
	ImGui::SliderFloat3("Diffuse", &m_pixelConstantData.mat.diffuse.x, 0.0f, 1.0f);
	ImGui::SliderFloat3("Specular", &m_pixelConstantData.mat.specular.x, 0.0f, 1.0f);
	ImGui::SliderFloat("Shiness", &m_pixelConstantData.mat.shiness, 0.0f, 10.0f);
}


void Core::Render()
{
	ComPtr<ID3D11DeviceContext> context = D3DUtils::GetInst().GetContext();
	float clearColor[4] = { 0.0f,0.0f,0.0f,1.0f };
	context->ClearRenderTargetView(m_renderTargetView.Get(), clearColor);
	context->ClearDepthStencilView(m_depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	if (m_drawWireFrame)
		context->RSSetState(m_wireRasterizerState.Get());
	else
		context->RSSetState(m_solidRasterizerState.Get());

	context->OMSetRenderTargets(1, m_renderTargetView.GetAddressOf(), m_depthStencilView.Get());
	context->OMSetDepthStencilState(m_depthStencilState.Get(), 0);

	for (auto& mesh : m_vecMeshes)
	{
		if (mesh)
			mesh->Render(context.Get(), m_drawNormal);
	}

	if (m_cubeMap)
		m_cubeMap->Render(context.Get(), m_drawNormal);

	for (auto& filter : m_filters)
	{
		if (filter)
			filter->Render(context.Get(), m_drawNormal);
	}
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
			ImGui_ImplDX11_NewFrame(); // GUI 프레임 시작
			ImGui_ImplWin32_NewFrame();

			ImGui::NewFrame(); // 어떤 것들을 렌더링 할지 기록 시작
			ImGui::Begin("Scene Control");

			// ImGui가 측정해주는 Framerate 출력
			ImGui::Text("Average %.3f ms/frame (%.1f FPS)",
				1000.0f / ImGui::GetIO().Framerate,
				ImGui::GetIO().Framerate);
			UpdateGUI();

			ImGui::SetWindowPos(ImVec2(0.0f, 0.0f));
			ImGui::End();
			ImGui::Render(); // 렌더링할 것들 기록 끝

			Update(ImGui::GetIO().DeltaTime);
			Render();
			ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData()); // GUI 렌더링
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
		100, // 윈도우 좌측 상단의 x 좌표
		100, // 윈도우 좌측 상단의 y 좌표
		wr.right - wr.left, // 윈도우 가로 방향 해상도
		wr.bottom - wr.top, // 윈도우 세로 방향 해상도
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

bool Core::InitDirect3D()
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


bool Core::CreateRenderTargetView()
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

bool Core::CreateRasterizerState()
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

void Core::CreateViewPort()
{
	ZeroMemory(&m_viewPort, sizeof(m_viewPort));
	m_viewPort.TopLeftX = 0.0f;
	m_viewPort.TopLeftY = 0.0f;
	m_viewPort.Width = m_fWidth;
	m_viewPort.Height = m_fHeight;
	m_viewPort.MinDepth = 0.0f;
	m_viewPort.MaxDepth = 1.0f;
}

void Core::SetViewPort()
{
	D3DUtils::GetInst().SetViewPort(&m_viewPort);
}

bool Core::CreateDepthStencilView()
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

bool Core::CreateDepthStencilState()
{
	D3D11_DEPTH_STENCIL_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.DepthEnable = true;
	desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	return D3DUtils::GetInst().CreateDepthStencilState(&desc, m_depthStencilState);
}

void Core::CreateFilters()
{
	m_pixelConstantData.bloom.dx = 1.0f / m_fWidth;
	m_pixelConstantData.bloom.dy = 1.0f / m_fHeight;

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
	case WM_LBUTTONUP:
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
	, m_iNumOfMultiSamplingLevel(0)
	, m_mainWindow(nullptr)
	, m_viewPort({})
{
	m_camera = make_shared<Camera>();
}

