#pragma once
#include "Mesh.h"

namespace Nome::Scene
{

class CCube : public CMesh
{
    DEFINE_INPUT(float, Height) { MarkDirty(); }

public:
    DECLARE_META_CLASS(CCube, CMesh);
    CCube() = default;
    CCube(const std::string& name)
        : CMesh(std::move(name))
    {
    }

    void UpdateEntity() override;

private:
    bool bBotCap = false;
    bool bTopCap = false;
};

}