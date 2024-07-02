#pragma once
#include "NonObject.h"
class CubeMap : public NonObject
{
public:
	CubeMap();
	explicit CubeMap(const Vector3& translation, const Vector3& rotation1, const Vector3& rotation2, const Vector3& scale);
	virtual void UpdateVertexConstantData(float dt) override;
	virtual void ReadyToRender(ID3D11DeviceContext* context) override;
	virtual void ReadImage(const string& textureName);
private:
	ComPtr<ID3D11ShaderResourceView> m_cubeMapResource;
};

