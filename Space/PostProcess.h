#pragma once
class ImageFilter;
class PostProcess
{
public:
    explicit PostProcess(float width, float height, UINT bloomLevel, ComPtr<ID3D11ShaderResourceView>& srv, ComPtr<ID3D11RenderTargetView>& rtv);
public:
    void Update(float dt);
    void Render(ComPtr<ID3D11DeviceContext>& context);
    void CreateFilters(UINT bloomLevel);
private:
    ComPtr<ID3D11ShaderResourceView> m_originalSRV;
    ComPtr<ID3D11RenderTargetView> m_finalRTV;

    ComPtr<ID3D11VertexShader> m_vertexShader;

    ComPtr<ID3D11PixelShader> m_copyShader;
    ComPtr<ID3D11PixelShader> m_blurShader;
    ComPtr<ID3D11PixelShader> m_bloomShader;
    ComPtr<ID3D11PixelShader> m_combineShader;

    ComPtr<ID3D11Buffer> m_vertexBuffer;
    ComPtr<ID3D11Buffer> m_indexBuffer;
    UINT m_indexCount;

    vector<shared_ptr<ImageFilter>> m_filters;

    ComPtr<ID3D11Buffer> m_constBuffer;

    float m_fWidth;
    float m_fHeight;
    struct ImageFilterConstData
    {
        float dx;
        float dy;
        float threshold;
        float strength;
    }m_constData;

    

    friend class ImageFilter;
};

