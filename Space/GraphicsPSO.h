#pragma once
#include "GraphicsCommons.h"
class GraphicsPSO
{
public:
	GraphicsPSO& operator = (const GraphicsPSO& other);
	void Setting();

public:
	void SetVS(ComPtr<ID3D11VertexShader>& vs) { m_vertexShader = vs; }
	void SetGS(ComPtr<ID3D11GeometryShader>& gs) { m_geometryShader = gs; }
	void SetPS(ComPtr<ID3D11PixelShader>& ps) { m_pixelShader = ps; }
	void SetInputLayout(ComPtr<ID3D11InputLayout>& inputLayout) { m_inputLayout = inputLayout; }
	void SetRS(ComPtr<ID3D11RasterizerState>& rs) { m_rasterizerState = rs; }
	void SetDSS(ComPtr<ID3D11DepthStencilState>& dss) { m_depthStencilState = dss; }
	void SetBS(ComPtr<ID3D11BlendState>& bs) { m_blendState = bs; }
	void SetBlendFactor(const float blendFactor[4]);
	void SetStencilRef(UINT ref) { m_stencilRef = ref; }
	void SetTopology(D3D11_PRIMITIVE_TOPOLOGY topology) { m_primitiveTopology = topology; }

public:
	ComPtr<ID3D11VertexShader>& GetVS() { return m_vertexShader; }
	ComPtr<ID3D11GeometryShader>& GetGS() { return m_geometryShader; }
	ComPtr<ID3D11PixelShader>& GetPS() { return m_pixelShader; }
	ComPtr<ID3D11InputLayout>& GetInputLayout() { return m_inputLayout; }
	ComPtr<ID3D11RasterizerState>& GetRS() { return m_rasterizerState; }
	ComPtr<ID3D11DepthStencilState>& GetDSS() { return m_depthStencilState; }
	ComPtr<ID3D11BlendState>& GetBS() { return m_blendState; }

private:
	ComPtr<ID3D11VertexShader> m_vertexShader;
	ComPtr<ID3D11GeometryShader> m_geometryShader;
	ComPtr<ID3D11PixelShader> m_pixelShader;
	ComPtr<ID3D11InputLayout> m_inputLayout;
	ComPtr<ID3D11RasterizerState> m_rasterizerState;
	ComPtr<ID3D11DepthStencilState> m_depthStencilState;
	ComPtr<ID3D11BlendState> m_blendState;

	float m_blendFactor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	UINT m_stencilRef = 0;

	D3D11_PRIMITIVE_TOPOLOGY m_primitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
};

