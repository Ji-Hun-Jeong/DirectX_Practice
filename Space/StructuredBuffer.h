#pragma once
#include "Buffer.h"
template <typename T_Struct>
class StructuredBuffer 
	: public Buffer<T_Struct>
{
public:
	void Init(const vector<T_Struct>& vec) override;

};

template<typename T_Struct>
inline void StructuredBuffer<T_Struct>::Init(const vector<T_Struct>& vec)
{
	D3DUtils::GetInst().CreateStructuredBuffer(sizeof(T_Struct), UINT(vec.size()), this->m_buffer, this->m_srv, this->m_uav);
	D3DUtils::GetInst().CreateStagingBuffer(sizeof(T_Struct), UINT(vec.size()), this->m_stagingBuffer);

	this->UploadToBuffer(vec);
}