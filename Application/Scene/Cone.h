#pragma once
#include "Mesh.h"

namespace Nome::Scene
{

class CCone : public CMesh
{
    DEFINE_INPUT(float, Height) { MarkDirty(); }
    DEFINE_INPUT(float, Radius) { MarkDirty(); }
    DEFINE_INPUT(float, PolySides) { MarkDirty(); }

public:
    DECLARE_META_CLASS(CCone, CMesh);
    CCone() = default;
    CCone(const std::string& name)
        : CMesh(std::move(name))
    {
    }

    void UpdateEntity() override;
};

}