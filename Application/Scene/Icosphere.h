#pragma once

#include "Mesh.h"

namespace Nome::Scene
{

class CIcosphere : public CMesh
{
    DEFINE_INPUT(float, Scale) { MarkDirty(); }
    DEFINE_INPUT(float, SubdivisionSteps) { MarkDirty(); }

public:
    DECLARE_META_CLASS(CIcosphere, CMesh);
    CIcosphere() = default;
    CIcosphere(const std::string& name)
        : CMesh(std::move(name))
    {
    }

    void UpdateEntity() override;

//private:
  //  bool bInvert = false;
};

}
