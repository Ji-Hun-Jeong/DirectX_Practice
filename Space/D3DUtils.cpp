#include "pch.h"
#include "D3DUtils.h"
#include "Core.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
D3DUtils D3DUtils::m_inst;
D3DUtils::D3DUtils()
{

}

bool D3DUtils::CreateDeviceAndSwapChain(UINT& numOfMultiSamplingLevel)
{
	D3D_DRIVER_TYPE driverType = D3D_DRIVER_TYPE::D3D_DRIVER_TYPE_HARDWARE;
	UINT createDeviceFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
	D3D_FEATURE_LEVEL featureLevel[] = { D3D_FEATURE_LEVEL_11_0 ,D3D_FEATURE_LEVEL_9_3 };
	D3D_FEATURE_LEVEL outputLevel;

	D3D11CreateDevice(nullptr, driverType, 0, createDeviceFlags, featureLevel,
		ARRAYSIZE(featureLevel), D3D11_SDK_VERSION, m_device.GetAddressOf(),
		&outputLevel, m_context.GetAddressOf());

	m_device->CheckMultisampleQualityLevels(DXGI_FORMAT_R16G16B16A16_FLOAT, 4,
		&numOfMultiSamplingLevel);

	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));
	swapChainDesc.BufferDesc.Width = UINT(Core::GetInst().m_fWidth);
	swapChainDesc.BufferDesc.Height = UINT(Core::GetInst().m_fHeight);
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	swapChainDesc.BufferCount = 2;
	swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	swapChainDesc.BufferUsage = DXGI_USAGE_SHADER_INPUT | DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.OutputWindow = Core::GetInst().GetMainWindow();
	swapChainDesc.Windowed = TRUE;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	if (numOfMultiSamplingLevel > 0)
	{
		swapChainDesc.SampleDesc.Count = 4;
		swapChainDesc.SampleDesc.Quality = numOfMultiSamplingLevel - 1;
	}
	else
	{
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.SampleDesc.Quality = 0;
	}
	HRESULT result = D3D11CreateDeviceAndSwapChain(nullptr, driverType, 0
		, createDeviceFlags, featureLevel, 1
		, D3D11_SDK_VERSION, &swapChainDesc
		, m_swapChain.GetAddressOf(), m_device.GetAddressOf()
		, &outputLevel, m_context.GetAddressOf());
	if (FAILED(result))
		return false;
	return true;
}

bool D3DUtils::CreateRenderTargetView(ID3D11Resource* resource
	, D3D11_RENDER_TARGET_VIEW_DESC* desc, ComPtr<ID3D11RenderTargetView>& rtv)
{
	HRESULT result = m_device->CreateRenderTargetView(resource, desc, rtv.GetAddressOf());
	if (FAILED(result))
		return false;
	return true;
}

bool D3DUtils::CreateRasterizerState(const D3D11_RASTERIZER_DESC* desc, ComPtr<ID3D11RasterizerState>& rasterizerState)
{
	HRESULT result = m_device->CreateRasterizerState(desc, rasterizerState.GetAddressOf());
	if (FAILED(result))
		return false;
	return true;
}

void D3DUtils::SetViewPort(const D3D11_VIEWPORT* viewPort)
{
	m_context->RSSetViewports(1, viewPort);
}

bool D3DUtils::CreateDepthStencilView(D3D11_TEXTURE2D_DESC* depthBufferDesc, ComPtr<ID3D11Texture2D>& depthBuffer, const D3D11_DEPTH_STENCIL_VIEW_DESC* depthStencilViewdesc
	, ComPtr<ID3D11DepthStencilView>& depthStencilView)
{
	HRESULT result = m_device->CreateTexture2D(depthBufferDesc, nullptr, depthBuffer.GetAddressOf());
	if (FAILED(result))
		return false;
	result = m_device->CreateDepthStencilView(depthBuffer.Get(), depthStencilViewdesc, depthStencilView.GetAddressOf());
	if (FAILED(result))
		return false;
	return true;
}

bool D3DUtils::CreateDepthStencilState(const D3D11_DEPTH_STENCIL_DESC* desc, ComPtr<ID3D11DepthStencilState>& depthStencilState)
{
	HRESULT result = m_device->CreateDepthStencilState(desc, depthStencilState.GetAddressOf());
	if (FAILED(result))
		return false;
	return true;
}

void D3DUtils::CreateVertexShaderAndInputLayout(const wstring& hlslPrefix, ComPtr<ID3D11VertexShader>& vs, const vector<D3D11_INPUT_ELEMENT_DESC>& inputLayoutDesc, ComPtr<ID3D11InputLayout>& inputLayout)
{
	ComPtr<ID3DBlob> shaderBlob;
	ComPtr<ID3DBlob> errorBlob;
	wstring fileName = hlslPrefix + L"VertexShader.hlsl";
	UINT compileFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
	compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
	HRESULT result = D3DCompileFromFile(fileName.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"main", "vs_5_0", compileFlags, 0, shaderBlob.GetAddressOf(), errorBlob.GetAddressOf());
	if (FAILED(result))
		assert(0);
	/*if (errorBlob)
		assert(0);*/

	result = m_device->CreateVertexShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), nullptr, &vs);
	if (FAILED(result))
		assert(0);
	result = m_device->CreateInputLayout(inputLayoutDesc.data(), UINT(inputLayoutDesc.size())
		, shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), inputLayout.GetAddressOf());
	if (FAILED(result))
		assert(0);
}

