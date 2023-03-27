#pragma once
#include "Mesh.h"

namespace Nome::Scene
{

struct CSweepPathInfo
{
    std::vector<CVertexInfo *> Positions;
    std::vector<size_t> CrossSectionIndices;
    bool IsClosed;
    bool IsBSpline;
    std::vector<Vector3> BspTangents;
    std::string Name;
};

class CSweepPath : public CMesh
{
public:
    DECLARE_META_CLASS(CSweepPath, CMesh);

    CSweepPath() = default;
    explicit CSweepPath(std::string name)
        : CMesh(std::move(name))
    {
    }

    void UpdateEntity() override;

protected:
    CSweepPathInfo SI;
};

}