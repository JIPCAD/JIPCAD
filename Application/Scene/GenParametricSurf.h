#pragma once
#include "Mesh.h"

namespace Nome::Scene
{

class CGenParametricSurf : public CMesh
{
    DEFINE_INPUT(float, u_start) { MarkDirty(); }
    DEFINE_INPUT(float, u_end) { MarkDirty(); }
    DEFINE_INPUT(float, v_start) { MarkDirty(); }
    DEFINE_INPUT(float, v_end) { MarkDirty(); }
    DEFINE_INPUT(float, u_segs) { MarkDirty(); }
    DEFINE_INPUT(float, v_segs) { MarkDirty(); }

public:
    DECLARE_META_CLASS(CGenParametricSurf, CMesh);
    CGenParametricSurf() = default;
    CGenParametricSurf(const std::string& name)
        : CMesh(std::move(name))
    {
    }

    void UpdateEntity() override;


private:
    //..
};

}
