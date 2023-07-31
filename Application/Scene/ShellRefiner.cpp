#include "ShellRefiner.h"
#undef M_PI

namespace Nome::Scene
{
CShellRefiner::CShellRefiner(DSMesh& _m)
{
    // New mesh
    currMesh = _m.newMakeCopy();

    // Aaron's code. This code is intended for the deletion of the Face object from the mesh, but
    // this is not clean. Needs to delete from the Surface node.

    /* targetFace = target;
    for (auto faceAdd = currMesh.faceList.begin(); faceAdd < currMesh.faceList.end(); faceAdd++)
    {
        Face* face = *faceAdd;
        std::string name = face->name;
        if (face->name == targetFace->name)
        {
            currMesh.deleteFace(targetFace);
            break;
        }
    } */
    size_t num_faces = currMesh.faceList.size(); // Mesh.faces

    faceVertices.resize(num_faces);
    newFaceVertices.resize(num_faces);

    for (auto face : currMesh.faceList)
    {
        int i = 0;
        for (auto vertex : face->vertices)
        {
            i++;
        }
        faceVertices[face->id] = i;
    }
    // Mesh.request_face_normals();

    size_t numVertices = currMesh.vertList.size();
    vertexEdges.resize(numVertices);
    newVertices.resize(numVertices);

    for (auto& pair : currMesh.randyedgeTable)
    {
        Vertex* currVert = pair.first;
        std::vector<Edge*> vertEdge = pair.second;
        vertexEdges[currVert->ID] = vertEdge.size();
    }
}
void CShellRefiner::Refine(float shellHeight, float shellWidth)
{
    bool needGrid = (shellWidth > 0);
    bool needShell = (shellHeight > 0);

    auto vertSize = currMesh.vertList.size();

    for (int i = 0; i < vertSize; i++)
    {
        auto vertex = currMesh.vertList[i];
        if (vertexEdges[vertex->ID] < 2)
        {
            continue;
        }
        generateNewVertices(vertex, shellHeight);
    }
    auto faces = currMesh.faceList; // Mesh.faces().to_vector()
    for (auto face : faces)
    {
        if (needGrid)
        {
            // width is used to scale the vertex offset.
            generateNewFacesVertices(face, shellHeight, shellWidth);
        }
        generateNewFaces(face, needGrid, needShell);
        //pushBackFaces(face);
    }
    if (needShell)
    {
        for (auto face : faces)
        {
            closeFace(face);
        }
    }
    currMesh.buildBoundary();
    currMesh.computeNormals();
}

void CShellRefiner::generateNewVertices(Vertex* vertex, float height)
{
    Vector3 point = vertex->position; // getPosition(vertex)

    if (height <= 0)
    {
        addPoint(point);
        newVertices[vertex->ID] = ShellVerticesInfo { vertex, (int)shellVertices.size() - 1 };
        return;
    }

    Vector3 sumEdges;
    if (vertexEdges[vertex->ID] > 2)
    {
        auto edges = currMesh.randyedgeTable[vertex];
        for (auto edge : edges)
        {
            // sumEdges is the sum of the normalized vectors of each edge
            // from each vertex
            sumEdges += getEdgeVector(edge, vertex).Normalized();
        }
    }
    else
    {
        // either smaller or equal to presumably
        std::vector<Edge*> edges = currMesh.randyedgeTable[vertex];

        Vector3 edge1;
        Vector3 edge2;

        // what is this supposed to be doing?
        bool forwardDir = (edges[1]->edge_ID - edges[0]->edge_ID) == 1;

        if (forwardDir)
        {
            std::cout << "1st edge of vert: " << edges[0]->edge_ID << std::endl;
            std::cout << "2nd edge of this vert: " << edges[1]->edge_ID << std::endl;
            edge1 = getEdgeVector(edges[0], vertex);
            edge2 = getEdgeVector(edges[1], vertex);
        }
        else // beginning vertex.
        {
            std::cout << "1st edge of vert: " << edges[1]->edge_ID << std::endl;
            std::cout << "2nd edge of vert: " << edges[0]->edge_ID << std::endl;
            edge1 = getEdgeVector(edges[1], vertex);
            edge2 = getEdgeVector(edges[0], vertex);
        }
        sumEdges = crossProduct(edge1, edge2);
    }
    sumEdges.Normalize();
    sumEdges *= height;

    Vector3 newPoint1 = point;
    Vector3 newPoint2 = point + sumEdges;
    Vertex* newVert1 = addPoint(newPoint1);
    Vertex* newVert2 = addPoint(newPoint2);
    int size = shellVertices.size();
    newVertices[vertex->ID] = ShellVerticesInfo { newVert1, size - 2, newVert2, size - 1 };
}
Vector3 CShellRefiner::crossProduct(Vector3 vectorA, Vector3 vectorB)
{
    return Vector3(vectorA.y * vectorB.z - vectorA.z * vectorB.y,
                   vectorA.z * vectorB.x - vectorA.x * vectorB.z,
                   vectorA.x * vectorB.y - vectorA.y * vectorB.x);
}

void CShellRefiner::generateNewFacesVertices(Face* face, float height, float width)
{
    size_t length = faceVertices[face->id];

    std::vector<Vector3> facePoints;
    std::vector<int> idxList;
    for (auto vertex : face->vertices)
    {
        facePoints.push_back(vertex->position); // getPosition(vertex));
        idxList.push_back(vertex->ID);
    }

    for (size_t index = 0; index < length; index++)
    {
        Vector3 curPoint = facePoints[index];
        Vector3 prevPoint = facePoints[(index - 1 + length) % length];
        Vector3 nextPoint = facePoints[(index + 1) % length];

        Vector3 prevPath = prevPoint - curPoint;
        Vector3 curPath = nextPoint - curPoint;

        if (flag)
        {
            // normalise the prev path and curr path
            prevPath.Normalize();
            curPath.Normalize();
        }

        float angle = getAngle(prevPath, curPath);
        // the offset vector is the average of the two edges.
        Vector3 offsetVector = (prevPath + curPath).Normalized() * width / sinf(angle / 2);

        Vector3 norm = crossProduct(prevPath, curPath).Normalized() * height / 2;

        Vector3 newPoint1 = curPoint + offsetVector - norm;
        Vector3 newPoint2 = curPoint + offsetVector + norm;

        Vertex* newVert1 = addPoint(newPoint1);
        Vertex* newVert2 = addPoint(newPoint2);
        int size = shellVertices.size();
        newFaceVertices[face->id][idxList[index]] =
            ShellVerticesInfo { newVert1, size - 2, newVert2,
                                size - 1 }; // Randy changed this to be new Vert
    }
}

void CShellRefiner::closeFace(Face* face)
{
    std::vector<int> idxList;
    for (auto vertex : face->vertices)
        idxList.push_back(vertex->ID);

    for (size_t i = 0; i < idxList.size(); i++)
    {
        int vertex1Id = idxList[i];
        int vertex2Id = idxList[(i + 1) % idxList.size()];

        int vertex1TopIndex = newVertices[vertex1Id].topIndex;
        int vertex1CurrIndex = newVertices[vertex1Id].currIndex;
        int vertex2TopIndex = newVertices[vertex2Id].topIndex;
        int vertex2CurrIndex = newVertices[vertex2Id].currIndex;

        Vertex* vertex1Top = newVertexList[vertex1TopIndex];
        Vertex* vertex1Curr = newVertexList[vertex1CurrIndex];
        Vertex* vertex2Top = newVertexList[vertex2TopIndex];
        Vertex* vertex2Curr = newVertexList[vertex2CurrIndex];

        std::vector<Edge*> boundaryEdgeList1 =
            currMesh.boundaryEdgeList(); // Randy added this. needed to check if vert is in boundary

        // check to see if the vertex is on an edge that is on a boundary.. In openMesh,
        // is_boundary() checked if a vertex is adjacent to a boundary edge

        // boundary edge -- an edge that only is adjacent to one face, i.e. face b is null.

        std::vector<Vertex*> verts = { vertex1Top, vertex1Curr, vertex2Top, vertex2Curr };
        std::vector<Edge*> boundaryList = currMesh.boundaryEdgeList();
        bool allOnBoundary = true;
        for (auto vert : verts)
        {
            bool onBoundary = false;
            std::vector<Edge*> boundaryList = currMesh.boundaryEdgeList();
            std::vector<Edge*> adjEdges = currMesh.randyedgeTable[vertex1Top];
            for (auto adjEdge : adjEdges)
            {
                if (std::find(boundaryList.begin(), boundaryList.end(), adjEdge)
                    != boundaryList.end())
                {
                    // just need one of the vert's edges to be onboundary
                    onBoundary = true;
                }
            }
            allOnBoundary = allOnBoundary && onBoundary;
        }

        // Aaron's edit. Basically, if all edges are on the Boundary
        // i.e. if all edges are not part of any face, then the new face is created.
        if (allOnBoundary)
        {
            // currMesh.addPolygonFace({ vertex1Top, vertex1Curr, vertex2Curr, vertex2Top
            // });

            // std::vector<int> faceIndexList = { vertex1TopIndex, vertex1CurrIndex,
            //                                    vertex2CurrIndex, vertex2TopIndex };
            // offsetFaces.push_back(faceIndexList);

            // Randy added below to replace above lines
            Face* offsetFaceClose =
                currMesh.addFace({ vertex1Top, vertex1Curr, vertex2Curr, vertex2Top });
            shellFaces.push_back(offsetFaceClose);
        }
        // if (vertex1Top.is_boundary() && vertex1Curr.is_boundary() &&
        // vertex2Top.is_boundary()
        //    && vertex2Curr.is_boundary())
        //{
        //    Mesh.add_face(vertex1Top, vertex1Curr, vertex2Curr, vertex2Top);
        //    std::vector<int> faceIndexList = { vertex1TopIndex, vertex1CurrIndex,
        //                                       vertex2CurrIndex, vertex2TopIndex };
        //    offsetFaces.push_back(faceIndexList);
        //}
    }
}
// Get vector representing a certain edge based on the edge and a given vertex
Vector3 CShellRefiner::getEdgeVector(Edge* edge, Vertex* vertex)
{
    Vector3 tempPoint;
    if (edge->va == vertex) // if (edge.v0() == vertex)
    {
        tempPoint = edge->vb->position; // getPosition(edge.v1());
    }
    else
    {
        tempPoint = edge->va->position; // getPosition(edge.v0());
    }

    return tempPoint - vertex->position; // getPosition(vertex);
}

void CShellRefiner::generateNewFaces(Face* face, bool needGrid, bool needShell)
{
    std::vector<int> indexList1, indexList2;
    std::vector<Vertex*> vertices1, vertices2;
    for (auto vertex : face->vertices)
    {
        int index = vertex->ID;

        int topIndex = newVertices[index].topIndex;
        int currIndex = newVertices[index].currIndex;
        indexList1.push_back(topIndex);
        indexList2.push_back(currIndex);
        vertices1.push_back(newVertexList[topIndex]);
        vertices2.push_back(newVertexList[currIndex]);
    }

    std::reverse(indexList2.begin(), indexList2.end());
    std::reverse(vertices2.begin(), vertices2.end());

    // shellFaces.push_back(indexList1);
    // shellFaces.push_back(indexList2);

    // Randy changed above line to the following
    Face* shellFace1 = new Face(vertices1); // takes in a vector of Vertex objects
    Face* shellFace2 = new Face(vertices2); // takes in a vector of Vertex objects
    shellFaces.push_back(shellFace1);
    shellFaces.push_back(shellFace2);

    currMesh.addFace(vertices1);
    currMesh.addFace(vertices2);
    if (!needGrid)
    {
        return;
    }
    int faceId = face->id;
    std::vector<int> idxList;

    for (auto vertex : face->vertices)
        idxList.push_back(vertex->ID);

    for (size_t i = 0; i < idxList.size(); i++)
    {

        int vertex1Id = idxList[i];
        int vertex2Id = idxList[(i + 1) % (idxList.size())];
        // Randy added below lines to hopefully replace above lines
        Vertex* vertex1Top = newVertices[vertex1Id].topVert;
        Vertex* vertex1TopInside = newFaceVertices[faceId][vertex1Id].topVert;
        Vertex* vertex2Top = newVertices[vertex2Id].topVert;
        Vertex* vertex2TopInside = newFaceVertices[faceId][vertex2Id].topVert;

        // std::vector<int> faceIndexList;
        // faceIndexList = {
        //    vertex1TopInsideIndex,
        //    vertex1TopIndex, // Randy the bug is here, for some reason always 0
        //    vertex2TopIndex, // Randy the bug is here, for some reason always 0
        //    vertex2TopInsideIndex,
        //};
        // shellFaces.push_back(faceIndexList);

        // Randy below line to hopefully replace above lines
        Face* shellFaceTop =
            currMesh.addFace({ vertex1TopInside, vertex1Top, vertex2Top, vertex2TopInside });
        shellFaces.push_back(shellFaceTop);

        // Mesh.add_face(newVertexList[vertex1TopInsideIndex],
        //              newVertexList[vertex1TopIndex], newVertexList[vertex2TopIndex],
        //              newVertexList[vertex2TopInsideIndex]);
    }
}

void CShellRefiner::pushBackFaces(Face* face)
{
    std::vector<int> idxList;
    std::vector<Vertex*> vertices1, vertices2;
    for (auto vertex : face->vertices)
    {
        int index = vertex->ID;

        int topIndex = newVertices[index].topIndex;
        int currIndex = newVertices[index].currIndex;
        vertices1.push_back(newVertexList[topIndex]);
        vertices2.push_back(newVertexList[currIndex]);
    }
    Face* face1 = currMesh.addFace(vertices1);
    Face* face2 = currMesh.addFace(vertices2);
    shellFaces.push_back(face1);
    shellFaces.push_back(face2);
}
float CShellRefiner::getAngle(Vector3 a, Vector3 b)
{
    float value = a.DotProduct(b) / a.Length() / b.Length();
    float epsilon = 1e-4;
    if (fabs(value - 1) < epsilon)
    {
        // value = 1
        return 0;
    }
    if (fabs(value + 1) < epsilon)
    {
        return (float)tc::M_PI;
    }
    if (fabs(value) < epsilon)
    {
        return (float)tc::M_PI / 2;
    }
    return acos(value);
}
Vertex* CShellRefiner::addPoint(Vector3 vector)
{
    // newVertexList.push_back(Mesh.add_vertex(CMeshImpl::Point(vector.x, vector.y,
    // vector.z)));

    // Randy replaced above with below two lines
    Vertex* newVert = new Vertex(vector.x, vector.y, vector.z, currMesh.vertList.size());
    newVert->name = "shellRefiner" + std::to_string(currMesh.vertList.size()) + "vert";
    currMesh.addVertex(newVert);
    newVertexList.push_back(newVert);

    shellVertices.push_back(newVert); // Randy replaced above with this
    return newVert;
}
}