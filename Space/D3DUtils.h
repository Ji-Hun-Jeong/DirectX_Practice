#pragma once
class D3DUtils
{
// InitDirect3D
public:
	bool CreateDeviceAndSwapChain(UINT& numOfMultiSamplingLevel);
	bool CreateRenderTargetView(ID3D11Resource* resource
		, D3D11_RENDER_TARGET_VIEW_DESC* desc, ComPtr<ID3D11RenderTargetView>& rtv);
	bool CreateRasterizerState(const D3D11_RASTERIZER_DESC* desc, ComPtr<ID3D11RasterizerState>& rasterizerState);
	void SetViewPort(const D3D11_VIEWPORT* viewPort);
	bool CreateDepthStencilView(D3D11_TEXTURE2D_DESC* depthBufferDesc, ComPtr<ID3D11Texture2D>& depthBuffer, const D3D11_DEPTH_STENCIL_VIEW_DESC* depthStencilViewdesc
		, ComPtr<ID3D11DepthStencilView>& depthStencilView);
	bool CreateDepthStencilState(const D3D11_DEPTH_STENCIL_DESC* desc, ComPtr<ID3D11DepthStencilState>& depthStencilState);
	
// CreateShader
public:
	void CreateVertexShaderAndInputLayout(const wstring& hlslPrefix, ComPtr<ID3D11VertexShader>& vs
		, const vector<D3D11_INPUT_ELEMENT_DESC>& inputLayoutDesc, ComPtr<ID3D11InputLayout>& inputLayout);
	void CreateGeometryShader(const wstring& hlslPrefix, ComPtr<ID3D11GeometryShader>& gs);
	void CreatePixelShader(const wstring& hlslPrefix, ComPtr<ID3D11PixelShader>& ps);
	void CreateSamplerState(ComPtr<ID3D11SamplerState>& samplerState);
	void ReadImage(const string& fileName, ComPtr<ID3D11Texture2D>& texture, ComPtr<ID3D11ShaderResourceView>& shaderResourceView);

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
	void UpdateBuffer(ComPtr<ID3D11Buffer>& buffer, const T& bufferData)
	{
		assert(buffer);
		D3D11_MAPPED_SUBRESOURCE resourceData;
		m_context->Map(buffer.Get(), NULL, D3D11_MAP_WRITE_DISCARD, NULL, &resourceData);
		memcpy(resourceData.pData, &bufferData, sizeof(bufferData));
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

