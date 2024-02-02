#pragma once
#include "Mesh.h"
#include "SweepPath.h"

namespace Nome::Scene
{

class CHilbert : public CSweepPath
{
    DEFINE_INPUT(float, Iteration) { MarkDirty(); }
    DEFINE_INPUT(float, Size) { MarkDirty(); }
    DEFINE_OUTPUT_WITH_UPDATE(CSweepPathInfo*, Hilbert) { UpdateEntity(); }

public:
    DECLARE_META_CLASS(CHilbert, CSweepPath);
    CHilbert() = default;
    CHilbert(const std::string& name)
        : CSweepPath(name)
    {
    }

    void UpdateEntity() override;
    void MarkDirty() override;

};

}
