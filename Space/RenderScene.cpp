#include "pch.h"
#include "RenderScene.h"
#include "GeometryGenerator.h"
#include "Mesh.h"
#include "Camera.h"
#include "Scene.h"
#include "SceneMgr.h"
#include "ModelLoader.h"
#include "Mirror.h"
#include "D3DUtils.h"
#include "GraphicsCommons.h"
#include "Light.h"
#include "GraphicsCommons.h"
#include "GraphicsPSO.h"
#include "KeyMgr.h"
#include "PostProcess.h"
#include "Texture2D.h"

RenderScene::RenderScene(SceneMgr* owner)
	: Scene(owner)
	, m_msaaTexture(make_shared<Texture2D>())
	, m_notMsaaTexture(make_shared<Texture2D>())
	, m_postEffectsTexture(make_shared<Texture2D>())
{
}

void RenderScene::Init()
{
	CreateRenderBuffer();
	D3DUtils::GetInst().CreateDepthOnlyResources(m_pOwner->m_fWidth, m_pOwner->m_fHeight, m_depthOnlyBuffer, m_depthOnlyDSV, m_depthOnlySRV);
	D3DUtils::GetInst().CreateConstantBuffer<GlobalConstData>(m_globalCD, m_globalConstBuffer);
	Scene::Init();

	m_postProcess = make_shared<PostProcess>(m_pOwner->m_fWidth, m_pOwner->m_fHeight, 2, m_postEffectsTexture->GetSRV(), m_pOwner->GetBackBufferRTV());
	MeshData squareData = GeometryGenerator::MakeSquare();
	m_postEffects = make_shared<Mesh>();
	m_postEffects->Init(squareData);
}

void RenderScene::Update(float dt)
{
	m_camera->Update(dt);

	UpdateGlobalCD();

	CalcPickingObject();

	for (auto& mirror : m_vecMirrors)
		mirror->Update(dt);

	for (auto& mesh : m_vecMesh)
		mesh->Update(dt);

	m_skybox->Update(dt);

	m_globalCD.view = m_globalCD.view.Transpose();
	m_globalCD.proj = m_globalCD.proj.Transpose();
	m_globalCD.viewProj = m_globalCD.viewProj.Transpose();
	m_globalCD.invProj = m_globalCD.invProj.Transpose();
	D3DUtils::GetInst().UpdateBuffer<GlobalConstData>(m_globalCD, m_globalConstBuffer);

	m_postEffects->Update(dt);
}

void RenderScene::Enter()
{
	ComPtr<ID3D11DeviceContext>& context = D3DUtils::GetInst().GetContext();
	context->VSSetSamplers(0, UINT(Graphics::g_vecSamplers.size()), Graphics::g_vecSamplers.data());
	context->GSSetSamplers(0, UINT(Graphics::g_vecSamplers.size()), Graphics::g_vecSamplers.data());
	context->PSSetSamplers(0, UINT(Graphics::g_vecSamplers.size()), Graphics::g_vecSamplers.data());

	context->VSSetConstantBuffers(0, 1, m_globalConstBuffer.GetAddressOf());
	context->GSSetConstantBuffers(0, 1, m_globalConstBuffer.GetAddressOf());
	context->PSSetConstantBuffers(0, 1, m_globalConstBuffer.GetAddressOf());
	context->PSSetShaderResources(10, (UINT)IBL_TYPE::END, m_iblSRV[(UINT)IBL_TYPE::SPECULAR].GetAddressOf());

	GETCAMERA()->SetPos(Vector3(0.0f, 1.0f, -1.0f));
	GETCAMERA()->SetSpeed(5.0f);
}

void RenderScene::Exit()
{
}

void RenderScene::InitIBL()
{
	ReadCubeImage("Image/PBR/SkyBox/SampleSpecularHDR.dds", IBL_TYPE::SPECULAR);
	ReadCubeImage("Image/PBR/SkyBox/SampleDiffuseHDR.dds", IBL_TYPE::IRRADIANCE);
	ReadCubeImage("Image/PBR/SkyBox/SampleBrdf.dds", IBL_TYPE::LUT);

}

