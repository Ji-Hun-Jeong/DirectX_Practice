#include "pch.h"
#include "Scene.h"
#include "D3DUtils.h"
#include "Camera.h"
#include "GeometryGenerator.h"
#include "KeyMgr.h"
#include "SceneMgr.h"
#include "Mirror.h"
#include "GraphicsCommons.h"
#include "GraphicsPSO.h"
#include "PostProcess.h"
#include "Light.h"

Scene::Scene()
	: m_camera(make_shared<Camera>())
{

}

void Scene::Init()
{
	D3DUtils::GetInst().CreateConstantBuffer<GlobalConstData>(m_globalCD, m_globalConstBuffer);
	InitIBL();
	InitMesh();
	InitSkyBox();
}

void Scene::Update(float dt)
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
}

void Scene::RenderDepthOnly(ComPtr<ID3D11DeviceContext>& context, ComPtr<ID3D11DepthStencilView>& dsv)
{
	Graphics::g_depthOnlyPSO.Setting();
	context->ClearDepthStencilView(dsv.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0.0f);
	context->OMSetRenderTargets(0, nullptr, dsv.Get());
	for (auto& mesh : m_vecMesh)
		mesh->Render(context);
	m_skybox->Render(context);
	for (auto& mirror : m_vecMirrors)
		mirror->Render(context);
}

void Scene::Render(ComPtr<ID3D11DeviceContext>& context, bool drawWireFrame)
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
		if (GETCURSCENE()->m_drawNormal)
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

void Scene::RenderLightView(ComPtr<ID3D11DeviceContext>& context)
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

void Scene::RenderMirrorWorld(ComPtr<ID3D11DeviceContext>& context, shared_ptr<Mirror>& mirror, bool drawWireFrame)
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

void Scene::ReadCubeImage(const string& fileName, IBL_TYPE textureType)
{
	if (fileName == "")
		return;
	ComPtr<ID3D11Texture2D> texture;
	ComPtr<ID3D11ShaderResourceView> shaderResourceView;
	D3DUtils::GetInst().ReadCubeImage(fileName, texture, shaderResourceView);

	m_iblTexture[(UINT)textureType] = texture;
	m_iblSRV[(UINT)textureType] = shaderResourceView;
}


void Scene::CalcPickingObject()
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

void Scene::UpdateGlobalCD()
{
	m_globalCD.eyePos = m_camera->GetPos();
	m_globalCD.view = m_camera->m_view;
	m_globalCD.proj = m_camera->m_projection;
	m_globalCD.invProj = m_globalCD.proj.Invert();
	m_globalCD.viewProj = m_globalCD.view * m_globalCD.proj;
	m_globalCD.strengthIBL = 1.0f;
}

void Scene::UpdateGlobalBuffer(const Matrix& viewProj)
{
	m_globalCD.viewProj = viewProj;
	D3DUtils::GetInst().UpdateBuffer<GlobalConstData>(m_globalCD, m_globalConstBuffer);
}
