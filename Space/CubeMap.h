#pragma once
#include "Mesh.h"
class CubeMap : public Mesh
{
public:
	CubeMap(const Vector3& translation, const Vector3& rotation1, const Vector3& rotation2, const Vector3& scale);
	CubeMap();
	virtual void UpdateVertexConstantData(float dt) override;
	virtual void ReadyToRender(ID3D11DeviceContext* context) override;
	virtual void CreateShaderResourceView(const string& textureName) override;
private:
	ComPtr<ID3D11ShaderResourceView> m_cubeMapResource;
};

