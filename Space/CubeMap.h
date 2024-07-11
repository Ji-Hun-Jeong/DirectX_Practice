#pragma once
#include "NonObject.h"
class CubeMap : public NonObject
{
public:
	CubeMap();
	explicit CubeMap(const Vector3& translation, const Vector3& rotation1, const Vector3& rotation2, const Vector3& scale);
	virtual void UpdateVertexConstantData(float dt) override;
	virtual void ReadyToRender(ID3D11DeviceContext* context, const ComPtr<ID3D11Buffer>& viewProjBuffer) override;
private:
};

