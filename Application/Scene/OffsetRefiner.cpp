// Project AddOffset - Zachary's changes. I modified this file extensively to work with the new
// winged edge DS
#include "OffsetRefiner.h"
#include "qcolor.h"
#include <set>
#include <cmath>
#include <unordered_set>
#include <tuple>
#undef M_PI

using namespace std;
namespace Nome::Scene
{

COffsetRefiner::COffsetRefiner(DSMesh& _m, bool offsetFlag)
{
    currMesh = _m.newMakeCopy(); // Project AddOffset

    flag = offsetFlag;
    size_t numFaces = currMesh.faceList.size(); // Mesh.n_faces();
    faceVertices.resize(numFaces);
    newFaceVertices.resize(numFaces);
    for (auto face : currMesh.faceList)
    {
        int i = 0;
        for (auto vertex : face->vertices)
            i++;
        faceVertices[face->id] = i;
    }
    // Mesh.request_face_normals();

    size_t numVertices = currMesh.vertList.size(); // Mesh.n_vertices();
    vertexEdges.resize(numVertices); //no of Edge per vertex
    newVertices.resize(numVertices); // no of new vertices
    // for (auto vertex : currMesh.vertList)//Mesh.vertices())
    //{
    //    int i = 0;
    //    for (auto edge : vertex.edges())
    //        i++;
    //    vertexEdges[vertex.idx()] = i; // Randy note: so seems like i is the # of edges that
    //    reference the vert
    //}

    for (auto& Pair : currMesh.randyedgeTable)
    {
        Vertex* currVert = Pair.first;
        std::vector<Edge*> vertEdges = Pair.second;
        vertexEdges[currVert->ID] = vertEdges.size();
    }
}

void COffsetRefiner::Refine(float height, float width)
{
    bool needGrid = (width > 0);
    bool needOffset = (height > 0);

    auto vertSize = currMesh.vertList.size();

    for (int i = 0; i < vertSize; i++) // Randy perhaps vertList is changing size wrong
    {
        auto vertex = currMesh.vertList[i];
        if (vertexEdges[vertex->ID] < 2)
        {
            continue;
        }
        //generateNewVertices(vertex, height);
        generateNewVerticesForFace(vertex, height);
    }
    auto faces = currMesh.faceList; // Mesh.faces().to_vector();
    for (auto face : faces)
    {
        //if (needGrid)
        //{
        generateNewFaceVertices(face, width, height);
        //}
        generateNewFaces(face, needGrid, needOffset);
    }
    if (needOffset)
    {
        for (auto face : faces)
            closeFace(face);
    }

    currMesh.buildBoundary(); // Randy added these on 2/26
    currMesh.computeNormals();
}

float COffsetRefiner::AngleBetween(Vector3 a, Vector3 b)
{
    // FUnction calculate vector between vec1 and vec2

    float dot = a.x * b.x + a.y * b.y + a.z * b.z;
    float maga = std::sqrt(a.x * a.x + a.y * a.y + a.z * a.z);
    float magb = std::sqrt(b.x * b.x + b.y * b.y + b.z * b.z);
    if (dot == 0)
    {
        return 1000.0;
    }
    //std::cout << dot / (maga * magb) << std::endl;
    float angle = std::acos(max(min(dot / (maga * magb), 1.0f),-1.0f));
    return angle;
}

float COffsetRefiner::getNormalizationComponent(std::set<Face*> facesReq, Vector3 sumNormals)
{
    float ang = 0.0;
    for (auto face : facesReq)
    {
        float curang = AngleBetween(sumNormals, face->normal);
        if (curang == 1000.0)
        {
            ang = 10000.0;
            break;
        }
        ang = ang + curang;
    }
    if (ang != 10000.0)
    {
        ang = ang / (facesReq.size());
        return (1 / abs(cos(ang)));
        /*std::cout << "This is the angle: "<< ang << std::endl;
        std::cout << "This is the value multipled to the sumNormals: " 1 / abs(cos(ang)) <<
        std::endl;
    */
    }
    else
    {
        return 4.0f;
    }
}
float COffsetRefiner::getNormalizationComponentV2(std::set<Face*> facesReq, Vector3 sumNormals) {
    float cosAng = 0.0f;
    for (auto face : facesReq)
    {
        float curang = AngleBetween(sumNormals, face->normal);
        if (curang == 1000.0)
        {
            cosAng = 10000.0f;
            break;
        }
        cosAng = cosAng + cos(curang);
    }
    if (cosAng != 10000.0f)
    {
        cosAng = cosAng / (facesReq.size());
        return (1 / abs(cosAng));
    }
    else
    {
        return 2.0f;
    }
}
    void COffsetRefiner::generateNewVerticesForFace(Vertex* vertex, float height)
{
    //All of this section is Aaron's code
    Vector3 point = vertex->position;

    if (height <= 0)
    {
        addPoint(point);
        newVertices[vertex->ID] =
            OffsetVerticesInfo { vertex, (int)offsetVertices.size() - 1 }; // Randy replaced above
        return;
    }
    Vector3 sumFaceNormals;
    std::vector<Edge *> edges = currMesh.randyedgeTable[vertex];
    std::set<Face*> facesReq = std::set<Face*>();
    for (auto edge : edges)
    {
        if (edge->fa != NULL)
        {
            facesReq.insert(edge->fa);
        }
        if (edge->fb != NULL)
        {
            facesReq.insert(edge->fb);
        }
    }
    Vector3 sumNormals;
    for (auto face : facesReq)
    {
        sumNormals += face->normal;
    }
    sumNormals.Normalize();
    // find the extending coefficient
    sumNormals *= height / 2;
    sumNormals *= getNormalizationComponent(facesReq, sumNormals);

    Vector3 newPoint1 = point - sumNormals;
    Vector3 newPoint2 = point + sumNormals;
    Vertex* newVert1 = addPoint(newPoint1);
    Vertex* newVert2 = addPoint(newPoint2);
    int size = offsetVertices.size();
    newVertices[vertex->ID] = OffsetVerticesInfo { newVert1, size - 2, newVert2, size - 1 };

}
void COffsetRefiner::generateNewVertices(Vertex* vertex, float height)
{
    Vector3 point = vertex->position; // getPosition(vertex);

    if (height <= 0)
    {
        addPoint(point);
        // newVertices[vertex->ID] = OffsetVerticesInfo { point, (int)offsetVertices.size() - 1 };
        newVertices[vertex->ID] =
            OffsetVerticesInfo { vertex, (int)offsetVertices.size() - 1 }; // Randy replaced above
        return;
    }

    Vector3 sumEdges;

    if (vertexEdges[vertex->ID] > 2)
    {
        auto edges = currMesh.randyedgeTable[vertex];
        for (auto edge : edges)
        {
            sumEdges += getEdgeVector(edge, vertex).Normalized();
        }
    }
    else
    {
        std::vector<Edge*> edges = currMesh.randyedgeTable[vertex]; // vertex.edges().to_vector();

        Vector3 edge1;
        Vector3 edge2;
        bool forwardDir = (edges[1]->edge_ID - edges[0]->edge_ID) == 1;
        if (forwardDir)
        {
            std::cout << "1st edge of vert: " << edges[0]->edge_ID << std::endl;
            std::cout << "2nd edge of this vert: " << edges[1]->edge_ID << std::endl;
            edge1 = getEdgeVector(edges[0], vertex);
            edge2 = getEdgeVector(edges[1], vertex);
        }
        else // beginning vertex. need to reverse order to get same cross product direction
        {
            std::cout << "1st edge of vert: " << edges[1]->edge_ID << std::endl;
            std::cout << "2nd edge of this vert: " << edges[0]->edge_ID << std::endl;
            edge1 = getEdgeVector(edges[1], vertex);
            edge2 = getEdgeVector(edges[0], vertex);
        }

        sumEdges = crossProduct(edge1, edge2);
    }
    sumEdges.Normalize();
    sumEdges *= height / 2;

    Vector3 newPoint1 = point - sumEdges;
    Vector3 newPoint2 = point + sumEdges;
    Vertex* newVert1 = addPoint(newPoint1);
    Vertex* newVert2 = addPoint(newPoint2);
    int size = offsetVertices.size();
    newVertices[vertex->ID] = OffsetVerticesInfo { newVert1, size - 2, newVert2, size - 1 };
}


void COffsetRefiner::generateNewFaceVertices(Face* face, float width, float height)
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
            //normalise the prev path and curr path
            prevPath.Normalize();
            curPath.Normalize();
        }

        float angle = getAngle(prevPath, curPath);
        Vector3 offsetVector = (prevPath + curPath).Normalized() * width / sinf(angle / 2);

        Vector3 norm = crossProduct(prevPath, curPath).Normalized() * height / 2;

        Vector3 newPoint1 = curPoint + offsetVector - norm;
        Vector3 newPoint2 = curPoint + offsetVector + norm;

        Vertex* newVert1 = addPoint(newPoint1);
        Vertex* newVert2 = addPoint(newPoint2);
        int size = offsetVertices.size();
        newFaceVertices[face->id][idxList[index]] =
            OffsetVerticesInfo { newVert2, size - 2, newVert1,
                                 size - 1 }; // Randy changed this to be new Vert
    }
}

