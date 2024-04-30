#pragma once
#include "Mesh.h"
class Normal :
    public Mesh
{
public:
    Normal(shared_ptr<Mesh> order);
    virtual void UpdateVertexConstantData(float dt);
private:
    shared_ptr<Mesh> m_pOrder;
};

