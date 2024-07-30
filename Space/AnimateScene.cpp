#include "pch.h"
#include "AnimateScene.h"
#include "SceneMgr.h"
#include "GraphicsCommons.h"
#include "GraphicsPSO.h"
#include "StructuredBuffer.h"

AnimateScene::AnimateScene(SceneMgr* owner)
	: Scene(owner)
{
}

void AnimateScene::Enter()
{
	srand(time(NULL));
	m_particle = make_shared<StructuredBuffer<Particle>>();
	vector<Particle> vec;
	vec.resize(25600);

	for (int i = 0; i < vec.size(); ++i)
	{
		float x = (rand() % 101 - 50) / 50.0f;
		float y = (rand() % 101 - 50) / 50.0f;
		vec[i].pos = Vector3(x, y, 1.0f);
		float r = rand() % 21 / 100.0f;//+ 0.8f;
		float g = rand() % 21 / 100.0f;//+ 0.8f;
		float b = rand() % 21 / 100.0f;//+ 0.8f;
		vec[i].color = Vector3(r, g, b);
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
	context->ClearRenderTargetView(m_pOwner->GetBackBufferRTV().Get(), clearColor);

	Graphics::g_basicComputePSO.Setting();

	ID3D11UnorderedAccessView* arr[] =
	{
		m_pOwner->GetBackBufferUAV().Get(),
		m_particle->GetUAV().Get()
	};
	context->CSSetUnorderedAccessViews(0, 2, arr, nullptr);
	context->Dispatch(ceil(m_pOwner->m_fWidth / 32.0f), ceil(m_pOwner->m_fHeight / 32.0f), 1);
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