void RenderScene::InitMesh()
{
	/*ModelLoader::GetInst().Load("Image/Character/Sample/", "angel_armor.fbx");
	auto& obj = ModelLoader::GetInst().resultMesh;
	obj->ReadImage("Image/Character/Sample/angel_armor_albedo.jpg", TEXTURE_TYPE::ALBEDO, true);
	obj->ReadImage("Image/Character/Sample/angel_armor_metalness.jpg", TEXTURE_TYPE::METAL);
	obj->ReadImage("Image/Character/Sample/angel_armor_normal.jpg", TEXTURE_TYPE::NORMAL);
	obj->ReadImage("Image/Character/Sample/angel_armor_e.jpg", TEXTURE_TYPE::EMISSIVE);
	obj->ReadImage("Image/Character/Sample/angel_armor_roughness.jpg", TEXTURE_TYPE::ROUGHNESS);
	m_vecMesh.push_back(obj);*/

	MeshData md = GeometryGenerator::MakeSphere(1.0f, 30, 30);
	auto light = make_shared<Light>();
	light->ReadImage("image/earth.jpg", TEXTURE_TYPE::ALBEDO, true);
	light->Init(md);
	m_vecMesh.push_back(light);
	m_vecLights.push_back(light);

	/*auto l = make_shared<Mesh>();
	l->Init(md);
	l->m_translation = Vector3(0.0f, 2.0f, 4.0f);
	m_vecMesh.push_back(l);*/

	MeshData sq = GeometryGenerator::MakeSquare();
	/*auto ground = make_shared<Mesh>();
	ground->Init(sq);
	ground->ReadImage("Image/Ground.png", TEXTURE_TYPE::ALBEDO, true);
	ground->m_rotation1 = Vector3(XM_PI / 2.0f, 0.0f, 0.0f);
	ground->m_scale = Vector3(5.0f);
	m_vecMesh.push_back(ground);*/


	auto mirror = make_shared<Mirror>("Mirror", Vector3(2.0f, 1.6f, 0.0f), Vector3(0.0f, 90.0f, 0.0f), Vector3(0.0f), Vector3(1.0f, 1.66f, 1.0f));
	mirror->Init(sq);
	m_vecMirrors.push_back(mirror);

	mirror = make_shared<Mirror>("Mirror2", Vector3(0.0f, 1.6f, 2.0f), Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f), Vector3(1.0f, 1.66f, 1.0f));
	mirror->Init(sq);
	m_vecMirrors.push_back(mirror);

	m_focusObj = light;

}

void RenderScene::InitSkyBox()
{
	MeshData sphereData = GeometryGenerator::MakeSphere(30.0f, 100, 100);
	std::reverse(sphereData.indices.begin(), sphereData.indices.end());
	m_skybox = make_shared<Mesh>();
	m_skybox->Init(sphereData);
}

void RenderScene::UpdateGUI()
{
	ImGui::Checkbox("DrawWireFrame", &SceneMgr::GetInst().m_drawWireFrame);
	ImGui::RadioButton("Render", &m_commonCD.mode, 0);
	ImGui::SameLine();
	ImGui::RadioButton("Depth", &m_commonCD.mode, 1);
	ImGui::SliderFloat("Depth Strength", &m_commonCD.depthStrength, 0.0f, 1.0f);
	ImGui::SliderFloat("Fog Strength", &m_commonCD.fogStrength, 0.0f, 1.0f);
	ImGui::CheckboxFlags("Use Albedo", &m_materialCD.useAlbedo, 1);
	ImGui::SliderFloat3("AlbedoFactor", &m_materialCD.albedoFactor.x, 0.0f, 1.0f);
	ImGui::Checkbox("DrawNormal", &m_drawNormal);
	ImGui::CheckboxFlags("Use Normal", &m_commonCD.useNormal, 1);
	ImGui::SliderFloat("NormalSize", &m_commonCD.normalSize, 0.0f, 2.0f);
	ImGui::CheckboxFlags("Use AO", &m_materialCD.useAO, 1);
	ImGui::CheckboxFlags("Use Emissive", &m_materialCD.useEmissive, 1);
	ImGui::SliderFloat3("EmissionFactor", &m_materialCD.emissionFactor.x, 0.0f, 1.0f);
	ImGui::CheckboxFlags("Use Height", &m_commonCD.useHeight, 1);
	ImGui::SliderFloat("Height Scale", &m_commonCD.heightScale, 0.0f, 2.0f);

	ImGui::CheckboxFlags("Use Metallic", &m_materialCD.useMetallic, 1);
	ImGui::SliderFloat("Metallic", &m_materialCD.metallicFactor, 0.0f, 1.0f);
	ImGui::CheckboxFlags("Use Roughness", &m_materialCD.useRoughness, 1);
	ImGui::SliderFloat("Roughness", &m_materialCD.roughnessFactor, 0.0f, 1.0f);

	ImGui::SliderFloat("Alpha", &m_fAlpha, 0.0f, 1.0f);
	ImGui::SliderFloat("Exposure", &m_globalCD.exposure, 0.0f, 5.0f);
	ImGui::SliderFloat("Gamma", &m_globalCD.gamma, 0.0f, 5.0f);
	ImGui::SliderFloat("AmbientFactor", &m_materialCD.ambientFactor, 0.0f, 1.0f);

	ImGui::SliderFloat3("LightStrength", &m_globalCD.light.lightStrength.x, 0.0f, 20.0f);
	if (m_vecLights[0])
	{
		ImGui::SliderFloat3("LightPos", &m_vecLights[0]->m_translation.x, -5.0f, 5.0f);
		ImGui::SliderFloat("SpotFactor", &m_vecLights[0]->m_fSpotFactor, 0.0f, 10.0f);
		ImGui::SliderFloat("Light Radius", &m_vecLights[0]->m_fRadius, 0.0f, 10.0f);
	}
	ImGui::SliderFloat3("HaloFactor", &m_globalCD.light.haloFactor.x, 0.0f, 10.0f);
	

	// ImGui::SliderFloat("fallOfStart", &m_pixelConstantData.light.fallOfStart, 0.0f, 5.0f);
	// ImGui::SliderFloat("fallOfEnd", &m_pixelConstantData.light.fallOfEnd, 0.0f, 10.0f);
}

