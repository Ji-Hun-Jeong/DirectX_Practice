#include "pch.h"
#include "GraphicsCommons.h"
#include "D3DUtils.h"
#include "GraphicsPSO.h"

namespace Graphics
{
	// Samplers
	ComPtr<ID3D11SamplerState> g_linearSampler;
	ComPtr<ID3D11SamplerState> g_clampSampler;
	vector<ID3D11SamplerState*> g_vecSamplers;

	// VertexShader
	ComPtr<ID3D11VertexShader> g_basicVS;
	ComPtr<ID3D11VertexShader> g_skyboxVS;
	ComPtr<ID3D11VertexShader> g_normalVS;
	ComPtr<ID3D11VertexShader> g_copyVS;
	ComPtr<ID3D11VertexShader> g_dirArrowVS;

	// InputLayout
	ComPtr<ID3D11InputLayout> g_basicInputLayout;

	// GeometryShader
	ComPtr<ID3D11GeometryShader> g_basicGS;
	ComPtr<ID3D11GeometryShader> g_normalGS;
	ComPtr<ID3D11GeometryShader> g_dirArrowGS;

	// PixelShader
	ComPtr<ID3D11PixelShader> g_basicPS;
	ComPtr<ID3D11PixelShader> g_skyboxPS;
	ComPtr<ID3D11PixelShader> g_normalPS;
	ComPtr<ID3D11PixelShader> g_copyPS;
	ComPtr<ID3D11PixelShader> g_blurPS;
	ComPtr<ID3D11PixelShader> g_combinePS;
	ComPtr<ID3D11PixelShader> g_dirArrowPS;

	// RasterizerState
	ComPtr<ID3D11RasterizerState> g_solidCWRS;
	ComPtr<ID3D11RasterizerState> g_solidCCWRS;
	ComPtr<ID3D11RasterizerState> g_wireCWRS;
	ComPtr<ID3D11RasterizerState> g_wireCCWRS;
	ComPtr<ID3D11RasterizerState> g_postProcessRS;

	// DepthStencilState
	ComPtr<ID3D11DepthStencilState> g_basicDSS;
	ComPtr<ID3D11DepthStencilState> g_maskDSS;
	ComPtr<ID3D11DepthStencilState> g_drawMaskDSS;

	// BlendState
	ComPtr<ID3D11BlendState> g_basicBS;

	// PSO
	GraphicsPSO g_defaultSolidPSO;
	GraphicsPSO g_defaultWirePSO;
	GraphicsPSO g_skyBoxSolidPSO;
	GraphicsPSO g_skyBoxWirePSO;
	GraphicsPSO g_normalPSO;
	GraphicsPSO g_stencilMaskSolidPSO;
	GraphicsPSO g_stencilMaskWirePSO;
	GraphicsPSO g_drawMaskSolidPSO;
	GraphicsPSO g_drawMaskWirePSO;
	GraphicsPSO g_drawMaskSkyBoxSolidPSO;
	GraphicsPSO g_drawMaskSkyBoxWirePSO;
	GraphicsPSO g_blendSolidPSO;
	GraphicsPSO g_blendWirePSO;
	GraphicsPSO g_postProcessPSO;

	void InitCommonStates()
	{
		InitSamplers();
		InitShaders();
		InitRasterizerState();
		InitDepthStencilState();
		InitBlendState();
		InitGraphicsPSO();
	}

	void InitSamplers()
	{
		D3D11_SAMPLER_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		// Linear Sampler
		desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		desc.MinLOD = 0;
		desc.MaxLOD = D3D11_FLOAT32_MAX;
		D3DUtils::GetInst().CreateSamplerState(g_linearSampler, desc);

		// Clamp Sampler
		desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
		D3DUtils::GetInst().CreateSamplerState(g_clampSampler, desc);

		g_vecSamplers.push_back(g_linearSampler.Get());
		g_vecSamplers.push_back(g_clampSampler.Get());
	}

	void InitShaders()
	{
		vector<D3D11_INPUT_ELEMENT_DESC> desc =
		{
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0},
		};

		D3DUtils::GetInst().CreateVertexShaderAndInputLayout(L"Basic", g_basicVS, desc, g_basicInputLayout);
		D3DUtils::GetInst().CreateVertexShaderAndInputLayout(L"SkyBox", g_skyboxVS, desc, g_basicInputLayout);
		D3DUtils::GetInst().CreateVertexShaderAndInputLayout(L"Normal", g_normalVS, desc, g_basicInputLayout);
		D3DUtils::GetInst().CreateVertexShaderAndInputLayout(L"Copy", g_copyVS, desc, g_basicInputLayout);
		D3DUtils::GetInst().CreateVertexShaderAndInputLayout(L"DirArrow", g_dirArrowVS, desc, g_basicInputLayout);

		D3DUtils::GetInst().CreatePixelShader(L"Basic", g_basicPS);
		D3DUtils::GetInst().CreatePixelShader(L"SkyBox", g_skyboxPS);
		D3DUtils::GetInst().CreatePixelShader(L"Normal", g_normalPS);
		D3DUtils::GetInst().CreatePixelShader(L"Copy", g_copyPS);
		D3DUtils::GetInst().CreatePixelShader(L"Blur", g_blurPS);
		D3DUtils::GetInst().CreatePixelShader(L"Combine", g_combinePS);
		D3DUtils::GetInst().CreatePixelShader(L"DirArrow", g_dirArrowPS);
		
