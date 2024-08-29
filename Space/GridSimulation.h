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
    // ���콺 Ŭ����ǥ�� �״�� �ް� cs�� �Ѱ��ְ� cs������ �� ��ǥ�� �ȼ��� �Ÿ��� ����ؼ�
    // ��������ϰ�(dist * color) -> �� �����Ӹ��� ���� ������� ���ֱ�
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

