#pragma once
#include "AnimateScene.h"
class GridSimulation :
    public AnimateScene
{
public:
    explicit GridSimulation(SceneMgr* pOwner);
    
public:
    virtual void Update(float dt);
    virtual void Render(ComPtr<ID3D11DeviceContext>& context, bool drawWireFrame);
    void Enter() override;

private:
    // 마우스 클릭좌표를 그대로 받고 cs로 넘겨주고 cs에서는 그 좌표랑 픽셀의 거리를 계산해서
    // 색깔결정하고(dist * color) -> 매 프레임마다 색깔 흐려지게 해주기
    ComPtr<ID3D11ComputeShader> m_densityCS;
    ComPtr<ID3D11ComputeShader> m_advectionCS;
    ComPtr<ID3D11ComputeShader> m_divergenceCS;
    ComPtr<ID3D11ComputeShader> m_jacobiCS;
    ComPtr<ID3D11ComputeShader> m_applyPressureCS;

    Texture2D m_density;
    Texture2D m_densityTemp;
    Texture2D m_velocity;
    Texture2D m_velocityTemp;
    Texture2D m_divergence;
    Texture2D m_pressure;
    Texture2D m_pressureTemp;

    struct GridConst
    {
        Vector4 densityColor;
        Vector2 velocity;
        UINT mouseX;
        UINT mouseY;
        float dt;
        Vector3 dummy;
    }m_constData;
    ComPtr<ID3D11Buffer> m_constBuffer;

    Vector2 m_curMousePos;
    Vector2 m_prevMousePos;
};

