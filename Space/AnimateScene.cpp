#include "pch.h"
#include "AnimateScene.h"
#include "SceneMgr.h"
#include "GraphicsCommons.h"
#include "GraphicsPSO.h"
#include "StructuredBuffer.h"
#include "Texture2D.h"

AnimateScene::AnimateScene(SceneMgr* owner)
	: Scene(owner)
{
}

void AnimateScene::Enter()
{
	srand(time(NULL));
	m_particle = make_shared<StructuredBuffer<Particle>>();
	m_stagingBuffer = make_shared<Texture2D>();
	m_stagingBuffer->Init(UINT(m_pOwner->m_fWidth), UINT(m_pOwner->m_fHeight),
		DXGI_FORMAT_R16G16B16A16_FLOAT);

	vector<Particle> vec;
	vec.resize(256);

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

	for (int i = 0; i < vec.size(); ++i)
	{
		float x = (rand() % 10001 - 5000) / 5000.0f;
		float y = (rand() % 10001 - 5000) / 5000.0f;
		vec[i].pos = Vector3(x, y, 1.0f);
		int idx = rand() % 6;
		vec[i].color = rainbow[idx];
	}
	m_particle->Init(vec);
}

void AnimateScene::Exit()
{
}

void AnimateScene::Update(float dt)
{
}

void AnimateScene::Render(ComPtr<ID3D11DeviceContext>& context, bool drawWireFrame)
{
	float clearColor[4] = { 0.0f,0.0f,0.0f,1.0f };
	// context->ClearRenderTargetView(m_stagingBuffer->GetRTV().Get(), clearColor);

	this->DissipateDensity(context);

	this->AdvectParticles(context);
	
	this->DrawSprites(context);

	context->CopyResource(m_pOwner->GetBackBufferTexture().Get(), m_stagingBuffer->GetTexture().Get());
}

void AnimateScene::UpdateGUI()
{
}

void AnimateScene::InitMesh()
{
}

void AnimateScene::InitIBL()
{
}

void AnimateScene::InitSkyBox()
{
}

void AnimateScene::ComputeShaderBarrier(ComPtr<ID3D11DeviceContext>& context)
{
	ID3D11ShaderResourceView* arrSRV[] =
	{ nullptr };
	context->CSSetShaderResources(0, 1, arrSRV);
	ID3D11UnorderedAccessView* arrUAV[] =
	{ nullptr };
	context->CSSetUnorderedAccessViews(0, 1, arrUAV, nullptr);
}

void AnimateScene::DissipateDensity(ComPtr<ID3D11DeviceContext>& context)
{
	context->CSSetShader(Graphics::g_densityCS.Get(), nullptr, 0);
	context->CSSetUnorderedAccessViews(0, 1, m_stagingBuffer->GetUAV().GetAddressOf(), nullptr);
	context->Dispatch(ceil(m_stagingBuffer->m_iWidth / 32), ceil(m_stagingBuffer->m_iHeight / 32), 1);

	this->ComputeShaderBarrier(context);
}

void AnimateScene::AdvectParticles(ComPtr<ID3D11DeviceContext>& context)
{
	Graphics::g_particlePSO.Setting();
	context->CSSetUnorderedAccessViews(0, 1, m_particle->GetAddressUAV(), nullptr);
	context->Dispatch(ceil(m_particle->GetBufferSize() / 256.0f), 1, 1);

	this->ComputeShaderBarrier(context);
}

void AnimateScene::DrawSprites(ComPtr<ID3D11DeviceContext>& context)
{
	context->RSSetViewports(1, &m_pOwner->GetViewPort());
	context->OMSetRenderTargets(1, m_stagingBuffer->GetRTV().GetAddressOf(), nullptr);

	context->VSSetShaderResources(0, 1, m_particle->GetAddressSRV());

	const float factor = 1.0f;
	const float blendFactor[4] = { factor ,factor ,factor ,factor };
	GraphicsPSO::SetBlendFactor(blendFactor);

	context->Draw(m_particle->GetBufferSize(), 0);
}
