#pragma once
class D3DUtils
{
	// InitDirect3D
public:
	bool CreateDeviceAndSwapChain();
	bool CreateRenderTargetView(ID3D11Resource* resource
		, D3D11_RENDER_TARGET_VIEW_DESC* desc, ComPtr<ID3D11RenderTargetView>& rtv);
	bool CreateRasterizerState(const D3D11_RASTERIZER_DESC* desc, ComPtr<ID3D11RasterizerState>& rasterizerState);
	void SetViewPort(const D3D11_VIEWPORT* viewPort);
	bool CreateDepthStencilView(D3D11_TEXTURE2D_DESC* depthBufferDesc, ComPtr<ID3D11Texture2D>& depthBuffer, const D3D11_DEPTH_STENCIL_VIEW_DESC* depthStencilViewdesc
		, ComPtr<ID3D11DepthStencilView>& depthStencilView);
	bool CreateDepthStencilState(const D3D11_DEPTH_STENCIL_DESC* desc, ComPtr<ID3D11DepthStencilState>& depthStencilState);
	bool CreateBlendState(const D3D11_BLEND_DESC* desc, ComPtr<ID3D11BlendState>& bs);
	// CreateShader
public:
	void CreateVertexShaderAndInputLayout(const wstring& hlslPrefix, ComPtr<ID3D11VertexShader>& vs
		, const vector<D3D11_INPUT_ELEMENT_DESC>& inputLayoutDesc, ComPtr<ID3D11InputLayout>& inputLayout);
	void CreateGeometryShader(const wstring& hlslPrefix, ComPtr<ID3D11GeometryShader>& gs);
	void CreatePixelShader(const wstring& hlslPrefix, ComPtr<ID3D11PixelShader>& ps);
	void CreateComputeShader(const wstring& hlslPrefix, ComPtr<ID3D11ComputeShader>& cs);
	void CreateSamplerState(ComPtr<ID3D11SamplerState>& samplerState, const D3D11_SAMPLER_DESC& desc);
	void ReadImage(const string& fileName, bool useSRGB,  ComPtr<ID3D11Texture2D>& texture, ComPtr<ID3D11ShaderResourceView>& shaderResourceView);
	void ReadImage1(const string& fileName, ComPtr<ID3D11Texture2D>& texture, ComPtr<ID3D11ShaderResourceView>& shaderResourceView);
	void ReadCubeImage(const string& fileName, ComPtr<ID3D11Texture2D>& texture, ComPtr<ID3D11ShaderResourceView>& shaderResourceView);
	void CreateDepthOnlyResources(float width, float height, ComPtr<ID3D11Texture2D>& buffer, ComPtr<ID3D11DepthStencilView>& dsv, ComPtr<ID3D11ShaderResourceView>& srv);
	void CreateStructuredBuffer(UINT sizeStruct, UINT numOfElement, ComPtr<ID3D11Buffer>& buffer
		, ComPtr<ID3D11ShaderResourceView>& srv, ComPtr<ID3D11UnorderedAccessView>& uav);

	void CreateStagingBuffer(UINT sizeStruct, UINT numOfElement, ComPtr<ID3D11Buffer>& buffer);
	void CreateIndirectArgsBuffer(UINT numOfElement, ComPtr<ID3D11Buffer>& buffer, ComPtr<ID3D11UnorderedAccessView>& uav);

private:
	void ReadLDRImage(const string& fileName, DXGI_FORMAT pixelFormat, vector<uint8_t>& image, int& width, int& height);
	void ReadHDRImage(const string& fileName, DXGI_FORMAT pixelFormat, vector<uint8_t>& image, int& width, int& height);
	template <typename T>
	void CreateSRVByStaging(vector<T>& image, int width, int height, DXGI_FORMAT pixelFormat, ComPtr<ID3D11Texture2D>& texture, ComPtr<ID3D11ShaderResourceView>& srv)
	{
		ComPtr<ID3D11Texture2D> stagingTexture;
		D3D11_TEXTURE2D_DESC textureDesc;
		ZeroMemory(&textureDesc, sizeof(textureDesc));
		textureDesc.Width = width;
		textureDesc.Height = height;
		textureDesc.MipLevels = textureDesc.ArraySize = 1;
		textureDesc.Format = pixelFormat;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.Usage = D3D11_USAGE_STAGING;
		textureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ;
		// BindFlags를 SRV로 하면 안됨

		// 스테이징 텍스쳐를 사용할 때는 저절로 텍스쳐가 만들어지지 않고 복사를 하나하나 해야함
		m_device->CreateTexture2D(&textureDesc, nullptr, stagingTexture.GetAddressOf());

		UINT pixelSize = sizeof(uint8_t) * 4;
		if (pixelFormat == DXGI_FORMAT_R16G16B16A16_FLOAT)
			pixelSize = sizeof(uint16_t) * 4;
		D3D11_MAPPED_SUBRESOURCE ms;
		m_context->Map(stagingTexture.Get(), NULL, D3D11_MAP_WRITE, NULL, &ms);
		uint8_t* pData = (uint8_t*)ms.pData;
		for (UINT h = 0; h < UINT(height); h++)
		{
			memcpy(&pData[h * ms.RowPitch], &image[h * width * pixelSize],
				width * pixelSize);
		}
		m_context->Unmap(stagingTexture.Get(), NULL);

		textureDesc.MipLevels = 0;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
		textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

		m_device->CreateTexture2D(&textureDesc, nullptr, texture.GetAddressOf());

		m_context->CopySubresourceRegion(texture.Get(), 0, 0, 0, 0, stagingTexture.Get(), 0, nullptr);
		m_device->CreateShaderResourceView(texture.Get(), nullptr, srv.GetAddressOf());
		m_context->GenerateMips(srv.Get());
	}
	// CreateBuffer
public:
	template <typename T_Vertex>
	void CreateVertexBuffer(const vector<T_Vertex>& vertices, ComPtr<ID3D11Buffer>& vertexBuffer)
	{
		D3D11_BUFFER_DESC bufferDesc;
		ZeroMemory(&bufferDesc, sizeof(bufferDesc));
		bufferDesc.ByteWidth = (UINT)vertices.size() * sizeof(T_Vertex);
		bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
		bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bufferDesc.CPUAccessFlags = 0;
		bufferDesc.StructureByteStride = sizeof(T_Vertex);

		D3D11_SUBRESOURCE_DATA dataDesc = { 0 };
		dataDesc.pSysMem = vertices.data();
		dataDesc.SysMemPitch = 0;
		dataDesc.SysMemSlicePitch = 0;

		m_device->CreateBuffer(&bufferDesc, &dataDesc, vertexBuffer.GetAddressOf());
	}