void RenderScene::RenderDepthOnly(ComPtr<ID3D11DeviceContext>& context, ComPtr<ID3D11DepthStencilView>& dsv)
{
	Graphics::g_depthOnlyPSO.Setting();
	context->ClearDepthStencilView(dsv.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
	context->OMSetRenderTargets(0, nullptr, dsv.Get());
	for (auto& mesh : m_vecMesh)
		mesh->Render(context);
	m_skybox->Render(context);
	for (auto& mirror : m_vecMirrors)
		mirror->Render(context);
}

void RenderScene::Render(ComPtr<ID3D11DeviceContext>& context, bool drawWireFrame)
{
	float clearColor[4] = { 0.0f,0.0f,0.0f,1.0f };
	context->ClearRenderTargetView(m_msaaTexture->GetRTV().Get(), clearColor);
	context->ClearRenderTargetView(m_notMsaaTexture->GetRTV().Get(), clearColor);

	context->RSSetViewports(1, &m_pOwner->m_viewPort);
	{
		this->RenderDepthOnly(context, m_depthOnlyDSV);
		this->RenderLightView(context);
	}

	{
		context->ClearDepthStencilView(m_pOwner->m_useMSAADSV.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
		context->OMSetRenderTargets(1, m_msaaTexture->GetRTV().GetAddressOf(), m_pOwner->m_useMSAADSV.Get());

		this->RenderObject(context, drawWireFrame);
	}

	// 현재까지는 MSAA가 지원되는 RTV에 렌더링하였고 이제는 그걸 MSAA지원 안되는 RTV에 복사
	context->ResolveSubresource(m_notMsaaTexture->GetTexture().Get(), 0, m_msaaTexture->GetTexture().Get(), 0
		, DXGI_FORMAT_R16G16B16A16_FLOAT);

	{
		Graphics::g_postEffectsPSO.Setting();
		vector<ID3D11ShaderResourceView*> srv =
		{ m_notMsaaTexture->GetSRV().Get(), m_depthOnlySRV.Get()/*GETCURSCENE()->m_vecLights[0]->GetLightViewSRV().Get()*/};
		context->PSSetShaderResources(20, UINT(srv.size()), srv.data());
		context->OMSetRenderTargets(1, m_postEffectsTexture->GetRTV().GetAddressOf(), nullptr); // DSV사용 x
		m_postEffects->Render(context);
	}

	{
		Graphics::g_postProcessPSO.Setting();
		m_postProcess->Render(context);
	}
	
}

void RenderScene::RenderObject(ComPtr<ID3D11DeviceContext>& context, bool drawWireFrame)
{
	for (auto& light : m_vecLights)
	{
		m_globalCD.light.lightPos = light->m_translation;
		m_globalCD.light.lightDir = light->GetLightDir();
		m_globalCD.light.lightViewProj = light->GetViewProj();
		m_globalCD.light.spotFactor = light->m_fSpotFactor;
		m_globalCD.light.radius = light->m_fRadius;
		D3DUtils::GetInst().UpdateBuffer<GlobalConstData>(m_globalCD, m_globalConstBuffer);

		// 다시 바꿔야함, 모든걸 한번에 넣어야함
		context->PSSetShaderResources(10 + (UINT)IBL_TYPE::END, 1, light->GetLightViewSRV().GetAddressOf());

		drawWireFrame ? Graphics::g_defaultWirePSO.Setting() : Graphics::g_defaultSolidPSO.Setting();
		for (auto& mesh : m_vecMesh)
			mesh->Render(context);

		drawWireFrame ? Graphics::g_skyBoxWirePSO.Setting() : Graphics::g_skyBoxSolidPSO.Setting();
		m_skybox->Render(context);

		// 노말 그리기
		if (m_drawNormal)
		{
			Graphics::g_normalPSO.Setting();
			for (auto& mesh : m_vecMesh)
				mesh->DrawNormal(context);
		}
		drawWireFrame ? Graphics::g_defaultWirePSO.Setting() : Graphics::g_defaultSolidPSO.Setting();
		context->ClearDepthStencilView(SceneMgr::GetInst().GetDepthStencilView().Get(), D3D11_CLEAR_STENCIL, 1.0f, 0);
		for (int i = 0; i < m_vecMirrors.size(); ++i)
		{
			GraphicsPSO::SetStencilRef(i + 1);
			Graphics::g_stencilMaskSolidPSO.Setting();
			m_vecMirrors[i]->Render(context);
		}
		for (int i = 0; i < m_vecMirrors.size(); ++i)
		{
			GraphicsPSO::SetStencilRef(i + 1);
			this->RenderMirrorWorld(context, m_vecMirrors[i], drawWireFrame);
		}
	}
}

void RenderScene::RenderLightView(ComPtr<ID3D11DeviceContext>& context)
{
	GlobalConstData prevData = m_globalCD;
	for (auto& light : m_vecLights)
	{
		m_globalCD.view = light->GetView();
		m_globalCD.proj = light->GetProj();
		m_globalCD.viewProj = light->GetViewProj();
		D3DUtils::GetInst().UpdateBuffer<GlobalConstData>(m_globalCD, m_globalConstBuffer);

		RenderDepthOnly(context, light->GetLightViewDSV());
	}
	m_globalCD = prevData;
	D3DUtils::GetInst().UpdateBuffer<GlobalConstData>(m_globalCD, m_globalConstBuffer);
}

void RenderScene::RenderMirrorWorld(ComPtr<ID3D11DeviceContext>& context, shared_ptr<Mirror>& mirror, bool drawWireFrame)
{
	// 여기서 거울을 그릴때마다 depth를 초기화를 해주기 때문에 거울을 하나씩 렌더링 하면 
	// 거울 안쪽세상이 현재 세상보다 위에 그려지는 것

	// context->OMSetDepthStencilState(SceneMgr::GetInst().GetDSS(DSS_TYPE::MASK).Get(), i + 1);
	// m_vecMirrors[i]->Render(context.Get(), m_viewProjBuffer); <- 이렇게 하면 안됌

	// 반드시 모든 거울을 스텐실처리를 하고 하나의 depthstencilview 위에서 전부 그려야한다.
	context->ClearDepthStencilView(SceneMgr::GetInst().GetDepthStencilView().Get(),
		D3D11_CLEAR_DEPTH, 1.0f, 0);

	Matrix basicViewProj = m_globalCD.viewProj;
	drawWireFrame ? Graphics::g_drawMaskWirePSO.Setting() : Graphics::g_drawMaskSolidPSO.Setting();
	this->UpdateGlobalBuffer(mirror->GetMirrorViewProj());
	for (auto& mesh : m_vecMesh)
		mesh->Render(context);

	drawWireFrame ? Graphics::g_drawMaskSkyBoxWirePSO.Setting() : Graphics::g_drawMaskSkyBoxSolidPSO.Setting();
	m_skybox->Render(context);

	float alpha[4] = { m_fAlpha,m_fAlpha ,m_fAlpha ,m_fAlpha };
	GraphicsPSO::SetBlendFactor(alpha);
	drawWireFrame ? Graphics::g_blendWirePSO.Setting() : Graphics::g_blendSolidPSO.Setting();

	// 거울을 그리면서 블랜딩을 해준다.
	this->UpdateGlobalBuffer(basicViewProj);
	for (auto& mirror : m_vecMirrors)
		mirror->Render(context);
}

void RenderScene::CalcPickingObject()
{
	if (!m_focusObj)
		return;
	if (!((KEYCHECK(LBUTTON, TAP) || KEYCHECK(LBUTTON, HOLD) || KEYCHECK(RBUTTON, TAP) || KEYCHECK(RBUTTON, HOLD))))
		return;
	Vector2 mouseNDCPos = KeyMgr::GetInst().GetMouseNDCPos();
	Vector3 nearPlane = Vector3(mouseNDCPos.x, mouseNDCPos.y, 0.0f);
	Vector3 farPlane = Vector3(mouseNDCPos.x, mouseNDCPos.y, 1.0f);
	Matrix inverseProjView = (m_globalCD.viewProj).Invert();
	Vector3 rayStartPos = Vector3::Transform(nearPlane
		, inverseProjView);
	Vector3 rayFinishPos = Vector3::Transform(farPlane
		, inverseProjView);
	Vector3 rayDir = rayFinishPos - rayStartPos;
	rayDir.Normalize();
	MyRay ray{ rayStartPos,rayDir };
	float dist = -1.0f;
	bool isCollision = false;
	for (auto& obj : m_vecMesh)
	{
		isCollision = obj->IsCollision(ray, dist);
		if (isCollision)
		{
			m_focusObj = obj;
			break;
		}
	}
	if (isCollision)
	{
		Vector3 collisionPos = rayStartPos + dist * rayDir;
		Vector3 center = m_focusObj->m_translation;
		static Vector3 prevPos = collisionPos;

		if (KEYCHECK(LBUTTON, HOLD))
		{
			Quaternion q = Quaternion::FromToRotation(prevPos - center, collisionPos - center);
			m_focusObj->m_collisionRotationMatrix *= Matrix::CreateFromQuaternion(q);
			prevPos = collisionPos;
		}
		else if (KEYCHECK(LBUTTON, TAP) || KEYCHECK(RBUTTON, TAP))
		{
			prevPos = collisionPos;
		}
		else if (KEYCHECK(RBUTTON, HOLD))
		{
			center = Vector3::Transform(center, m_globalCD.view);
			collisionPos = Vector3::Transform(collisionPos, m_globalCD.view);
			prevPos = Vector3::Transform(prevPos, m_globalCD.view);
			Vector3 dir = collisionPos - prevPos;
			dir.z = 0.0f;
			center += dir;
			center = Vector3::Transform(center, m_globalCD.view.Invert());
			m_focusObj->m_translation = center;
			collisionPos = Vector3::Transform(collisionPos, m_globalCD.view.Invert());
			prevPos = collisionPos;
		}
	}
}

void RenderScene::UpdateGlobalCD()
{
	m_globalCD.eyePos = m_camera->GetPos();
	m_globalCD.view = m_camera->m_view;
	m_globalCD.proj = m_camera->m_projection;
	m_globalCD.invProj = m_globalCD.proj.Invert();
	m_globalCD.viewProj = m_globalCD.view * m_globalCD.proj;
	m_globalCD.strengthIBL = 1.0f;
}

void RenderScene::UpdateGlobalBuffer(const Matrix& viewProj)
{
	m_globalCD.viewProj = viewProj;
	D3DUtils::GetInst().UpdateBuffer<GlobalConstData>(m_globalCD, m_globalConstBuffer);
}

void RenderScene::CreateRenderBuffer()
{
	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.Width = UINT(m_pOwner->m_fWidth);
	desc.Height = UINT(m_pOwner->m_fHeight);
	desc.MipLevels = desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	if (m_pOwner->m_iNumOfMultiSamplingLevel)
	{
		desc.SampleDesc.Count = 4;
		desc.SampleDesc.Quality = m_pOwner->m_iNumOfMultiSamplingLevel - 1;
	}
	else
	{
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
	}
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_RENDER_TARGET;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;
	m_msaaTexture->Init(desc);

	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.BindFlags |= D3D11_BIND_SHADER_RESOURCE;
	m_notMsaaTexture->Init(desc);

	m_postEffectsTexture->Init(desc);
}