void COffsetRefiner::generateNewFaces(Face* face, bool needGrid, bool needOffset)
{

    if (!needGrid)
    {
        //Aaron changed this to include points specific to faces rather than to edges. 
        //Previously was causing bugs as there was collision between the in and out
        std::vector<int> indexList1, indexList2;
        std::vector<Vertex*> vertices1, vertices2;
        for (auto vertex : face->vertices)
        {
            int index = vertex->ID;

            //int topIndex = newFaceVertices[face->id][vertex->ID].topIndex;
            //int bottomIndex = newFaceVertices[face->id][vertex->ID].bottomIndex;
            //indexList1.push_back(topIndex);
            //indexList2.push_back(bottomIndex);

            //vertices1.push_back(newFaceVertices[face->id][vertex->ID].topVert);
            //vertices2.push_back(newFaceVertices[face->id][vertex->ID].bottomVert);

            //Aaron commented this out.

            int topIndex = newVertices[index].topIndex;
            int bottomIndex = newVertices[index].bottomIndex;
            indexList1.push_back(topIndex);
            indexList2.push_back(bottomIndex);
            vertices1.push_back(newVertexList[topIndex]);
            vertices2.push_back(newVertexList[bottomIndex]);
        }
        //std::reverse(indexList2.begin(), indexList1.end());
        //std::reverse(vertices2.begin(), vertices2.end());
        std::reverse(indexList1.begin(), indexList1.end());
        std::reverse(vertices1.begin(), vertices1.end());

        // offsetFaces.push_back(indexList1);
        // offsetFaces.push_back(indexList2);

        // Randy changed above line to the following
        Face* offsetFace1 = new Face(vertices1); // takes in a vector of Vertex objects
        Face* offsetFace2 = new Face(vertices2); // takes in a vector of Vertex objects
        
        offsetFaces.push_back(offsetFace1);
        offsetFaces.push_back(offsetFace2);

        //Set to different colors
        std::array<float,3> color1 = {0.f, 0.f, 0.f};
        std::array<float, 3> color2 = { 1.f, 0.9f, 0.3f };
        offsetFace1->color = color1;
        offsetFace2->color = color2;

        currMesh.addFace(vertices1, color1);
        currMesh.addFace(vertices2, color2);
        return;
    }

    int faceId = face->id;
    std::vector<int> idxList;

    for (auto vertex : face->vertices)
        idxList.push_back(vertex->ID);

    std::set<std::tuple<uint32_t, uint32_t, uint32_t, uint32_t>> duplicate_map;
    for (size_t i = 0; i < idxList.size(); i++) {
        int vertex1Id = idxList[i];
        // Vertex* vertex1Top = newVertices[vertex1Id].topVert;
        // Vertex* vertex1TopInside = newFaceVertices[faceId][vertex1Id].topVert;
        // Vertex* vertex2Top = newVertices[vertex2Id].topVert;
        // Vertex* vertex2TopInside = newFaceVertices[faceId][vertex2Id].topVert;

        // Vertex* vertex1Bottom = newVertices[vertex1Id].bottomVert;
        // Vertex* vertex1BottomInside = newFaceVertices[faceId][vertex1Id].bottomVert;
        // Vertex* vertex2Bottom = newVertices[vertex2Id].bottomVert;
        // Vertex* vertex2BottomInside = newFaceVertices[faceId][vertex2Id].bottomVert;



    }
    for (size_t i = 0; i < idxList.size(); i++)
    {
        int vertex1Id = idxList[i];
        int vertex2Id = idxList[(i + 1) % idxList.size()];

        int vertex1TopIndex = newVertices[vertex1Id].topIndex;
        int vertex1TopInsideIndex = newFaceVertices[faceId][vertex1Id].topIndex;

        int vertex2TopIndex = newVertices[vertex2Id].topIndex;
        int vertex2TopInsideIndex = newFaceVertices[faceId][vertex2Id].topIndex;

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
        // offsetFaces.push_back(faceIndexList);
        //Set to different colors
        // Randy below line to hopefully replace above lines
        // std::cout << "FACE CREATION" << std::endl; 
        // std::cout << "vertex1id: " << vertex1Id << std::endl;
        // std::cout << "vertex2id: " << vertex2Id << std::endl;
        // std::cout << "First Face: " << std::endl;
        // std::cout << vertex1TopInside->position.x << " " << vertex1TopInside->position.y << " " << vertex1TopInside->position.z << std::endl;
        // std::cout << vertex1Top->position.x << " " << vertex1Top->position.y << " " << vertex1Top->position.z << std::endl;
        // std::cout << vertex2Top->position.x << " " << vertex2Top->position.y << " " << vertex2Top->position.z << std::endl;
        // std::cout << vertex2TopInside->position.x << " " << vertex2TopInside->position.y << " " << vertex2TopInside->position.z << std::endl;
        if (duplicate_map.find(std::make_tuple(vertex1TopInside->ID, vertex1Top->ID, vertex2Top->ID, vertex2TopInside->ID)) == duplicate_map.end()) {
            Face* offsetFaceTop =
                currMesh.addFace({ vertex2TopInside, vertex2Top, vertex1Top, vertex1TopInside });

            offsetFaces.push_back(offsetFaceTop);
            duplicate_map.insert(std::make_tuple(vertex1TopInside->ID, vertex1Top->ID, vertex2Top->ID, vertex2TopInside->ID)); 
        }

        // Mesh.add_face(newVertexList[vertex1TopInsideIndex],
        //              newVertexList[vertex1TopIndex], newVertexList[vertex2TopIndex],
        //              newVertexList[vertex2TopInsideIndex]);

        if (needOffset)
        {
            Vertex* vertex1Bottom = newVertices[vertex1Id].bottomVert;
            Vertex* vertex1BottomInside = newFaceVertices[faceId][vertex1Id].bottomVert;
            Vertex* vertex2Bottom = newVertices[vertex2Id].bottomVert;
            Vertex* vertex2BottomInside = newFaceVertices[faceId][vertex2Id].bottomVert;

            Face* offsetFaceBotTop = currMesh.addFace(
                { vertex2BottomInside, vertex2TopInside, vertex1TopInside, vertex1BottomInside });
            offsetFaces.push_back(offsetFaceBotTop);
            // std::cout << "Second Face: " << std::endl;
            // std::cout << vertex1BottomInside->position.x << " " << vertex1BottomInside->position.y << " " << vertex1BottomInside->position.z << std::endl;
            // std::cout << vertex1TopInside->position.x << " " << vertex1TopInside->position.y << " " << vertex1TopInside->position.z << std::endl;
            // std::cout << vertex2TopInside->position.x << " " << vertex2TopInside->position.y << " " << vertex2TopInside->position.z << std::endl;
            // std::cout << vertex2BottomInside->position.x << " " << vertex2BottomInside->position.y << " " << vertex2BottomInside->position.z << std::endl;


            Face* offsetFaceBot =
                currMesh.addFace({ vertex2Bottom, vertex2BottomInside, vertex1BottomInside,
                                   vertex1Bottom }); 
            // std::cout << "Third Face: " << std::endl;
            // std::cout << vertex1Bottom->position.x << " " << vertex1Bottom->position.y << " " << vertex1Bottom->position.z << std::endl;
            // std::cout << vertex1BottomInside->position.x << " " << vertex1BottomInside->position.y << " " << vertex1BottomInside->position.z << std::endl;
            // std::cout << vertex2BottomInside->position.x << " " << vertex2BottomInside->position.y << " " << vertex2BottomInside->position.z << std::endl;
            // std::cout << vertex2Bottom->position.x << " " << vertex2Bottom->position.y << " " << vertex2Bottom->position.z << std::endl;
            offsetFaces.push_back(offsetFaceBot);
        }
    }
}