		D3DUtils::GetInst().CreateGeometryShader(L"Basic", g_basicGS);
		D3DUtils::GetInst().CreateGeometryShader(L"Normal", g_normalGS);
		D3DUtils::GetInst().CreateGeometryShader(L"DirArrow", g_dirArrowGS);
	}

	void InitRasterizerState()
	{
		D3D11_RASTERIZER_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.FillMode = D3D11_FILL_SOLID;	// WireFrame
		desc.CullMode = D3D11_CULL_BACK;	// None, Front
		desc.FrontCounterClockwise = false;
		desc.DepthClipEnable = true;
		desc.MultisampleEnable = true;
		D3DUtils::GetInst().CreateRasterizerState(&desc, g_solidCWRS);
		desc.FrontCounterClockwise = true;
		D3DUtils::GetInst().CreateRasterizerState(&desc, g_solidCCWRS);
		desc.FillMode = D3D11_FILL_WIREFRAME;
		D3DUtils::GetInst().CreateRasterizerState(&desc, g_wireCCWRS);
		desc.FrontCounterClockwise = false;
		D3DUtils::GetInst().CreateRasterizerState(&desc, g_wireCWRS);
		desc.FillMode = D3D11_FILL_SOLID;
		desc.CullMode = D3D11_CULL_MODE::D3D11_CULL_NONE;
		desc.DepthClipEnable = false;
		D3DUtils::GetInst().CreateRasterizerState(&desc, g_postProcessRS);
	}

	void InitDepthStencilState()
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
		D3DUtils::GetInst().CreateDepthStencilState(&desc, g_basicDSS);

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
		D3DUtils::GetInst().CreateDepthStencilState(&desc, g_maskDSS);

		{
			desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
			desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;	// 거울을 그자리에 다시 그려야 하기 때문
			desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
			desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
			desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
			desc.FrontFace.StencilFunc = D3D11_COMPARISON_EQUAL;	// OMSetDepthStencilState에서 설정한 ref값과 같은놈만 그림
		}
		D3DUtils::GetInst().CreateDepthStencilState(&desc, g_drawMaskDSS);
	}

	void InitBlendState()
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

		D3DUtils::GetInst().CreateBlendState(&desc, g_basicBS);
	}

	void InitGraphicsPSO()
	{
		// DefaultSolidPSO
		g_defaultSolidPSO.SetVS(g_basicVS);
		g_defaultSolidPSO.SetInputLayout(g_basicInputLayout);
		g_defaultSolidPSO.SetPS(g_basicPS);
		g_defaultSolidPSO.SetRS(g_solidCWRS);
		g_defaultSolidPSO.SetDSS(g_basicDSS);

		// DefaultWirePSO
		g_defaultWirePSO = g_defaultSolidPSO;
		g_defaultWirePSO.SetRS(g_wireCWRS);

		// SkyBoxSolidPSO
		g_skyBoxSolidPSO = g_defaultSolidPSO;
		g_skyBoxSolidPSO.SetVS(g_skyboxVS);
		g_skyBoxSolidPSO.SetPS(g_skyboxPS);

		// SkyBoxWirePSO
		g_skyBoxWirePSO = g_skyBoxSolidPSO;
		g_skyBoxWirePSO.SetRS(g_wireCWRS);

		// NormalPSO
		g_normalPSO = g_defaultSolidPSO;
		g_normalPSO.SetVS(g_normalVS);
		g_normalPSO.SetGS(g_normalGS);
		g_normalPSO.SetPS(g_normalPS);
		g_normalPSO.SetTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

		// StencilSolidMaskPSO
		g_stencilMaskSolidPSO = g_defaultSolidPSO;
		g_stencilMaskSolidPSO.SetDSS(g_maskDSS);

		// StencilWireMaskPSO
		g_stencilMaskWirePSO = g_stencilMaskSolidPSO;
		g_stencilMaskWirePSO.SetRS(g_wireCWRS);

		// DrawMaskSolidPSO
		g_drawMaskSolidPSO = g_defaultSolidPSO;
		g_drawMaskSolidPSO.SetRS(g_solidCCWRS);
		g_drawMaskSolidPSO.SetDSS(g_drawMaskDSS);

		// DrawMaskWirePSO
		g_drawMaskWirePSO = g_drawMaskSolidPSO;
		g_drawMaskWirePSO.SetRS(g_wireCCWRS);

		// DrawMaskSkyBoxSolidPSO
		g_drawMaskSkyBoxSolidPSO = g_drawMaskSolidPSO;
		g_drawMaskSkyBoxSolidPSO.SetVS(g_skyboxVS);
		g_drawMaskSkyBoxSolidPSO.SetPS(g_skyboxPS);

		// DrawMaskSkyBoxWirePSO
		g_drawMaskSkyBoxWirePSO = g_drawMaskSkyBoxSolidPSO;
		g_drawMaskSkyBoxWirePSO.SetRS(g_wireCCWRS);

		// BlendSolidPSO
		g_blendSolidPSO = g_drawMaskSolidPSO;
		g_blendSolidPSO.SetRS(g_solidCWRS);
		g_blendSolidPSO.SetBS(g_basicBS);

		// BlendWirePSO
		g_blendWirePSO = g_blendSolidPSO;
		g_blendWirePSO.SetRS(g_wireCWRS);

		// PostProcessPSO;
		g_postProcessPSO = g_defaultSolidPSO;
		g_postProcessPSO.SetVS(g_copyVS);
		g_postProcessPSO.SetRS(g_postProcessRS);
	}

}
