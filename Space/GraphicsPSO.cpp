#include "pch.h"
#include "GraphicsPSO.h"
#include "D3DUtils.h"

GraphicsPSO& GraphicsPSO::operator=(const GraphicsPSO& other)
{
	m_vertexShader = other.m_vertexShader;
	m_geometryShader = other.m_geometryShader;
	m_pixelShader = other.m_pixelShader;
	m_inputLayout = other.m_inputLayout;
	m_rasterizerState = other.m_rasterizerState;
	m_depthStencilState = other.m_depthStencilState;
	m_blendState = other.m_blendState;

	SetBlendFactor(other.m_blendFactor);
	m_stencilRef = other.m_stencilRef;
	m_primitiveTopology = other.m_primitiveTopology;

	return *this;
}

void GraphicsPSO::Setting()
{
	ComPtr<ID3D11DeviceContext>& context = D3DUtils::GetInst().GetContext();
	context->IASetInputLayout(m_inputLayout.Get());
	context->IASetPrimitiveTopology(m_primitiveTopology);
	context->VSSetShader(m_vertexShader.Get(),0,0);
	context->GSSetShader(m_geometryShader.Get(), 0, 0);
	context->PSSetShader(m_pixelShader.Get(), 0, 0);
	context->RSSetState(m_rasterizerState.Get());
	context->OMSetDepthStencilState(m_depthStencilState.Get(), m_stencilRef);
	context->OMSetBlendState(m_blendState.Get(), m_blendFactor, 0xff);
}

void GraphicsPSO::SetBlendFactor(const float blendFactor[4])
{
	memcpy(m_blendFactor, blendFactor, sizeof(float) * 4);
}
