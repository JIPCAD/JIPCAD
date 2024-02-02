#pragma once
#include "Mesh.h"
#include "SweepPath.h"

namespace Nome::Scene
{

class CHilbCube : public CSweepPath
{
    DEFINE_INPUT(float, Iteration) { MarkDirty(); }
    DEFINE_INPUT(float, Size) { MarkDirty(); }
    DEFINE_OUTPUT_WITH_UPDATE(CSweepPathInfo*, HilbCube) { UpdateEntity(); }

public:
    DECLARE_META_CLASS(CHilbCube, CSweepPath);
    CHilbCube() = default;
    CHilbCube(const std::string& name)
        : CSweepPath(name)
    {
    }

    void UpdateEntity() override;
    void MarkDirty() override;

};

}
