#include "pch.h"
#include "SPHScene.h"
#include "SPHKernels.h"
#include "Texture2D.h"

SPHScene::SPHScene(SceneMgr* pOwner)
	: AnimateScene(pOwner)
	, m_radius(1.0f / 16.0f)
	, m_mass(1.0f)
	, m_pressureCoeff(1.0f)
	, m_density0(1.0f)
	, m_viscosity(0.1f)
{
}

void SPHScene::Update(float dt)
{
	dt = 1.0f / 60.0f * 0.25f;

	random_device rd;
	mt19937 gen(rd());
	uniform_real_distribution<float> randomTheta(-3.141592f, 3.141592f);
	uniform_real_distribution<float> randomSpeed(1.5f, 2.0f);
	uniform_real_distribution<float> randomLife(0.0f, 1.0f);
	uniform_real_distribution<float> randomX(-1.0f, 1.0f);

	UINT newCount = 1;
	auto& vec = m_particle->GetVec();
	for (auto& p : vec)
	{
		if (newCount == 0)
			break;
		if (p.lifeTime > 0.0f)
			continue;
		float theta = randomTheta(gen);
		p.pos = Vector3(sin(theta), cos(theta), 0.0f) * 0.1f + Vector3(-0.3f, 0.3f, 0.0f);
		p.lifeTime = randomLife(gen) * 1.5f;
		p.velocity = Vector3(1.0f, 0.0f, 0.0f);
		p.radius = m_radius;
		p.size = m_radius;
		--newCount;
	}

	newCount = 1;

	for (auto& p : vec)
	{
		if (newCount == 0)
			break;
		if (p.lifeTime > 0.0f)
			continue;
		float theta = randomTheta(gen);
		p.pos = Vector3(sin(theta), cos(theta), 0.0f) * 0.1f + Vector3(0.3f, 0.3f, 0.0f);
		p.lifeTime = randomLife(gen) * 1.5f;
		p.velocity = Vector3(-1.0f, 0.0f, 0.0f);
		p.radius = m_radius;
		p.size = m_radius;
		--newCount;
	}

	m_particle->UploadToBuffer();

	auto& context = D3DUtils::GetInst().GetContext();
//	Graphics::g_computeDensityPSO.Setting();
//	
//#pragma omp parallel for
//	for (int i = 0; i < vec.size(); ++i)
//	{
//		if (vec[i].lifeTime < 0.0f)
//			continue;
//
//		vec[i].density = 0.0f;
//		vector<float> density(vec.size(), vec[i].density);
//		m_density.Upload<float>(density);
//
//		m_orderConst.i = i;
//		D3DUtils::GetInst().UpdateBuffer<OrderConst>(m_orderConst, m_orderBuffer);
//
//		vector<ID3D11UnorderedAccessView*> uav =
//		{
//			m_density.GetUAV().Get(),
//			m_particle->GetUAV()
//		};
//		context->CSSetUnorderedAccessViews(0, uav.size(), uav.data(), nullptr);
//		context->CSSetConstantBuffers(0, 1, m_orderBuffer.GetAddressOf());
//		context->Dispatch(ceil(vec.size() / 1024.0f), 1, 1);
//		ComputeShaderBarrier(context);
//
//		m_density.Download<float>(density);
//		for (auto& f : density)
//			vec[i].density += f;
//		
//		vec[i].pressure = m_pressureCoeff *
//			(pow(vec[i].density / m_density0, 7.0f) - 1.0f);
//	}
#pragma omp parallel for
	for (int i = 0; i < vec.size(); ++i)
	{
		if (vec[i].lifeTime < 0.0f)
			continue;

		vec[i].density = 0.0f;
		for (int j = 0; j < vec.size(); ++j)
		{
			if (vec[j].lifeTime < 0.0f)
				continue;

			const float dist =
				(vec[i].pos - vec[j].pos)
				.Length();

			if (dist >= m_radius)
				continue;

			vec[i].density +=
				m_mass * SPHKernels::CubicSpline(dist / m_radius);
		}

		vec[i].pressure = m_pressureCoeff *
			(pow(vec[i].density / m_density0, 7.0f) - 1.0f);
	}

#pragma omp parallel for
	for (int i = 0; i < vec.size(); ++i)
	{
		if (vec[i].lifeTime < 0.0f)
			continue;

		Vector3 pressureForce(0.0f);
		Vector3 viscosity(0.0f);

		const float densityI = vec[i].density;
		const float pressureI = vec[i].pressure;
		const Vector3 velocityI = vec[i].velocity;
		const float h = vec[i].size;
		for (int j = 0; j < vec.size(); ++j)
		{
			if (vec[j].lifeTime < 0.0f)
				continue;

			const Vector3 x_ij = vec[i].pos - vec[j].pos;
			const float dist = x_ij.Length();

			if (dist >= m_radius)
				continue;

			if (dist < 1e-3f) // 수치 오류 방지
				continue;

			const float densityJ = vec[j].density;
			const float pressureJ = vec[j].pressure;
			const Vector3 velocityJ = vec[j].velocity;
			const float csg = SPHKernels::CubicSplineGrad(dist / m_radius);
			const Vector3 w_ij = csg * (x_ij / dist);

			const Vector3 gradPressure = densityI * m_mass *
				(pressureI / pow(densityI, 2.0f)
					+ pressureJ / pow(densityJ, 2.0f)) * w_ij;

			const Vector3 laplacianVelocity = (2.0f * m_mass *
				(velocityI - velocityJ) / densityJ)
				* (x_ij.Dot(w_ij) / (x_ij.Dot(x_ij) + 0.01f * h * h));

			pressureForce += gradPressure;
			viscosity += laplacianVelocity;
		}

		pressureForce *= -m_mass / densityI;
		viscosity *= m_mass * m_viscosity;

		vec[i].force = pressureForce + viscosity;
	}

	const float ground = -0.8f;
	const float left = -0.8f;
	const float right = 0.8f;
	const float cor = 0.5f;

	for (auto& p : vec)
	{
		if (p.lifeTime <= 0.0f)
			continue;
		p.velocity += p.force * dt;
		p.velocity += m_gravity * dt;
		p.pos += p.velocity * dt;
		if (p.pos.y < ground && p.velocity.y < 0.0f)
		{
			p.pos.y = ground;
			p.velocity.y = -p.velocity.y * cor;
		}
		if (p.pos.x < left && p.velocity.x < 0.0f)
		{
			p.pos.x = left;
			p.velocity.x = -p.velocity.x * cor;
		}
		if (p.pos.x > right && p.velocity.x > 0.0f)
		{
			p.pos.x = right;
			p.velocity.x = -p.velocity.x * cor;
		}
	}

	m_particle->UploadToBuffer();
}

void SPHScene::Render(ComPtr<ID3D11DeviceContext>& context, bool drawWireFrame)
{
	float clearColor[4] = { 0.0f,0.0f,0.0f,1.0f };
	context->ClearRenderTargetView(m_stagingBuffer->GetRTV().Get(), clearColor);

	//this->DissipateDensity(context);

	//this->AdvectParticles(context);

	this->DrawSprites(context);

	context->CopyResource(m_pOwner->GetBackBufferTexture().Get(), m_stagingBuffer->GetTexture().Get());
}

void SPHScene::Enter()
{
	AnimateScene::Enter();
	m_density.Init(m_particle->GetBufferSize(), 1, DXGI_FORMAT_R32_FLOAT);
	
	D3DUtils::GetInst().CreateConstantBuffer<OrderConst>(m_orderConst, m_orderBuffer);
}

void SPHScene::DrawSprites(ComPtr<ID3D11DeviceContext>& context)
{
	Graphics::g_sphPSO.Setting();
	AnimateScene::DrawSprites(context);
}