void COffsetRefiner::closeFace(Face* face)
{
    std::vector<int> idxList;
    for (auto vertex : face->vertices)
        idxList.push_back(vertex->ID);

    for (size_t i = 0; i < idxList.size(); i++)
    {
        int vertex1Id = idxList[i];
        int vertex2Id = idxList[(i + 1) % idxList.size()];

        int vertex1TopIndex = newVertices[vertex1Id].topIndex;
        int vertex1BottomIndex = newVertices[vertex1Id].bottomIndex;
        int vertex2TopIndex = newVertices[vertex2Id].topIndex;
        int vertex2BottomIndex = newVertices[vertex2Id].bottomIndex;

        Vertex* vertex1Top = newVertexList[vertex1TopIndex];
        Vertex* vertex1Bottom = newVertexList[vertex1BottomIndex];
        Vertex* vertex2Top = newVertexList[vertex2TopIndex];
        Vertex* vertex2Bottom = newVertexList[vertex2BottomIndex];

        std::vector<Edge*> boundaryEdgeList1 =
            currMesh.boundaryEdgeList(); // Randy added this. needed to check if vert is in boundary

        // check to see if the vertex is on an edge that is on a boundary.. In openMesh,
        // is_boundary() checked if a vertex is adjacent to a boundary edge

        //boundary edge is an edge that only has one face attached to it,.

        std::vector<Vertex*> verts = { vertex1Top, vertex1Bottom, vertex2Top, vertex2Bottom };
        std::vector<Edge*> boundaryList = currMesh.boundaryEdgeList();
        bool allOnBoundary = false;
        for (auto vert : verts)
        {
            bool onBoundary = false;
            std::vector<Edge*> boundaryList = currMesh.boundaryEdgeList();
            std::vector<Edge*> adjEdges = currMesh.randyedgeTable[vert];
            for (auto adjEdge : adjEdges)
            {
                if (std::find(boundaryList.begin(), boundaryList.end(), adjEdge)
                    != boundaryList.end())
                {
                    // just need one of the vert's edges to be onboundary
                    onBoundary = true;
                }
            }
            allOnBoundary = allOnBoundary || onBoundary;
        }

        if (allOnBoundary)
        {
            // currMesh.addPolygonFace({ vertex1Top, vertex1Bottom, vertex2Bottom, vertex2Top });

            // std::vector<int> faceIndexList = { vertex1TopIndex, vertex1BottomIndex,
            //                                    vertex2BottomIndex, vertex2TopIndex };
            // offsetFaces.push_back(faceIndexList);

            // Randy added below to replace above lines
            /*Face* offsetFaceClose =
                currMesh.addFace({ vertex1Top, vertex1Bottom, vertex2Bottom, vertex2Top });*/
            Face* offsetFaceClose =
                currMesh.addFace({ vertex2Top, vertex2Bottom, vertex1Bottom, vertex1Top });
            offsetFaces.push_back(offsetFaceClose);
        }
        // if (vertex1Top.is_boundary() && vertex1Bottom.is_boundary() && vertex2Top.is_boundary()
        //    && vertex2Bottom.is_boundary())
        //{
        //    Mesh.add_face(vertex1Top, vertex1Bottom, vertex2Bottom, vertex2Top);
        //    std::vector<int> faceIndexList = { vertex1TopIndex, vertex1BottomIndex,
        //                                       vertex2BottomIndex, vertex2TopIndex };
        //    offsetFaces.push_back(faceIndexList);
        //}
    }
}

