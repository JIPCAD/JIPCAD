#pragma once
#include "Mesh.h"
#include <unordered_map>

namespace Nome::Scene
{

class COffsetRefiner
{
public:
    COffsetRefiner(CMeshImpl& _m, bool offsetFlag);
    void Refine(float height, float width);
    std::vector<Vector3> GetVertices() { return offsetVertices; };
    std::vector<std::vector<int>> GetFaces() { return offsetFaces; };

private:
    struct OffsetVerticesInfo {
        Vector3 topPos;
        int topIndex;
        Vector3 bottomPos;
        int bottomIndex;
    };

    void generateNewVertices(OpenMesh::SmartVertexHandle vertex, float height);
    void generateNewFaceVertices(OpenMesh::SmartFaceHandle face, float width);
    void generateNewFaces(OpenMesh::SmartFaceHandle face);
    void closeFace(OpenMesh::SmartFaceHandle face);

    Vector3 getPosition(OpenMesh::SmartVertexHandle vertex);
    Vector3 getEdgeVector(OpenMesh::SmartEdgeHandle edge, OpenMesh::SmartVertexHandle vertex);

    float getAngle(Vector3 vectorA, Vector3 vectorB);
    Vector3 crossProduct(Vector3 vectorA, Vector3 vectorB);

    std::vector<int> faceVertices;
    std::vector<int> vertexEdges;
    std::vector<std::unordered_map<int, OffsetVerticesInfo>> newFaceVertices;
    std::vector<OffsetVerticesInfo> newVertices;
    CMeshImpl Mesh;
    bool flag;

    std::vector<Vector3> offsetVertices;
    std::vector<std::vector<int>> offsetFaces;


    std::vector<OpenMesh::SmartVertexHandle> newVertexHandleList;
    void addPoint(Vector3 vector);

};

}

