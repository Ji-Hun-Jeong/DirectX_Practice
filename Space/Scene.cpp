#include "pch.h"
#include "Scene.h"
#include "D3DUtils.h"
#include "Camera.h"
#include "GeometryGenerator.h"
#include "KeyMgr.h"
#include "SceneMgr.h"
#include "Mirror.h"
#include "PostProcess.h"
#include "Light.h"

Scene::Scene(SceneMgr* owner)
	: m_camera(make_shared<Camera>())
	, m_pOwner(owner)
{

}

void Scene::Init()
{
	InitIBL();
	InitMesh();
	InitSkyBox();
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


