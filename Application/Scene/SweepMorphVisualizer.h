#pragma once
#include "Mesh.h"
#include "SweepPath.h"
#include "Sweep.h"
#include "Vector3.h"
#include "SweepMorph.h"

namespace Nome::Scene
{

class CSweepMorphVisualizer : public CMesh
{
    DEFINE_INPUT(CSweep *, Sweep) { MarkDirty(); }
    DEFINE_INPUT(float, MorphIndex) { MarkDirty(); }

public:
    DECLARE_META_CLASS(CSweepMorphVisualizer, CMesh);

    CSweepMorphVisualizer() = default;
    explicit CSweepMorphVisualizer(const std::string& name)
        : CMesh(std::move(name))
    {
    }

    void UpdateEntity() override;

};

}
