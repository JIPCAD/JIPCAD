#pragma once
#include "Mesh.h"
#include <LangUtils.h>
#include <QString>
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <unordered_map>
#include <vector>

namespace Nome::Scene
{

// 1. Spatial Hash Grid keys and hash definition
struct SpatialCellKey
{
    int x, y, z;
    bool operator==(const SpatialCellKey& o) const { return x == o.x && y == o.y && z == o.z; }
};

struct SpatialCellHash
{
    size_t operator()(const SpatialCellKey& k) const
    {
        return ((std::hash<int>()(k.x) ^ (std::hash<int>()(k.y) << 1)) >> 1)
            ^ (std::hash<int>()(k.z) << 1);
    }
};

// 2. SpatialHashGrid definition MUST precede CMeshMerger
struct SpatialHashGrid
{
    float cellSize;
    std::unordered_map<SpatialCellKey, std::vector<Vertex*>, SpatialCellHash> grid;

    SpatialHashGrid(float size = 1e-4f)
        : cellSize(std::max(size, 1e-6f))
    {
    }

    SpatialCellKey getCellKey(const Vector3& pos) const
    {
        return { static_cast<int>(std::floor(pos.x / cellSize)),
                 static_cast<int>(std::floor(pos.y / cellSize)),
                 static_cast<int>(std::floor(pos.z / cellSize)) };
    }

    void insert(Vertex* v)
    {
        if (!v)
            return;
        grid[getCellKey(v->position)].push_back(v);
    }

    std::pair<Vertex*, float> findClosest(const Vector3& pos, float maxDist) const
    {
        SpatialCellKey baseKey = getCellKey(pos);
        Vertex* closest = nullptr;
        float minDistSq = maxDist * maxDist;

        for (int dx = -1; dx <= 1; ++dx)
        {
            for (int dy = -1; dy <= 1; ++dy)
            {
                for (int dz = -1; dz <= 1; ++dz)
                {
                    SpatialCellKey key = { baseKey.x + dx, baseKey.y + dy, baseKey.z + dz };
                    auto it = grid.find(key);
                    if (it != grid.end())
                    {
                        for (Vertex* v : it->second)
                        {
                            Vector3 diff = v->position - pos;
                            float distSq = diff.x * diff.x + diff.y * diff.y + diff.z * diff.z;
                            if (distSq < minDistSq)
                            {
                                minDistSq = distSq;
                                closest = v;
                            }
                        }
                    }
                }
            }
        }
        return { closest, std::sqrt(minDistSq) };
    }
};

class CMeshMerger : public CMesh
{
    DEFINE_INPUT(float, Level) { MarkDirty(); }
    DEFINE_INPUT(float, Height) { MarkDirty(); }
    DEFINE_INPUT(float, Width) { MarkDirty(); }

    void UpdateEntity() override;

public:
    DECLARE_META_CLASS(CMeshMerger, CEntity);

    CMeshMerger() = default;

    explicit CMeshMerger(const std::string& name)
        : CMesh(name)
    {
    }

    void FinalizeMerge();
    void MergeIn(CMeshInstance& meshInstance, bool shouldMergePoints);
    void MergeClear();

    bool offset(DSMesh& _m);
    bool offset(DSMesh& _m, double height, double width, std::string outerRimSurface,
                std::string innerRimSurface, bool outerRimHidden, bool innerRimHidden);

    void Catmull();
    void Catmull(CMeshInstance& meshInstance, bool shouldMergePoints);
    void Catmull2(CMeshInstance& meshInstance, bool shouldMergePoints);

    void Shell(std::string f);
    void doShell(DSMesh& _m, Face* f);

    bool subdivide(DSMesh& _m, unsigned int n);

    void setSubLevel(int level) { subdivisionLevel = level; }
    void setOffset(bool o) { offsetIdent = o; }

    void setOffsetHeightWidth(double height, double width)
    {
        h = height;
        w = width;
    }

    void setOffsetRims(std::string outerRimSurface, std::string innerRimSurface,
                       bool outerRimHidden, bool innerRimHidden)
    {
        _outerRimSurface = outerRimSurface;
        _innerRimSurface = innerRimSurface;
        _outerRimHidden = outerRimHidden;
        _innerRimHidden = innerRimHidden;
    }

    void setShellHeightWidth(double height, double width)
    {
        shellH = height;
        shellW = width;
    }

    void SetSharp(bool setSharp) { isSharp = setSharp; }
    void SetOffsetFlag(bool flag) { offsetFlag = flag; }

    void ExportAsStl(QString filename);
    std::vector<std::string> splitString(const std::string& str, const char delim);

    void CreateNormals(DSMesh& ds, bool faceNormals, float faceNormalMultiplier, bool vertexNormals,
                       float vertexNormalMultiplier);
    void CreateNormalsCurr(bool faceNormals, float faceNormalMultiplier, bool vertexNormals,
                           float vertexNormalMultiplier);
    DSMesh getCurrMesh();
    tc::Matrix3x4 getMergedMeshTf();
    void MergeCurr();
    void changeColors(std::string surfaceName, std::string backfaceName);
    std::shared_ptr<CMeshInstance> currMeshInstance;
    Nome::Scene::CSceneTreeNode* treeNode;
    DSMesh& GetDSMesh() { return currMesh; }

    void ResetSpatialGrid(float size) { spatialGrid = SpatialHashGrid(size); }

private:
    SpatialHashGrid spatialGrid; // Uses default constructor SpatialHashGrid(1e-4f)
    DSMesh MergedMesh;
    std::pair<Vertex*, float> FindClosestVertex(const tc::Vector3& pos);
    tc::Vector3 calculate3PlaneIntersection(tc::Vector3 p, tc::Vector3 n1, tc::Vector3 n2,
                                            tc::Vector3 n3, double offsetDistance);
    unsigned int VertCount = 0;
    unsigned int FaceCount = 0;
    double shellH = 0.1f, shellW = 0.1f;

    OpenMesh::VPropHandleT<CMeshImpl::Point> vp_pos_;
    OpenMesh::EPropHandleT<CMeshImpl::Point> ep_pos_;
    OpenMesh::FPropHandleT<CMeshImpl::Point> fp_pos_;

    void setAllNewPointPointersToNull();
    void compileNewMesh(std::vector<Face*>& newFaceList);
    void makeVertexPointsD(std::vector<Vertex*>& newVertList);
    void makeEdgePoints(std::vector<Vertex*>& newVertList);
    void makeFacePoints(std::vector<Vertex*>& newVertList);
    void ccSubdivision(int level);

    unsigned int subdivisionLevel = 0;
    double h = 0.0f, w = 0.0f;
    std::string _outerRimSurface;
    std::string _innerRimSurface;
    bool _outerRimHidden;
    bool _innerRimHidden;
    bool offsetIdent = false;
    bool isSharp = true;
    bool offsetFlag = true;
    tc::Matrix3x4 MergedMeshTf;
};
}