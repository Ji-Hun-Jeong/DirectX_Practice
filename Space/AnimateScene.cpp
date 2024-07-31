#include "pch.h"
#include "AnimateScene.h"
#include "SceneMgr.h"
#include "GraphicsCommons.h"
#include "GraphicsPSO.h"
#include "StructuredBuffer.h"
#include "IndirectArgsBuffer.h"
#include "Texture2D.h"
#include "KeyMgr.h"
#include "Core.h"

AnimateScene::AnimateScene(SceneMgr* owner)
	: Scene(owner)
{
}

void AnimateScene::Enter()
{
	srand(time(NULL));
	D3DUtils::GetInst().CreateConstantBuffer<AnimateConstData>(m_constData, m_constBuffer);
	m_particle = make_shared<StructuredBuffer<Particle>>();

	m_stagingBuffer = make_shared<Texture2D>();
	m_stagingBuffer->Init(UINT(m_pOwner->m_fWidth), UINT(m_pOwner->m_fHeight),
		DXGI_FORMAT_R16G16B16A16_FLOAT);

	random_device rd;
	mt19937 gen(rd());
	uniform_real_distribution<float> randomColor(0.0f, 6.0f);
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
	vec.resize(2560);

	for (auto& p : vec)
		p.color = rainbow[UINT(randomColor(gen))];

	m_particle->Init();
	this->CreateIndirectArgsBuffer(m_particle->GetBufferSize());
}

void AnimateScene::Update(float dt)
{
	//m_constData.mousePos = KeyMgr::GetInst().GetMouseNDCPos();
	//m_constData.dt = dt * 0.5f;
	//D3DUtils::GetInst().UpdateBuffer(m_constData, m_constBuffer);

	dt *= 0.5f;
	random_device rd;
	mt19937 gen(rd());
	uniform_real_distribution<float> randomTheta(-3.141592f, 3.141592f);
	uniform_real_distribution<float> randomSpeed(1.5f, 2.0f);
	uniform_real_distribution<float> randomLife(0.0f, 1.0f);
	uniform_real_distribution<float> randomX(-1.0f, 1.0f);

	auto& vec = m_particle->GetVec();
	int newCount = 50;
	if (KEYCHECK(LBUTTON, HOLD))
	{
		Vector2 mousePos = KeyMgr::GetInst().GetMouseNDCPos();
		for (auto& p : vec)
		{
			if (newCount <= 0)
				break;
			if (p.lifeTime < 0 && newCount > 0)
			{
				const float theta = randomTheta(gen);
				p.pos = Vector3(cos(theta), sin(theta), 0.0f) * 0.1f +
					Vector3(mousePos.x, mousePos.y, 0.0f);
				p.velocity = Vector3(cos(theta), sin(theta), 0.0f) * randomSpeed(gen);
				p.lifeTime = randomLife(gen) * 1.5f;
				p.radius = cos(randomTheta(gen)) * 0.05f;
				--newCount;
			}
		}
	}

	newCount = 10;
	for (auto& p : vec)
	{
		if (newCount <= 0)
			break;
		if (p.lifeTime < 0 && newCount > 0)
		{
			p.pos = Vector3(cos(randomTheta(gen)), sin(randomTheta(gen)), 0.0f) * 0.1f;
			p.velocity = Vector3(-1.0f, 0.0f, 0.0f) * randomSpeed(gen);
			p.lifeTime = randomLife(gen) * 1.5f;
			p.radius = cos(randomTheta(gen)) * 0.05f;
			--newCount;
		}
	}

	const float aspect = Core::GetInst().GetAspect();
	const float ground = -0.8f;
	const float cor = 0.5f;
	const Vector3 gravity = Vector3(0.0f, -9.8f, 0.0f);
	for (auto& p : vec)
	{
		if (p.lifeTime < 0)
			continue;
		p.velocity += gravity * dt;
		p.pos += p.velocity * dt;
		p.lifeTime -= dt;
		if (p.pos.y < ground && p.velocity.y < 0.0f)
		{
			p.pos.y = ground + 1e-5;
			p.velocity.y = -p.velocity.y * cor * randomSpeed(gen);
		}
		else if (p.pos.x < -1.0f && p.velocity.x < 0.0f)
		{
			p.pos.x = -1.0f + 1e-5;
			p.velocity.x = -p.velocity.x * cor * randomSpeed(gen);
		}
		else if (p.pos.x > 1.0f && p.velocity.x > 0.0f)
		{
			p.lifeTime = -1.0f;
		}
	}
	m_particle->UploadToBuffer();
}

void AnimateScene::Render(ComPtr<ID3D11DeviceContext>& context, bool drawWireFrame)
{
	float clearColor[4] = { 0.0f,0.0f,0.0f,1.0f };
	context->ClearRenderTargetView(m_stagingBuffer->GetRTV().Get(), clearColor);

	//this->DissipateDensity(context);

	//this->AdvectParticles(context);

	this->DrawSprites(context);

	context->CopyResource(m_pOwner->GetBackBufferTexture().Get(), m_stagingBuffer->GetTexture().Get());
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
	context->CSSetConstantBuffers(0, 1, m_constBuffer.GetAddressOf());
	context->CSSetUnorderedAccessViews(0, 1, m_particle->GetAddressUAV(), nullptr);
	context->Dispatch(ceil(m_particle->GetBufferSize() / 256.0f), 1, 1);

	this->ComputeShaderBarrier(context);
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
