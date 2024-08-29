#include "pch.h"
#include "SpriteScene.h"
#include "KeyMgr.h"
#include "Core.h"

SpriteScene::SpriteScene(SceneMgr* pOwner)
	: AnimateScene(pOwner)
{
}

void SpriteScene::Update(float dt)
{
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

	for (auto& p : m_particle->GetVec())
	{
		if (p.lifeTime < 0)
			continue;
		p.velocity += m_gravity * dt;
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

void SpriteScene::DissipateDensity(ComPtr<ID3D11DeviceContext>& context)
{
	// 이건 그 전 시뮬레이션 코드
	context->CSSetShader(Graphics::g_densityCS.Get(), nullptr, 0);
	context->CSSetUnorderedAccessViews(0, 1, m_stagingBuffer->GetUAV().GetAddressOf(), nullptr);
	context->Dispatch(ceil(m_stagingBuffer->m_iWidth / 32), ceil(m_stagingBuffer->m_iHeight / 32), 1);

	this->ComputeShaderBarrier(context);
}

void SpriteScene::AdvectParticles(ComPtr<ID3D11DeviceContext>& context)
{
	Graphics::g_particlePSO.Setting();
	context->CSSetUnorderedAccessViews(0, 1, m_particle->GetAddressUAV(), nullptr);
	context->Dispatch(ceil(m_particle->GetBufferSize() / 256.0f), 1, 1);

	this->ComputeShaderBarrier(context);
}

void SpriteScene::DrawSprites(ComPtr<ID3D11DeviceContext>& context)
{
	Graphics::g_particlePSO.Setting();
	AnimateScene::DrawSprites(context);
}
void SpriteScene::Render(ComPtr<ID3D11DeviceContext>& context, bool drawWireFrame)
{
	float clearColor[4] = { 0.0f,0.0f,0.0f,1.0f };
	context->ClearRenderTargetView(m_stagingBuffer->GetRTV().Get(), clearColor);

	//this->DissipateDensity(context);

	//this->AdvectParticles(context);

	this->DrawSprites(context);

	context->CopyResource(m_pOwner->GetBackBufferTexture().Get(), m_stagingBuffer->GetTexture().Get());
}