// Vector3 COffsetRefiner::getPosition(OpenMesh::SmartVertexHandle vertex)
//{
//    const auto& pos = Mesh.point(vertex);
//    return Vector3(pos[0], pos[1], pos[2]);
//}

Vector3 COffsetRefiner::getEdgeVector(Edge* edge, Vertex* vertex)
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

Vector3 COffsetRefiner::crossProduct(Vector3 vectorA, Vector3 vectorB)
{
    return Vector3(vectorA.y * vectorB.z - vectorA.z * vectorB.y,
                   vectorA.z * vectorB.x - vectorA.x * vectorB.z,
                   vectorA.x * vectorB.y - vectorA.y * vectorB.x);
}

float COffsetRefiner::getAngle(Vector3 vectorA, Vector3 vectorB)
{
    float value = vectorA.DotProduct(vectorB) / vectorA.Length() / vectorB.Length();
    float epsilon = 1e-4;
    if (fabs(value - 1) < epsilon)
    {
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

    return acosf(vectorA.DotProduct(vectorB) / vectorA.Length() / vectorB.Length());
}

Vertex* COffsetRefiner::addPoint(Vector3 vector)
{
    // newVertexList.push_back(Mesh.add_vertex(CMeshImpl::Point(vector.x, vector.y, vector.z)));

    // Randy replaced above with below two lines
    Vertex* newVert = new Vertex(vector.x, vector.y, vector.z, currMesh.vertList.size());
    newVert->name = "offsetRefiner" + std::to_string(currMesh.vertList.size()) + "vert";
    currMesh.addVertex(newVert);
    newVertexList.push_back(newVert);
    // offsetVertices.push_back(vector);

    offsetVertices.push_back(newVert); // Randy replaced above with this
    return newVert;
}

}