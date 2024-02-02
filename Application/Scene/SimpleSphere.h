#pragma once
#include "Mesh.h"

namespace Nome::Scene
{

class CSimpleSphere : public CMesh
{
    DEFINE_INPUT(float, Radius) { MarkDirty(); }
    DEFINE_INPUT(float, ThetaSegs) { MarkDirty(); }
    DEFINE_INPUT(float, PhiSegs) { MarkDirty(); }

public:
    DECLARE_META_CLASS(CSimpleSphere, CMesh);
    CSimpleSphere() = default;
    CSimpleSphere(const std::string& name)
        : CMesh(std::move(name))
    {
    }

    void UpdateEntity() override;
};

}
