#pragma once
class GraphicsPSO;
namespace Graphics
{
	// Samplers
	extern ComPtr<ID3D11SamplerState> g_linearSampler;
	extern ComPtr<ID3D11SamplerState> g_clampSampler;
	extern vector<ID3D11SamplerState*> g_vecSamplers;

	// VertexShader
	extern ComPtr<ID3D11VertexShader> g_basicVS;
	extern ComPtr<ID3D11VertexShader> g_skyboxVS;
	extern ComPtr<ID3D11VertexShader> g_normalVS;
	extern ComPtr<ID3D11VertexShader> g_copyVS;
	extern ComPtr<ID3D11VertexShader> g_dirArrowVS;

	// InputLayout
	extern ComPtr<ID3D11InputLayout> g_basicInputLayout;

	// GeometryShader
	extern ComPtr<ID3D11GeometryShader> g_basicGS;
	extern ComPtr<ID3D11GeometryShader> g_normalGS;
	extern ComPtr<ID3D11GeometryShader> g_dirArrowGS;

	// PixelShader
	extern ComPtr<ID3D11PixelShader> g_basicPS;
	extern ComPtr<ID3D11PixelShader> g_skyboxPS;
	extern ComPtr<ID3D11PixelShader> g_normalPS;
	extern ComPtr<ID3D11PixelShader> g_copyPS;
	extern ComPtr<ID3D11PixelShader> g_blurPS;
	extern ComPtr<ID3D11PixelShader> g_combinePS;
	extern ComPtr<ID3D11PixelShader> g_postEffectsPS;
	extern ComPtr<ID3D11PixelShader> g_dirArrowPS;

	// RasterizerState
	extern ComPtr<ID3D11RasterizerState> g_solidCWRS;
	extern ComPtr<ID3D11RasterizerState> g_solidCCWRS;
	extern ComPtr<ID3D11RasterizerState> g_wireCWRS;
	extern ComPtr<ID3D11RasterizerState> g_wireCWRS;
	extern ComPtr<ID3D11RasterizerState> g_postProcessRS;

	// DepthStencilState
	extern ComPtr<ID3D11DepthStencilState> g_basicDSS;
	extern ComPtr<ID3D11DepthStencilState> g_maskDSS;
	extern ComPtr<ID3D11DepthStencilState> g_drawMaskDSS;

	// BlendState
	extern ComPtr<ID3D11BlendState> g_basicBS;

	// PSO
	extern GraphicsPSO g_defaultSolidPSO;
	extern GraphicsPSO g_defaultWirePSO;
	extern GraphicsPSO g_skyBoxSolidPSO;
	extern GraphicsPSO g_skyBoxWirePSO;
	extern GraphicsPSO g_normalPSO;
	extern GraphicsPSO g_stencilMaskSolidPSO;
	extern GraphicsPSO g_stencilMaskWirePSO;
	extern GraphicsPSO g_drawMaskSolidPSO;
	extern GraphicsPSO g_drawMaskWirePSO;
	extern GraphicsPSO g_drawMaskSkyBoxSolidPSO;
	extern GraphicsPSO g_drawMaskSkyBoxWirePSO;
	extern GraphicsPSO g_blendSolidPSO;
	extern GraphicsPSO g_blendWirePSO;
	extern GraphicsPSO g_postEffectsPSO;
	extern GraphicsPSO g_postProcessPSO;
	
	void InitCommonStates();
	void InitSamplers();
	void InitShaders();
	void InitRasterizerState();
	void InitDepthStencilState();
	void InitBlendState();
	void InitGraphicsPSO();
}
