#pragma once
#include "NonObject.h"
class ImageFilter;
class PostProcess :
    public NonObject
{
public:
    explicit PostProcess(float width, float height, ComPtr<ID3D11ShaderResourceView>& srv, ComPtr<ID3D11RenderTargetView>& rtv);
public:
    void Update(float dt) override;
    void Render(ID3D11DeviceContext* context) override;
    void CreateFilters();
private:
    ComPtr<ID3D11ShaderResourceView> m_originalSRV;
    ComPtr<ID3D11RenderTargetView> m_finalRTV;

    ComPtr<ID3D11PixelShader> m_blurXShader;
    ComPtr<ID3D11PixelShader> m_blurYShader;
    ComPtr<ID3D11PixelShader> m_bloomShader;
    ComPtr<ID3D11PixelShader> m_combineShader;

    vector<shared_ptr<ImageFilter>> m_filters;

    float m_fWidth;
    float m_fHeight;
    friend class ImageFilter;
};

