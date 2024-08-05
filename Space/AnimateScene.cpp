#include "pch.h"
#include "AnimateScene.h"
#include "SceneMgr.h"
#include "KeyMgr.h"
#include "Core.h"

AnimateScene::AnimateScene(SceneMgr* owner)
	: Scene(owner)
{
}

void AnimateScene::Enter()
{
	m_particle = make_shared<StructuredBuffer<Particle>>();

	m_stagingBuffer = make_shared<Texture2D>();
	m_stagingBuffer->Init(UINT(m_pOwner->m_fWidth), UINT(m_pOwner->m_fHeight),
		DXGI_FORMAT_R16G16B16A16_FLOAT);

	random_device rd;
	mt19937 gen(rd());
	uniform_int_distribution<uint32_t> randomColor(0, 6);
	vector<Vector3> rainbow =
	{
		{1.0f, 0.0f, 0.0f},  // Red
		{1.0f, 0.65f, 0.0f}, // Orange
		{1.0f, 1.0f, 0.0f},  // Yellow
		{0.0f, 1.0f, 0.0f},  // Green
		{0.0f, 0.0f, 1.0f},  // Blue
		{0.3f, 0.0f, 0.5f},  // Indigo
		{0.5f, 0.0f, 1.0f}   // Violet/Purple
	};

	auto& vec = m_particle->GetVec();
	vec.resize(2048);

	for (auto& p : vec)
		p.color = rainbow[randomColor(gen)];

	m_particle->Init();

	this->CreateIndirectArgsBuffer(m_particle->GetBufferSize());
}


void AnimateScene::CreateIndirectArgsBuffer(UINT count)
{
	m_indirectArgsBuffer = make_shared<IndirectArgsBuffer<IndirectArgs>>();
	m_indirectArgsBuffer->GetVec() =
	{
		{count, 1, 0, 0}
	};
	m_indirectArgsBuffer->Init();
}

void AnimateScene::ComputeShaderBarrier(ComPtr<ID3D11DeviceContext>& context)
{
	ID3D11ShaderResourceView* arrSRV[6] =
	{ nullptr };
	context->CSSetShaderResources(0, 6, arrSRV);
	ID3D11UnorderedAccessView* arrUAV[6] =
	{ nullptr };
	context->CSSetUnorderedAccessViews(0, 6, arrUAV, nullptr);
}



void AnimateScene::DrawSprites(ComPtr<ID3D11DeviceContext>& context)
{
	Graphics::g_particlePSO.Setting();
	context->RSSetViewports(1, &m_pOwner->GetViewPort());
	context->OMSetRenderTargets(1, m_stagingBuffer->GetRTV().GetAddressOf(), nullptr);

	context->VSSetShaderResources(0, 1, m_particle->GetAddressSRV());

	const float factor = 1.0f;
	const float blendFactor[4] = { factor ,factor ,factor ,factor };
	GraphicsPSO::SetBlendFactor(blendFactor);

	//context->Draw(m_particle->GetBufferSize(), 0);
	context->DrawInstancedIndirect(m_indirectArgsBuffer->GetBuffer().Get(), sizeof(IndirectArgs) * 0);
}

