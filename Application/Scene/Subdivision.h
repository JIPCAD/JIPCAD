#include <opensubdiv/far/topologyRefinerFactory.h>
#include <opensubdiv/far/primvarRefiner.h>
#include <opensubdiv/far/topologyDescriptor.h>
#include <array>

///temp


using namespace OpenSubdiv;

Sdc::SchemeType SubdivisionType() {
    return Sdc::SCHEME_CATMARK;
}
Sdc::Options SubdivisionOptions()
{
    Sdc::Options options;

    options.SetVtxBoundaryInterpolation(
        Sdc::Options::VTX_BOUNDARY_EDGE_AND_CORNER
    );

    options.SetFVarLinearInterpolation(
        Sdc::Options::FVAR_LINEAR_NONE
    );

    return options;
}


Far::TopologyRefiner* GetRefiner(DSMesh& _m, bool isSharp)
{

    std::cout << "[OpenSubdiv] GetRefiner called. isSharp = "
          << isSharp << std::endl;

int incomingSharpEdges = 0;
for (auto* edge : _m.edges())
{
    if (edge && edge->sharpness > 0.0f)
    {
        incomingSharpEdges++;
        std::cout << "[OpenSubdiv] incoming sharp edge "
                  << edge->v0()->name << " - "
                  << edge->v1()->name
                  << " sharpness = " << edge->sharpness
                  << std::endl;
    }
}
int incomingSharpVertices = 0;

for (auto* vertex : _m.vertList)
{
    if (vertex && vertex->sharpness > 0.0f)
    {
        ++incomingSharpVertices;

        std::cout
            << "[OpenSubdiv] incoming sharp vertex "
            << vertex->name
            << " sharpness = "
            << vertex->sharpness
            << std::endl;
    }
}

std::cout
    << "[OpenSubdiv] incoming sharp vertex count = "
    << incomingSharpVertices
    << std::endl;

std::cout << "[OpenSubdiv] incoming sharp edge count = "
          << incomingSharpEdges << std::endl;
    typedef Far::TopologyDescriptor Descriptor;
    Descriptor desc;

    desc.numVertices = (int)_m.vertList.size();
    desc.numFaces = (int)_m.faceList.size();

    // map Vertex* -> contiguous index
    std::unordered_map<Vertex*, int> vIndex;
    vIndex.reserve(_m.vertList.size());
    for (int i = 0; i < (int)_m.vertList.size(); ++i)
        vIndex[_m.vertList[i]] = i;

    // FIX: Use std::vector to automatically manage memory and prevent massive memory leaks!
    std::vector<int> faceVerts(desc.numFaces);
    std::vector<int> faceVertsIndices;

    for (int f = 0; f < desc.numFaces; ++f)
    {
        int nv = (int)_m.faceList[f]->vertices.size();
        faceVerts[f] = nv;
        for (auto* v : _m.faceList[f]->vertices)
        {
            faceVertsIndices.push_back(vIndex[v]);
        }
    }

    desc.numVertsPerFace = faceVerts.data();
    desc.vertIndicesPerFace = faceVertsIndices.data();

    std::vector<float> cornerWeights;
    std::vector<int> cornerIndices;
    std::vector<float> creaseWeights;
    std::vector<int> creaseIndices;

    if (isSharp)
    {
        desc.numCorners = _m.n_vertices();
        cornerWeights.resize(desc.numCorners);
        cornerIndices.resize(desc.numCorners);

        int cornerCount = 0;
        for (auto v_itr : _m.vertList)
        {
            cornerWeights[cornerCount] = v_itr->sharpness;
            // FIX: Use the contiguous vIndex, NOT the unsafe ID!
            cornerIndices[cornerCount] = vIndex[v_itr];
            cornerCount++;
        }
        desc.cornerWeights = cornerWeights.data();
        desc.cornerVertexIndices = cornerIndices.data();

        desc.numCreases = _m.n_edges();
creaseWeights.resize(desc.numCreases);
creaseIndices.resize(desc.numCreases * 2);

int creaseCount = 0;
for (auto edge : _m.edges())
{
    creaseWeights[creaseCount] = edge->sharpness;
    creaseIndices[creaseCount * 2] = vIndex[edge->v0()];
    creaseIndices[creaseCount * 2 + 1] = vIndex[edge->v1()];
    creaseCount++;
}
desc.creaseVertexIndexPairs = creaseIndices.data();
desc.creaseWeights = creaseWeights.data();
    }

    return Far::TopologyRefinerFactory<Descriptor>::Create(
        desc,
        Far::TopologyRefinerFactory<Descriptor>::Options(SubdivisionType(), SubdivisionOptions()));
}
