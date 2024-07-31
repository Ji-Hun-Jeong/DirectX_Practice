#pragma once
#include "Buffer.h"
template <typename T_Struct>
class IndirectArgsBuffer :
    public Buffer<T_Struct>
{
public:
    void Init(const vector<T_Struct>& vec) override;
};

template<typename T_Struct>
inline void IndirectArgsBuffer<T_Struct>::Init(const vector<T_Struct>& vec)
{
    D3DUtils::GetInst().CreateIndirectArgsBuffer(vec.size(), this->m_buffer, this->m_uav);
    D3DUtils::GetInst().CreateStagingBuffer(sizeof(T_Struct), vec.size(), this->m_stagingBuffer);

    this->UploadToBuffer(vec);
}
