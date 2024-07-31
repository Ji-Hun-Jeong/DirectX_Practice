#pragma once
#include "D3DUtils.h"
template <typename T_Struct>
class StructuredBuffer
{
public:
	void Init(const vector<T_Struct>& vec)
	{
		D3DUtils::GetInst().CreateStructuredBuffer(sizeof(T_Struct), UINT(vec.size()), m_buffer, m_srv, m_uav);
		D3DUtils::GetInst().CreateStagingBuffer(sizeof(T_Struct), UINT(vec.size()), m_stagingBuffer);

		UploadToBuffer(vec);
	}

	void UploadToBuffer(const vector<T_Struct>& vec)
	{
		m_vec = vec;
		D3DUtils::GetInst().UpdateBuffer(m_vec, m_stagingBuffer);
		D3DUtils::GetInst().GetContext()->CopyResource(m_buffer.Get(), m_stagingBuffer.Get());
	}

public:
	auto GetSRV() { return m_srv.Get(); }
	auto GetUAV() { return m_uav.Get(); }
	auto GetAddressSRV() { return m_srv.GetAddressOf(); }
	auto GetAddressUAV() { return m_uav.GetAddressOf(); }
	auto GetBufferSize() { return m_vec.size(); }

private:
	vector<T_Struct> m_vec;
	ComPtr<ID3D11Buffer> m_buffer;
	ComPtr<ID3D11Buffer> m_stagingBuffer;

	ComPtr<ID3D11ShaderResourceView> m_srv;
	ComPtr<ID3D11UnorderedAccessView> m_uav;
};

