#pragma once
#include "AnimateScene.h"
class CurlNoise :
    public AnimateScene
{
public:
    explicit CurlNoise(SceneMgr* pOwner);

public:
    virtual void Update(float dt);
    virtual void Render(ComPtr<ID3D11DeviceContext>& context, bool drawWireFrame);
    void Enter() override;

private:
    ComPtr<ID3D11VertexShader> m_curlNoiseVS;
    ComPtr<ID3D11GeometryShader> m_curlNoiseGS;
    ComPtr<ID3D11PixelShader> m_curlNoisePS;
    ComPtr<ID3D11InputLayout> m_curlNoiseIL;
    ComPtr<ID3D11BlendState> m_curlNoiseBS;
    ComPtr<ID3D11ComputeShader> m_curlNoiseCS;

};

