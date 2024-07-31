#pragma once
#include "Buffer.h"
template <typename T_Struct>
class IndirectArgsBuffer :
    public Buffer<T_Struct>
{
public:
    void Init() override;
};

template<typename T_Struct>
inline void IndirectArgsBuffer<T_Struct>::Init()
{
    D3DUtils::GetInst().CreateIndirectArgsBuffer(this->m_vec.size(), this->m_buffer, this->m_uav);
    D3DUtils::GetInst().CreateStagingBuffer(sizeof(T_Struct), this->m_vec.size(), this->m_stagingBuffer);

    this->UploadToBuffer();
}
