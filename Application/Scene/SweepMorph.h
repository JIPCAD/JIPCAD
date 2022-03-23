#pragma once
#include "Mesh.h"
#include "SweepPath.h"
#include "Sweep.h"
#include "Vector3.h"

namespace Nome::Scene
{

class CSweepMorph : public CSweep
{
    DEFINE_INPUT(CSweepPathInfo *, Path) { MarkDirty(); }
    DEFINE_INPUT(float, Azimuth) { MarkDirty(); }
    DEFINE_INPUT(float, Twist) { MarkDirty(); }

    DEFINE_OUTPUT_WITH_UPDATE(CSweepPathInfo *, MorphPath) { UpdateEntity(); }
    DEFINE_OUTPUT_WITH_UPDATE(CSweepPathInfo *, MorphFirstCrossSection) { UpdateEntity(); }

public:
    DECLARE_META_CLASS(CSweepMorph, CSweep);

    CSweepMorph() = default;
    explicit CSweepMorph(const std::string& name)
        : CSweep(std::move(name))
    {
    }

    void MarkDirty() override;
    void UpdateEntity() override;

private:
    bool bBeginCap = false;
    bool bEndCap = false;
    bool bMintorsion = false;
    bool bReverse = false;
    bool bCutBegin = false;
    bool bCutEnd = false;
};

}