	template <typename T_Int>
	void CreateIndexBuffer(const vector<T_Int>& indices, ComPtr<ID3D11Buffer>& indexBuffer)
	{
		D3D11_BUFFER_DESC bufferDesc;
		ZeroMemory(&bufferDesc, sizeof(bufferDesc));
		bufferDesc.ByteWidth = (UINT)indices.size() * sizeof(T_Int);
		bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
		bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bufferDesc.CPUAccessFlags = 0;
		bufferDesc.StructureByteStride = sizeof(T_Int);

		D3D11_SUBRESOURCE_DATA dataDesc = { 0 };
		dataDesc.pSysMem = indices.data();
		dataDesc.SysMemPitch = 0;
		dataDesc.SysMemSlicePitch = 0;
		m_device->CreateBuffer(&bufferDesc, &dataDesc, indexBuffer.GetAddressOf());
	}

	template <typename T_Constant>
	void CreateConstantBuffer(const T_Constant& constantData, ComPtr<ID3D11Buffer>& constantBuffer)
	{
		D3D11_BUFFER_DESC bufferDesc;
		bufferDesc.ByteWidth = sizeof(constantData);
		bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		bufferDesc.MiscFlags = 0;
		bufferDesc.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA dataDesc;
		dataDesc.pSysMem = &constantData;
		dataDesc.SysMemPitch = 0;
		dataDesc.SysMemSlicePitch = 0;
		m_device->CreateBuffer(&bufferDesc, &dataDesc, constantBuffer.GetAddressOf());
	}

	// UpdateDirect3D
public:
	template <typename T>
	void UpdateBuffer(const T& bufferData, ComPtr<ID3D11Buffer>& buffer)
	{
		assert(buffer);
		D3D11_MAPPED_SUBRESOURCE resourceData;
		m_context->Map(buffer.Get(), NULL, D3D11_MAP_WRITE_DISCARD, NULL, &resourceData);
		memcpy(resourceData.pData, &bufferData, sizeof(bufferData));
		m_context->Unmap(buffer.Get(), NULL);
	}

	template <typename T_Struct>
	void UpdateBuffer(const vector<T_Struct>& vec, ComPtr<ID3D11Buffer>& buffer)
	{
		assert(buffer);
		D3D11_MAPPED_SUBRESOURCE ms;
		m_context->Map(buffer.Get(), NULL, D3D11_MAP_WRITE, NULL, &ms);
		memcpy(ms.pData, vec.data(), sizeof(T_Struct) * vec.size());
		m_context->Unmap(buffer.Get(), NULL);
	}

	// DirectX Default Member
private:
	ComPtr<ID3D11Device> m_device;
	ComPtr<ID3D11DeviceContext> m_context;
	ComPtr<IDXGISwapChain> m_swapChain;

	// Getter(), Setter()
public:
	ComPtr<ID3D11Device>& GetDevice() { return m_device; }
	ComPtr<ID3D11DeviceContext>& GetContext() { return m_context; }
	ComPtr<IDXGISwapChain>& GetSwapChain() { return m_swapChain; }

	// SingleTon
	SINGLE(D3DUtils)
};

