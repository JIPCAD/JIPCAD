#pragma once
#include "Mesh.h"
#include "SweepPath.h"

namespace Nome::Scene
{

class CArc : public CSweepPath
{
    DEFINE_INPUT(float, maj_rad) { MarkDirty(); }
    DEFINE_INPUT(float, min_rad) { MarkDirty(); }
    DEFINE_INPUT(float, theta_max) { MarkDirty(); }
    DEFINE_INPUT(float, phi_min) { MarkDirty(); }
    DEFINE_INPUT(float, phi_max) { MarkDirty(); }
    DEFINE_INPUT(float, theta_segs) { MarkDirty(); }
    DEFINE_INPUT(float, phi_segs) { MarkDirty(); }
    DEFINE_OUTPUT_WITH_UPDATE(CSweepPathInfo*, Arc) { UpdateEntity(); }

public:
    DECLARE_META_CLASS(CArc, CSweepPath);
    CArc() = default;
    CArc(const std::string& name)
        : CSweepPath(name)
    {
    }

    void UpdateEntity() override;
    void MarkDirty() override;
};

}