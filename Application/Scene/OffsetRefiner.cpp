#include "OffsetRefiner.h"
#undef M_PI

namespace Nome::Scene
{

COffsetRefiner::COffsetRefiner(CMeshImpl& _m, bool offsetFlag) {

    Mesh = _m;
    flag = offsetFlag;

    size_t numFaces = Mesh.n_faces();
    faceVertices.resize(numFaces);
    newFaceVertices.resize(numFaces);
    for (auto face : Mesh.faces()) {
        int i = 0;
        for (auto vertex : face.vertices())
            i++;
        faceVertices[face.idx()] = i;
    }

    size_t numVertices = Mesh.n_vertices();
    vertexEdges.resize(numVertices);
    newVertices.resize(numVertices);
    for (auto vertex : Mesh.vertices()) {
        int i = 0;
        for (auto edge : vertex.edges())
            i++;
        vertexEdges[vertex.idx()] = i;
    }
}

void COffsetRefiner::Refine(float height, float width) {
    for (auto vertex : Mesh.vertices()) {
        if (vertexEdges[vertex.idx()] < 2) { continue; }
        generateNewVertices(vertex, height);
    }

    auto faces = Mesh.faces().to_vector();
    for (auto face : faces) {
        generateNewFaceVertices(face, width);
        generateNewFaces(face);
    }

    for (auto face : faces) {
        closeFace(face);
    }
}

void COffsetRefiner::generateNewVertices(OpenMesh::SmartVertexHandle vertex, float height) {
    Vector3 point = getPosition(vertex);
    Vector3 sumEdges;

    if (vertexEdges[vertex.idx()] > 2) {
        for (auto edge : vertex.edges()) {
            sumEdges += getEdgeVector(edge, vertex).Normalized();
        }
    } else {
        std::vector<OpenMesh::SmartEdgeHandle> edges = vertex.edges().to_vector();
        Vector3 edge1 = getEdgeVector(edges[0], vertex);
        Vector3 edge2 = getEdgeVector(edges[1], vertex);

        sumEdges = crossProduct(edge1, edge2);
    }

    sumEdges.Normalize();
    sumEdges *= height / 2;

    Vector3 newPoint1 = point - sumEdges;
    Vector3 newPoint2 = point + sumEdges;

    addPoint(newPoint1);
    addPoint(newPoint2);

    int size = offsetVertices.size();
    newVertices[vertex.idx()] = OffsetVerticesInfo{newPoint1, size - 2, newPoint2, size - 1};
}

void COffsetRefiner::generateNewFaceVertices(OpenMesh::SmartFaceHandle face, float width) {

    size_t length = faceVertices[face.idx()];

    std::vector<Vector3> facePoints;
    std::vector<int> idxList;
    for (auto vertex : face.vertices()) {
        facePoints.push_back(getPosition(vertex));
        idxList.push_back(vertex.idx());
    }

    for (size_t index = 0; index < length; index++) {
        Vector3 curPoint = facePoints[index];
        Vector3 prevPoint = facePoints[(index - 1 + length) % length];
        Vector3 nextPoint = facePoints[(index + 1) % length];

        Vector3 prevPath = prevPoint - curPoint;
        Vector3 curPath = nextPoint - curPoint;

        if (flag) {
            prevPath.Normalize();
            curPath.Normalize();
        }

        float angle = getAngle(prevPath, curPath);
        Vector3 offsetVector = (prevPath + curPath).Normalized() * width / sinf(angle / 2);

        int idx = idxList[index];
        auto newVerticesInfo = newVertices[idx];
        Vector3 newPoint1 = newVerticesInfo.topPos + offsetVector;
        Vector3 newPoint2 = newVerticesInfo.bottomPos + offsetVector;

        addPoint(newPoint1);
        addPoint(newPoint2);

        int size = offsetVertices.size();
        newFaceVertices[face.idx()][idx] = OffsetVerticesInfo{newPoint1, size - 2, newPoint2, size - 1};
    }
}

void COffsetRefiner::generateNewFaces(OpenMesh::SmartFaceHandle face) {
    int faceId = face.idx();

    std::vector<int> idxList;
    for (auto vertex : face.vertices())
        idxList.push_back(vertex.idx());

    for (size_t i = 0; i < idxList.size(); i++) {
        int vertex1Id = idxList[i];
        int vertex2Id = idxList[(i + 1) % idxList.size()];

        int vertex1TopIndex = newVertices[vertex1Id].topIndex;
        int vertex1BottomIndex = newVertices[vertex1Id].bottomIndex;
        int vertex1TopInsideIndex = newFaceVertices[faceId][vertex1Id].topIndex;
        int vertex1BottomInsideIndex = newFaceVertices[faceId][vertex1Id].bottomIndex;

        int vertex2TopIndex = newVertices[vertex2Id].topIndex;
        int vertex2BottomIndex = newVertices[vertex2Id].bottomIndex;
        int vertex2TopInsideIndex = newFaceVertices[faceId][vertex2Id].topIndex;
        int vertex2BottomInsideIndex = newFaceVertices[faceId][vertex2Id].bottomIndex;

        std::vector<int> faceIndexList;
        faceIndexList = { vertex1TopInsideIndex, vertex1TopIndex,  vertex2TopIndex, vertex2TopInsideIndex, };
        offsetFaces.push_back(faceIndexList);
        Mesh.add_face(
            newVertexHandleList[vertex1TopInsideIndex],
            newVertexHandleList[vertex1TopIndex],
            newVertexHandleList[vertex2TopIndex],
            newVertexHandleList[vertex2TopInsideIndex]
        );

        faceIndexList = { vertex1BottomInsideIndex, vertex1TopInsideIndex, vertex2TopInsideIndex, vertex2BottomInsideIndex };
        offsetFaces.push_back(faceIndexList);
        Mesh.add_face(
        newVertexHandleList[vertex1BottomInsideIndex],
            newVertexHandleList[vertex1TopInsideIndex],
            newVertexHandleList[vertex2TopInsideIndex],
            newVertexHandleList[vertex2BottomInsideIndex]
        );

        faceIndexList = { vertex1BottomIndex, vertex1BottomInsideIndex, vertex2BottomInsideIndex, vertex2BottomIndex };
        offsetFaces.push_back(faceIndexList);
        Mesh.add_face(
        newVertexHandleList[vertex1BottomIndex],
            newVertexHandleList[vertex1BottomInsideIndex],
            newVertexHandleList[vertex2BottomInsideIndex],
            newVertexHandleList[vertex2BottomInsideIndex]
        );
    }

}

void COffsetRefiner::closeFace(OpenMesh::SmartFaceHandle face) {
    std::vector<int> idxList;
    for (auto vertex : face.vertices())
        idxList.push_back(vertex.idx());

    for (size_t i = 0; i < idxList.size(); i++) {
        int vertex1Id = idxList[i];
        int vertex2Id = idxList[(i + 1) % idxList.size()];

        int vertex1TopIndex = newVertices[vertex1Id].topIndex;
        int vertex1BottomIndex = newVertices[vertex1Id].bottomIndex;
        int vertex2TopIndex = newVertices[vertex2Id].topIndex;
        int vertex2BottomIndex = newVertices[vertex2Id].bottomIndex;


        OpenMesh::SmartVertexHandle vertex1Top = newVertexHandleList[vertex1TopIndex];
        OpenMesh::SmartVertexHandle vertex1Bottom = newVertexHandleList[vertex1BottomIndex];
        OpenMesh::SmartVertexHandle vertex2Top = newVertexHandleList[vertex2TopIndex];
        OpenMesh::SmartVertexHandle vertex2Bottom = newVertexHandleList[vertex2BottomIndex];

        if (vertex1Top.is_boundary() && vertex1Bottom.is_boundary() && vertex2Top.is_boundary() && vertex2Bottom.is_boundary()) {
            Mesh.add_face(vertex1Top, vertex1Bottom, vertex2Bottom, vertex2Top);
            std::vector<int> faceIndexList = { vertex1TopIndex, vertex1BottomIndex, vertex2BottomIndex, vertex2TopIndex };
            offsetFaces.push_back(faceIndexList);
        }
    }
}

Vector3  COffsetRefiner::getPosition(OpenMesh::SmartVertexHandle vertex) {
    const auto& pos = Mesh.point(vertex);
    return Vector3(pos[0], pos[1], pos[2]);
}

Vector3 COffsetRefiner::getEdgeVector(OpenMesh::SmartEdgeHandle edge, OpenMesh::SmartVertexHandle vertex) {
    Vector3 tempPoint;
    if (edge.v0() == vertex) {
        tempPoint = getPosition(edge.v1());
    } else {
        tempPoint = getPosition(edge.v0());
    }

    return tempPoint - getPosition(vertex);
}

Vector3 COffsetRefiner::crossProduct(Vector3 vectorA, Vector3 vectorB)
{
    return Vector3(vectorA.y * vectorB.z - vectorA.z * vectorB.y,
                   vectorA.z * vectorB.x - vectorA.x * vectorB.z,
                   vectorA.x * vectorB.y - vectorA.y * vectorB.x);
}

float COffsetRefiner::getAngle(Vector3 vectorA, Vector3 vectorB) {
    float value = vectorA.DotProduct(vectorB) / vectorA.Length() / vectorB.Length();
    float epsilon = 1e-4;
    if (fabs(value - 1) < epsilon) { return 0; }
    if (fabs(value + 1) < epsilon) { return (float)tc::M_PI; }
    if (fabs(value) < epsilon) {return (float)tc::M_PI / 2; }

    return acosf(vectorA.DotProduct(vectorB) / vectorA.Length() / vectorB.Length()) ;
}

void COffsetRefiner::addPoint(Vector3 vector) {
    newVertexHandleList.push_back(Mesh.add_vertex(CMeshImpl::Point(vector.x, vector.y, vector.z)));
    offsetVertices.push_back(vector);
}

}
