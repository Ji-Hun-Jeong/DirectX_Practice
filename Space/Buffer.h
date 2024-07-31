#pragma once
#include "D3DUtils.h"
template <typename T_Struct>
class Buffer
{
public:
	virtual void Init(const vector<T_Struct>& vec) = 0;
	void UploadToBuffer(const vector<T_Struct>& vec);

public:
	auto GetBuffer() { return m_buffer; }
	auto GetBufferSize() { return m_vec.size(); }
	auto GetSRV() { return m_srv.Get(); }
	auto GetUAV() { return m_uav.Get(); }
	auto GetAddressSRV() { return m_srv.GetAddressOf(); }
	auto GetAddressUAV() { return m_uav.GetAddressOf(); }

protected:
	vector<T_Struct> m_vec;
	ComPtr<ID3D11Buffer> m_buffer;
	ComPtr<ID3D11Buffer> m_stagingBuffer;

	ComPtr<ID3D11ShaderResourceView> m_srv;
	ComPtr<ID3D11RenderTargetView> m_rtv;
	ComPtr<ID3D11UnorderedAccessView> m_uav;
};

template<typename T_Struct>
inline void Buffer<T_Struct>::UploadToBuffer(const vector<T_Struct>& vec)
{
	m_vec = vec;
	D3DUtils::GetInst().UpdateBuffer(m_vec, m_stagingBuffer);
	D3DUtils::GetInst().GetContext()->CopyResource(m_buffer.Get(), m_stagingBuffer.Get());
}