void D3DUtils::CreateGeometryShader(const wstring& hlslPrefix, ComPtr<ID3D11GeometryShader>& gs)
{
	ComPtr<ID3DBlob> shaderBlob;
	ComPtr<ID3DBlob> errorBlob;
	wstring fileName = hlslPrefix + L"GeometryShader.hlsl";
	UINT compileFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
	compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
	HRESULT result = D3DCompileFromFile(fileName.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"main", "gs_5_0", compileFlags, 0, shaderBlob.GetAddressOf(), errorBlob.GetAddressOf());
	if (FAILED(result))
		assert(0);
	result = m_device->CreateGeometryShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), nullptr, &gs);
	if (FAILED(result))
		assert(0);
}

void D3DUtils::CreatePixelShader(const wstring& hlslPrefix, ComPtr<ID3D11PixelShader>& ps)
{
	ComPtr<ID3DBlob> shaderBlob;
	ComPtr<ID3DBlob> errorBlob;
	wstring fileName = hlslPrefix + L"PixelShader.hlsl";
	UINT compileFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
	compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
	HRESULT result = D3DCompileFromFile(fileName.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE
		, "main", "ps_5_0", compileFlags, 0, shaderBlob.GetAddressOf(), errorBlob.GetAddressOf());
	if (FAILED(result))
		assert(0);
	/*if (errorBlob)
		assert(0);*/
	result = m_device->CreatePixelShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize()
		, nullptr, ps.GetAddressOf());
	if (FAILED(result))
		assert(0);
}

void D3DUtils::CreateSamplerState(ComPtr<ID3D11SamplerState>& samplerState)
{
	D3D11_SAMPLER_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	desc.MinLOD = 0;
	desc.MaxLOD = D3D11_FLOAT32_MAX;
	m_device->CreateSamplerState(&desc, samplerState.GetAddressOf());
}

void D3DUtils::ReadImage(const string& fileName, ComPtr<ID3D11Texture2D>& texture, ComPtr<ID3D11ShaderResourceView>& shaderResourceView)
{
	int width = 0;
	int height = 0;
	int channel = 0;
	vector<uint8_t> image;
	DXGI_FORMAT format;
	string fileFormat = fileName.substr(fileName.size() - 3);
	if (fileFormat=="exr")
	{
		format = DXGI_FORMAT_R16G16B16A16_FLOAT;
		ReadHDRImage(fileName, format, image, width, height);
	}
	else
	{
		format = DXGI_FORMAT_R8G8B8A8_UNORM;
		ReadLDRImage(fileName, format, image, width, height);
	}

	this->CreateSRVByStaging(image, width, height, format, texture, shaderResourceView);
}

void D3DUtils::ReadLDRImage(const string& fileName, DXGI_FORMAT pixelFormat, vector<uint8_t>& image, int& width, int& height)
{
	int alphaChannel = 0;
	unsigned char* img = stbi_load(fileName.c_str(), &width, &height, &alphaChannel, 0);
	//assert(alphaChannel == 4);

	image.resize(width * height * 4);
	for (size_t i = 0; i < width * height; i++)
	{
		for (size_t c = 0; c < 3; c++)
		{
			image[4 * i + c] = img[i * alphaChannel + c];
		}
		image[4 * i + 3] = 255;
	}

}

void D3DUtils::ReadHDRImage(const string& fileName, DXGI_FORMAT pixelFormat, vector<uint8_t>& image, int& width, int& height)
{
	const wstring szFile(fileName.begin(), fileName.end());
	TexMetadata metaData;
	CHECKRESULT(GetMetadataFromEXRFile(szFile.c_str(), metaData));
	ScratchImage img;
	CHECKRESULT(LoadFromEXRFile(szFile.c_str(), &metaData, img));
	
	width = int(metaData.width);
	height = int(metaData.height);
	if (pixelFormat != metaData.format)
		assert(0);
	image.resize(img.GetPixelsSize());	// 그냥 메모리 통째로 복사 그럼 gpu내부에서 알아서 16비트 단위로 읽어들임
	memcpy(image.data(), img.GetPixels(), image.size());
}

void D3DUtils::ReadImage1(const string& fileName, ComPtr<ID3D11Texture2D>& texture, ComPtr<ID3D11ShaderResourceView>& shaderResourceView)
{
	int width = 0;
	int height = 0;
	int alphaChannel = 0;
	unsigned char* img = stbi_load(fileName.c_str(), &width, &height, &alphaChannel, 0);
	//assert(alphaChannel == 4);

	std::vector<uint8_t> image;
	image.resize(width * height * 4);
	for (size_t i = 0; i < width * height; i++)
	{
		for (size_t c = 0; c < 3; c++)
		{
			image[4 * i + c] = img[i * alphaChannel + c];
		}
		image[4 * i + 3] = 255;
	}


	D3D11_TEXTURE2D_DESC textureDesc;
	ZeroMemory(&textureDesc, sizeof(textureDesc));
	textureDesc.Width = width;
	textureDesc.Height = height;
	textureDesc.MipLevels = textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_IMMUTABLE;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

	D3D11_SUBRESOURCE_DATA dataDesc;
	dataDesc.pSysMem = image.data();
	dataDesc.SysMemPitch = textureDesc.Width * 4 * sizeof(uint8_t);
	//dataDesc.SysMemSlicePitch = 0;

	m_device->CreateTexture2D(&textureDesc, &dataDesc, texture.GetAddressOf());
	m_device->CreateShaderResourceView(texture.Get(), nullptr, shaderResourceView.GetAddressOf());
}





