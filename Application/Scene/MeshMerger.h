#pragma once
#include "Mesh.h"
#include <LangUtils.h>
#include <QString>
#include <iostream>
#include <fstream>

#include <cstdio>

namespace Nome::Scene
{

class CMeshMerger : public CMesh
{
    DEFINE_INPUT(float, Level) { MarkDirty(); }
    DEFINE_INPUT(float, Height) { MarkDirty(); }
    DEFINE_INPUT(float, Width) { MarkDirty(); }

    void UpdateEntity() override;
    // void MarkDirty() override;


public:
    DECLARE_META_CLASS(CMeshMerger, CEntity);

    CMeshMerger() = default;
    explicit CMeshMerger(const std::string& name)
        : CMesh(name)
    {
    }

    // No update yet, please just use one time
    void MergeIn(CMeshInstance& meshInstance, bool shouldMergePoints);

    void MergeClear();

    bool offset(DSMesh& _m);

    bool offset(DSMesh& _m, double height, double width);

    // sd_flag can be set to sharp and plain cc to have different types of subdivision
    void Catmull();
    void Shell(std::string f);
    void doShell(DSMesh& _m, Face* f);

    bool subdivide(DSMesh& _m, unsigned int n);

    void setSubLevel(int level) {
        subdivisionLevel = level;
    }

    void setOffset(bool o) { 
        offsetIdent = o;
    }

    
    void setOffsetHeightWidth(double height, double width)
    {
        h = height;
        w = width;
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

private:
    DSMesh MergedMesh;
    std::pair<Vertex*, float> FindClosestVertex(const tc::Vector3& pos);

    unsigned int VertCount = 0;
    unsigned int FaceCount = 0;
    double shellH = 0.1f, shellW = 0.1f;


    OpenMesh::VPropHandleT<CMeshImpl::Point> vp_pos_; // next vertex pos
    OpenMesh::EPropHandleT<CMeshImpl::Point> ep_pos_; // new edge pts
    OpenMesh::FPropHandleT<CMeshImpl::Point> fp_pos_; // new face pts

    /// add ccsubdivision to subdivide non-manifold shapes
    void setAllNewPointPointersToNull();
    void compileNewMesh(std::vector<Face*>& newFaceList);
    void makeVertexPointsD(std::vector<Vertex*>& newVertList);
    void makeEdgePoints(std::vector<Vertex*>& newVertList);
    void makeFacePoints(std::vector<Vertex*>& newVertList);
    void ccSubdivision(int level);

    //CMeshImpl MergedMesh;
    unsigned int subdivisionLevel = 0;
    double h = 0.0f, w = 0.0f;
    bool offsetIdent = false;
    bool isSharp = true;
    // true == NOME_OFFSET_DEFAULT, false == NOME_OFFSET_GRID
    bool offsetFlag = true;

};
}

