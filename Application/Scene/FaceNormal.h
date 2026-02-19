#pragma once
#include "Mesh.h"

namespace Nome::Scene
{

class CFaceNormal : public CMesh
{
    DEFINE_INPUT(float, Segments) { MarkDirty(); }
    DEFINE_INPUT_ARRAY(CMeshInstance*, Meshes) { MarkDirty(); }

public:
    DECLARE_META_CLASS(CFaceNormal, CMesh);
    CFaceNormal() = default;
    explicit CFaceNormal(const std::string& name)
        : CMesh(name)
    {
    }
    void UpdateEntity() override;
    void MarkDirty() override;
};
}