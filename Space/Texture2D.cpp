#include "pch.h"
#include "Texture2D.h"
#include "D3DUtils.h"

Texture2D::Texture2D()
    : m_iWidth(0)
    , m_iHeight(0)
{
}

void Texture2D::Init(const vector<uint8_t>& vec, UINT width, UINT height, DXGI_FORMAT pixelFormat)
{
    auto& context = D3DUtils::GetInst().GetContext();
    auto& device = D3DUtils::GetInst().GetDevice();

    D3D11_TEXTURE2D_DESC texDesc;
    ZeroMemory(&texDesc, sizeof(texDesc));
    texDesc.Width = width;
    texDesc.Height = height;
    texDesc.MipLevels = texDesc.ArraySize = 1;
    texDesc.Format = pixelFormat;
    texDesc.SampleDesc.Count = 1;
    texDesc.SampleDesc.Quality = 0;
    texDesc.Usage = D3D11_USAGE_STAGING;
    texDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ;
    texDesc.MiscFlags = 0;

    D3D11_MAPPED_SUBRESOURCE ms;
    context->Map(m_stagingTexture.Get(), NULL, D3D11_MAP_WRITE, NULL, &ms);

    uint8_t* dst = (uint8_t*)vec.data();
    uint8_t* src = (uint8_t*)ms.pData;
    for (UINT i = 0; i < height; ++i)
        memcpy(&dst[i * width], &src[i * width], sizeof(uint8_t) * width);

    context->Unmap(m_stagingTexture.Get(), NULL);

    context->CopyResource(m_texture.Get(), m_stagingTexture.Get());

    CHECKRESULT(device->CreateShaderResourceView(m_texture.Get(), nullptr, m_srv.GetAddressOf()));
    CHECKRESULT(device->CreateRenderTargetView(m_texture.Get(), nullptr, m_rtv.GetAddressOf()));
    CHECKRESULT(device->CreateUnorderedAccessView(m_texture.Get(), nullptr, m_uav.GetAddressOf()));
}
