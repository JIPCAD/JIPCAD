/**
 * @author Andy Wang, UC Berkeley.
 * Copyright 2016 reserve.
 * UC Berkeley, Slide_2016 project.
 * Advised by Prof. Sequin H. Carlos.
 */

//#include "group.h"
#include "DataStructureMesh.h"
#include "qcolor.h"
//#include "parameter.h"
#include <map>
#include <algorithm>

Mesh::Mesh(int type)
{
    user_set_color = false;
    //transformations_up.clear();
    parent = NULL;
    this->type = type;
    in_editing_mode = false;
    if (type == 1 || type == 2)
    {
        n = 0;
        ro = 0.0f;
        ratio = 0.0f;
        h = 0.0f;
        n_expr = "";
        ro_expr = "";
        ratio_expr = "";
        h_expr = "";
    }
    isConsolidateMesh = false;
}
void Mesh::addVertex(Vertex* v) { 
    v->ID = vertList.size();  // Robert added this

    vertList.push_back(v); 
    nameToVert[v->name] = v; // Randy added this
    idToVert[v->ID] = v; // Randy added this on 2/19. Doesn't make sense to have a nameToVert but no idToVert.
}

void Mesh::addVertex(float x, float y, float z) {
    Vertex* vertCopy = new Vertex(x, y, z, vertList.size());
    vertCopy->name = "SubdivVert" + std::to_string(vertList.size());
    addVertex(vertCopy);
}

Edge* Mesh::findEdge(const std::string& v1, const std::string& v2, bool setmobius) {
    return findEdge(nameToVert.at(v1), nameToVert.at(v2), setmobius);
}
Edge* Mesh::findEdge(Vertex* v1, Vertex* v2, bool setmobius)
{
    std::unordered_map<Vertex*, std::vector<Edge*>>::iterator vIt;
    std::vector<Edge*>::iterator eIt;
    vIt = edgeTable.find(v2);
    if (vIt != edgeTable.end())
    {
        for (eIt = (vIt->second).begin(); eIt < (vIt->second).end(); eIt++)
        {
            if ((*eIt)->vb == v1)
            {
                // cout<<"Find Edge from vertex "<<v2 -> ID<<" to vertex "<<v1 -> ID<<"."<<endl;
                return (*eIt);
            }
        }
    }
    vIt = edgeTable.find(v1);
    if (vIt != edgeTable.end())
    {
        for (eIt = (vIt->second).begin(); eIt < (vIt->second).end(); eIt++)
        {
            if ((*eIt)->vb == v2)
            {
                if (setmobius)
                {
                    // cout<<"Find M Edge from vertex "<<v1 -> ID<<" to vertex "<<v2 -> ID<<"."<<endl; why is this happening
                    (*eIt)->mobius = true;
                    (*eIt)->va->onMobius = true;
                    (*eIt)->vb->onMobius = true;
                }
                return (*eIt);
            }
        }
    }
    return NULL;
}

void Mesh::setEdgeSharpness(const std::string& v1, const std::string& v2, float sharpness)
{
    auto it1 = nameToVert.find(v1);
    auto it2 = nameToVert.find(v2);

    if (it1 == nameToVert.end() || it2 == nameToVert.end())
    {
        std::cout << "[sharp] Could not find vertex pair: "
                  << v1 << " " << v2 << std::endl;
        return;
    }

    Edge* edge = findEdge(it1->second, it2->second, false);

    if (!edge)
    {
        std::cout << "[sharp] Could not find existing edge: "
                  << v1 << " - " << v2
                  << ". Make sure sharp is applied after the face using this edge is created."
                  << std::endl;
        return;
    }

   edge->sharpness = sharpness;
    edge->isSharp = sharpness > 0.0f;

    // Optional but useful if OpenSubdiv corner sharpness is enabled.
    edge->v0()->sharpness = std::max(edge->v0()->sharpness, sharpness);
    edge->v1()->sharpness = std::max(edge->v1()->sharpness, sharpness);

    std::cout << "[sharp] Set edge "
              << v1 << " - " << v2
              << " sharpness = " << sharpness << std::endl;
}

Edge* Mesh::createEdge(Vertex* v1, Vertex* v2)
{
    Edge* edge = findEdge(v1, v2);
    if (edge == NULL)
    {
        //std::cout<<"Creating new Edge from vertex "<<v1 -> ID<<" to vertex "<<v2 -> ID<<"."<<std::endl;
        edge = new Edge(v1, v2);
        edge->edge_ID = edgeList.size();
        edgeList.push_back(edge);
        if (v1->oneEdge == NULL)
        {
            v1->oneEdge = edge;
        }
        if (v2->oneEdge == NULL)
        {
            v2->oneEdge = edge;
        }
        std::unordered_map<Vertex*, std::vector<Edge*>>::iterator vIt;
        vIt = edgeTable.find(v1);
        if (vIt != edgeTable.end())
        {
            (vIt->second).push_back(edge);
        }
        else
        {
            std::vector<Edge*> currEdges;
            currEdges.push_back(edge);
            edgeTable[v1] = currEdges;
        }

        if (randyedgeTable.count(v1) > 0)
        {
            randyedgeTable[v1].push_back(edge);
        }
        else
        {
            std::vector<Edge*> currEdges;
            currEdges.push_back(edge);
            randyedgeTable[v1] = currEdges;
        }

        if (randyedgeTable.count(v2) > 0)
        {
            randyedgeTable[v2].push_back(edge);
        }
        else
        {
            std::vector<Edge*> currEdges;
            currEdges.push_back(edge);
            randyedgeTable[v2] = currEdges;
        }
    }
    // cout<<"The va of edge is "<<edge -> va -> ID<<" . The vb is "<< edge -> vb -> ID<<" ."<<endl;
    return edge;
}

Face* Mesh::addFace(std::vector<Vertex*> vertices, std::string surfaceName,
                    std::string backfaceName, bool reverseOrder)
{
    if (vertices.size() < 3)
    {
        std::cout << "A face have at least 3 vertices" << std::endl;
        return NULL; // Randy added the null
    }
    Face* newFace = new Face(vertices); // Randy added the (vertices)
    newFace->surfaceName = surfaceName;
    newFace->backfaceName = backfaceName;
    std::vector<Vertex*>::iterator vIt;
    std::vector<Edge*> edgesInFace;
    std::vector<Edge*>::iterator eIt;
    Edge* currEdge = NULL;

    if (!reverseOrder)
    {
        for (vIt = vertices.begin(); vIt < vertices.end(); vIt++)
        {
            if (vIt != vertices.end() - 1)
            {
                //Aaron's edit: there are a bunch of edges that have two equal vertices. 
                if (*vIt == *(vIt + 1))
                {
                    continue;
                }
                currEdge = createEdge(*vIt, *(vIt + 1));
                edgesInFace.push_back(currEdge);
            }
            else
            {
                if (*vIt == *(vertices.begin()))
                {
                    continue;
                }
                currEdge = createEdge(*vIt, *(vertices.begin()));
                edgesInFace.push_back(currEdge);
            }
            if (currEdge->fa == NULL)
            {
                currEdge->fa = newFace;
            }
            else if (currEdge->fb == NULL)
            {
                currEdge->fb = newFace;
            }
            else
            {
                std::cout << "addFace ERROR: Try to create a Non-Manifold at edge with vertex1 : "
                     << currEdge->va->position.x << " " << currEdge->va->position.y << "  "
                     << currEdge->va->position.z << " and vertex2 :"
                    << currEdge->vb->position.x << " " << currEdge->vb->position.y << "  "
                                                     << currEdge->vb->position.z
                          << std::endl;
               // exit(0);
            }
        }
    }
    else
    {
        for (vIt = vertices.end() - 1; vIt >= vertices.begin(); vIt--)
        {
            if (vIt != vertices.begin())
            {
                currEdge = createEdge(*vIt, *(vIt - 1));
                edgesInFace.push_back(currEdge);
            }
            else
            {
                currEdge = createEdge(*vIt, *(vertices.end() - 1));
                edgesInFace.push_back(currEdge);
            }
            if (currEdge->fa == NULL)
            {
                currEdge->fa = newFace;
            }
            else if (currEdge->fb == NULL)
            {
                currEdge->fb = newFace;
            }
            else
            {
                std::cout << "ERROR: Try to create a Non-Manifold at edge with vertex1 : "
                          << currEdge->va->ID << " and vertex2 :" << currEdge->vb->ID << std::endl;
               // exit(0); // TODO: This is being triggered
            }
        }
    }
    if (currEdge==NULL)
    {
        return NULL;
    }
    newFace->oneEdge = currEdge;
    for (eIt = edgesInFace.begin(); eIt < edgesInFace.end(); eIt++)
    {
        Edge* currEdge = (*eIt);
        if (eIt == edgesInFace.begin())
        {
            if (newFace == currEdge->fa)
            {
                currEdge->nextVbFa = *(eIt + 1);
                currEdge->nextVaFa = *(edgesInFace.end() - 1);
            }
            else
            {
                if (currEdge->mobius)
                {
                    currEdge->nextVbFb = *(eIt + 1);
                    currEdge->nextVaFb = *(edgesInFace.end() - 1);
                }
                else
                {
                    currEdge->nextVaFb = *(eIt + 1);
                    currEdge->nextVbFb = *(edgesInFace.end() - 1);
                }
            }
        }
        else if (eIt == (edgesInFace.end() - 1))
        {
            if (newFace == currEdge->fa)
            {
                currEdge->nextVbFa = *(edgesInFace.begin());
                currEdge->nextVaFa = *(eIt - 1);
            }
            else
            {
                if (currEdge->mobius)
                {
                    currEdge->nextVbFb = *(edgesInFace.begin());
                    currEdge->nextVaFb = *(eIt - 1);
                }
                else
                {
                    currEdge->nextVaFb = *(edgesInFace.begin());
                    currEdge->nextVbFb = *(eIt - 1);
                }
            }
        }
        else
        {
            if (newFace == currEdge->fa)
            {
                currEdge->nextVbFa = *(eIt + 1);
                currEdge->nextVaFa = *(eIt - 1);
            }
            else
            {
                if (currEdge->mobius)
                {
                    currEdge->nextVbFb = *(eIt + 1);
                    currEdge->nextVaFb = *(eIt - 1);
                }
                else
                {
                    currEdge->nextVaFb = *(eIt + 1);
                    currEdge->nextVbFb = *(eIt - 1);
                }
            }
        }
    }
    //newFace->surfaceName = surfaceName;
    //newFace->backfaceName = backfaceName;
    newFace->id = faceList.size();
    newFace->name = "addedFace" + std::to_string(faceList.size()); // Randy added this on 2/12 because realized we weren't naming DS faces anywhere else
    faceList.push_back(newFace);
    nameToFace[newFace->name] = newFace; // Randy added this
    return newFace;
}
Face* Mesh::addFace(std::vector<Vertex*> vertices, std::array<float, 3> color,
                    std::string surfaceName, std::string backfaceName,
                     bool reverseOrder)
{
    if (vertices.size() < 3)
    {
        std::cout << "A face have at least 3 vertices" << std::endl;
        return NULL; // Randy added the null
    }
    Face* newFace = new Face(vertices); // Randy added the (vertices)
    newFace->color = color;
    newFace->surfaceName = surfaceName;
    newFace->backfaceName = backfaceName;
    std::vector<Vertex*>::iterator vIt;
    std::vector<Edge*> edgesInFace;
    std::vector<Edge*>::iterator eIt;
    Edge* currEdge = NULL;

    if (!reverseOrder)
    {
        for (vIt = vertices.begin(); vIt < vertices.end(); vIt++)
        {
            if (vIt != vertices.end() - 1)
            {
                // Aaron's edit: there are a bunch of edges that have two equal vertices.
                if (*vIt == *(vIt + 1))
                {
                    continue;
                }
                currEdge = createEdge(*vIt, *(vIt + 1));
                edgesInFace.push_back(currEdge);
            }
            else
            {
                if (*vIt == *(vertices.begin()))
                {
                    continue;
                }
                currEdge = createEdge(*vIt, *(vertices.begin()));
                edgesInFace.push_back(currEdge);
            }
            if (currEdge->fa == NULL)
            {
                currEdge->fa = newFace;
            }
            else if (currEdge->fb == NULL)
            {
                currEdge->fb = newFace;
            }
            else
            {
                std::cout << "addFace ERROR: Try to create a Non-Manifold at edge with vertex1 : "
                          << currEdge->va->position.x << " " << currEdge->va->position.y << "  "
                          << currEdge->va->position.z
                          << " and vertex2 :" << currEdge->vb->position.x << " "
                          << currEdge->vb->position.y << "  " << currEdge->vb->position.z
                          << std::endl;
                // exit(0);
            }
        }
    }
    else
    {
        for (vIt = vertices.end() - 1; vIt >= vertices.begin(); vIt--)
        {
            if (vIt != vertices.begin())
            {
                currEdge = createEdge(*vIt, *(vIt - 1));
                edgesInFace.push_back(currEdge);
            }
            else
            {
                currEdge = createEdge(*vIt, *(vertices.end() - 1));
                edgesInFace.push_back(currEdge);
            }
            if (currEdge->fa == NULL)
            {
                currEdge->fa = newFace;
            }
            else if (currEdge->fb == NULL)
            {
                currEdge->fb = newFace;
            }
            else
            {
                std::cout << "ERROR: Try to create a Non-Manifold at edge with vertex1 : "
                          << currEdge->va->ID << " and vertex2 :" << currEdge->vb->ID << std::endl;
                // exit(0); // TODO: This is being triggered
            }
        }
    }
    if (currEdge == NULL)
    {
        return NULL;
    }
    newFace->oneEdge = currEdge;
    for (eIt = edgesInFace.begin(); eIt < edgesInFace.end(); eIt++)
    {
        Edge* currEdge = (*eIt);
        if (eIt == edgesInFace.begin())
        {
            if (newFace == currEdge->fa)
            {
                currEdge->nextVbFa = *(eIt + 1);
                currEdge->nextVaFa = *(edgesInFace.end() - 1);
            }
            else
            {
                if (currEdge->mobius)
                {
                    currEdge->nextVbFb = *(eIt + 1);
                    currEdge->nextVaFb = *(edgesInFace.end() - 1);
                }
                else
                {
                    currEdge->nextVaFb = *(eIt + 1);
                    currEdge->nextVbFb = *(edgesInFace.end() - 1);
                }
            }
        }
        else if (eIt == (edgesInFace.end() - 1))
        {
            if (newFace == currEdge->fa)
            {
                currEdge->nextVbFa = *(edgesInFace.begin());
                currEdge->nextVaFa = *(eIt - 1);
            }
            else
            {
                if (currEdge->mobius)
                {
                    currEdge->nextVbFb = *(edgesInFace.begin());
                    currEdge->nextVaFb = *(eIt - 1);
                }
                else
                {
                    currEdge->nextVaFb = *(edgesInFace.begin());
                    currEdge->nextVbFb = *(eIt - 1);
                }
            }
        }
        else
        {
            if (newFace == currEdge->fa)
            {
                currEdge->nextVbFa = *(eIt + 1);
                currEdge->nextVaFa = *(eIt - 1);
            }
            else
            {
                if (currEdge->mobius)
                {
                    currEdge->nextVbFb = *(eIt + 1);
                    currEdge->nextVaFb = *(eIt - 1);
                }
                else
                {
                    currEdge->nextVaFb = *(eIt + 1);
                    currEdge->nextVbFb = *(eIt - 1);
                }
            }
        }
    }
    newFace->id = faceList.size();
    newFace->name =
        "addedFace" + std::to_string(faceList.size()); // Randy added this on 2/12 because realized
                                                       // we weren't naming DS faces anywhere else
    newFace->color = color;
    
    faceList.push_back(newFace);
    nameToFace[newFace->name] = newFace; // Randy added this
    return newFace;
}
    // Build the next pointers for boundary edges in the mesh.
// @param mesh: refer to the mesh to build connection in.
// This one takes O(E) time.
void Mesh::buildBoundary()
{
    std::unordered_map<Vertex*, std::vector<Edge*>>::iterator evIt;
    std::vector<Edge*> edgesAtThisPoint;
    std::vector<Edge*>::iterator eIt;
    for (evIt = edgeTable.begin(); evIt != edgeTable.end(); evIt++)
    {
        edgesAtThisPoint = evIt->second;
        if (!edgesAtThisPoint.empty())
        {
            for (eIt = edgesAtThisPoint.begin(); eIt < edgesAtThisPoint.end(); eIt++)
            {
                Edge* currEdge = (*eIt);
                //NextEdge is null means that the face fb must be null
                if ((currEdge->nextEdge(currEdge->va, currEdge->fb)) == NULL)
                {
                    Edge* firstBoundaryEdge = currEdge;
                    Vertex* currVert = currEdge->va;
                    Edge* nextBoundaryEdge;
                    // cout<<"first: "<<currEdge -> va -> ID<<" "<<currEdge -> vb -> ID<<endl;
                    do
                    {
                        // cout<<"Now building boundary at vertex: "<<endl;
                        // cout<<currVert -> ID<<endl;
                        Face* currFace = currEdge->fa;
                        nextBoundaryEdge = currEdge->nextEdge(currVert, currFace);
                        while (nextBoundaryEdge->fb != NULL)
                        {
                            currFace = nextBoundaryEdge->theOtherFace(currFace);
                            nextBoundaryEdge = nextBoundaryEdge->nextEdge(currVert, currFace);
                        }
                        currEdge->setNextEdge(currVert, NULL, nextBoundaryEdge);
                        nextBoundaryEdge->setNextEdge(currVert, NULL, currEdge);
                        currEdge = nextBoundaryEdge;
                        currVert = currEdge->theOtherVertex(currVert);
                    } while (currEdge != firstBoundaryEdge);
                }
            }
        }
    }
}

// @param p1, p2, p3 are positions of three vertices,
// with edge p1 -> p2 and edge p2 -> p3.
tc::Vector3 getNormal3Vertex(tc::Vector3 p1, tc::Vector3 p2, tc::Vector3 p3) { return (p2 - p1).CrossProduct(p3 - p2); }

// Get the surface normal.
// @param currFace: pointer of the face.
void getFaceNormal(Face* currFace)
{
    // cout<<"New Face!"<<endl;
    /*
    Edge* firstEdge = currFace->oneEdge;
    Edge* currEdge;
    Edge* nextEdge;
    currEdge = firstEdge;
    tc::Vector3 avgNorm = tc::Vector3(0, 0, 0);
    tc::Vector3 p1;
    tc::Vector3 p2;
    tc::Vector3 p3;
    */
    float Nx = 0, Ny = 0, Nz = 0;
    size_t n = currFace->vertices.size();

    for (size_t i = 0; i < n; ++i)
    {
        const tc::Vector3& current = currFace->vertices.at(i)->position;
        const tc::Vector3& next = currFace->vertices.at((i + 1) % n)->position;
        Nx += (current.y - next.y) * (current.z + next.z);
        Ny += (current.z - next.z) * (current.x + next.x);
        Nz += (current.x - next.x) * (current.y + next.y);
    }

    tc::Vector3 normal(Nx, Ny, Nz);
    currFace->normal = normal.Normalized();
    /*
    {
        if (currFace == currEdge->fa)
        {
            nextEdge = currEdge->nextVbFa;
            p1 = currEdge->va->position;
            p2 = currEdge->vb->position;
            p3 = nextEdge->theOtherVertex(currEdge->vb)->position;
        }
        if (currFace == currEdge->fb)
        {
            if (currEdge->mobius)
            {
                nextEdge = currEdge->nextVbFb;
                p1 = currEdge->va->position;
                p2 = currEdge->vb->position;
                p3 = nextEdge->theOtherVertex(currEdge->vb)->position;
            }
            else
            {
                nextEdge = currEdge->nextVaFb;
                p1 = currEdge->vb->position;
                p2 = currEdge->va->position;
                p3 = nextEdge->theOtherVertex(currEdge->va)->position;
            }
        }
        avgNorm += getNormal3Vertex(p1, p2, p3);
    }
    currFace->normal = avgNorm.Normalized();
    */
}

// Get the vertex normal
// @param currVert: the target vertex.
void getVertexNormal(Vertex* currVert)
{
    
    //std::cout << "pre calculation normal: (" << currVert->normal.x << "," << currVert->normal.y
      //        << "," << currVert->normal.z << ")\n"; 
    Edge* firstEdge = currVert->oneEdge;
    if (firstEdge == nullptr)
    {
        std::cout << currVert->name
                  << " is a lonely vertex without any adjacent edges. This error message also "
                     "appear for shapes with non-manifold verts and for polylines"
                  << std::endl;
        return;
    }

    Face* currFace = (firstEdge->fa != nullptr) ? firstEdge->fa : firstEdge->fb;
    tc::Vector3 avgNorm(0, 0, 0);

    // 1. BOUNDARY SEARCH LOOP
    Edge* startEdge = firstEdge;
    Edge* checkEdge = firstEdge;
    Face* checkFace = currFace;

    do
    {
        // If the current edge is boundary, use it.
        if (checkEdge->fa == nullptr || checkEdge->fb == nullptr)
        {
            startEdge = checkEdge;
            break;
        }

        Edge* nextE = checkEdge->nextEdge(currVert, checkFace);

        if (nextE == nullptr)
            break;

        // If nextE is boundary, save it BEFORE calling theOtherFace(),
        // because theOtherFace() may return nullptr and you would lose it.
        if (nextE->fa == nullptr || nextE->fb == nullptr)
        {
            startEdge = nextE;
            break;
        }

        Face* nextFace = nextE->theOtherFace(checkFace);

        if (nextFace == nullptr)
        {
            startEdge = nextE;
            break;
        }

        checkEdge = nextE;
        checkFace = nextFace;

    } while (checkEdge != firstEdge && checkEdge != nullptr);

    // 2. NORMAL CALCULATION LOOP
    bool breakOnNext = false;
    Edge* currEdge = startEdge;
    currFace = (currEdge->fa != nullptr) ? currEdge->fa : currEdge->fb;
    //std::cout << "\n---calc begin---\n";
    while (currEdge != nullptr)
    {
        if (currFace == nullptr)
        {
            break;
        }
        Vertex* otherVert = (currEdge->va == currVert) ? currEdge->vb : currEdge->va;

        // 1. Decide if we should skip the math for this step
        bool skipMath = false;

        // Skip if it touches a hole, OR if the face is a ribbon
        if (otherVert->name.find("_hole123") != std::string::npos
            || currFace->name.find("offsetRibbon") != std::string::npos)
        {
            skipMath = true;
        }
        if (!skipMath)
        {

            int v_idx = -1;
            for (int index = 0; index < currFace->vertices.size(); ++index)
            {
                if (currFace->vertices[index] == currVert)
                {
                    v_idx = index;
                    break;
                }
            }

            if (v_idx != -1)
            {
                Vertex* vPrev = currFace->vertices[(v_idx - 1 + currFace->vertices.size())
                                                   % currFace->vertices.size()];
                Vertex* vNext = currFace->vertices[(v_idx + 1) % currFace->vertices.size()];
                //std::cout << "names: " << vPrev->name << ", and " << vNext->name << "; Face Name: << " << currFace->name << "; Face Vertex Count: "<< currFace->vertices.size()<<"\n";
                if (!(vPrev->name.find("_hole") != std::string::npos
                      || vNext->name.find("_hole") != std::string::npos))
                {
                    tc::Vector3 e1 = (vPrev->position - currVert->position).Normalized();
                    tc::Vector3 e2 = (vNext->position - currVert->position).Normalized();

                    float dot = e1.DotProduct(e2);
                    dot = std::max(-1.0f, std::min(1.0f, dot));

                    float angle = acos(dot);
                    if (currFace->name.find("offsetRibbon") == std::string::npos)
                    {
                        avgNorm += (currFace->normal * angle);
                    }
                }
            }
        }
        // Advance to the next edge
        Edge* next = currEdge->nextEdge(currVert, currFace);

        // Prevent null pointer crash at the end of a boundary fan
        if (next == nullptr)
            break;

        currFace = next->theOtherFace(currFace);
        currEdge = next;

        if (currEdge == startEdge)
        {
            break;
            if ((avgNorm.x == 0 && avgNorm.y == 0 && avgNorm.z == 0) && !breakOnNext)
                breakOnNext = true;
            else
                break;
        }

    }

    // Normalize the final accumulated vector
    tc::Vector3 finalNormal = avgNorm.Normalized();
    if (finalNormal.x != 0 || finalNormal.y != 0 || finalNormal.z != 0)
    {
        //std::cout << "non zero normal\n";
        currVert->normal = finalNormal;
    }
    else
    {
        /*
        std::cout << "zero normal - (" << currVert->normal.x << "," << currVert->normal.y << ","
                  << currVert->normal.z << ")\n";
        */
    }
    std::cout << "NORMAL START for " << currVert->name << "\n";
    std::cout << "  startEdge: " << startEdge->va->name << " - " << startEdge->vb->name << "\n";

    if (startEdge->fa)
        std::cout << "  fa surface=" << startEdge->fa->surfaceName << "\n";
    else
        std::cout << "  fa=null\n";

    if (startEdge->fb)
        std::cout << "  fb surface=" << startEdge->fb->surfaceName << "\n";
    else
        std::cout << "  fb=null\n";
}

// Iterate over every vertex in the mesh and compute its normal
void Mesh::computeNormals(bool isPolyline)
{
    std::vector<Vertex*>::iterator vIt;
    std::vector<Face*>::iterator fIt;
    // cout<<"faceTable size: "<<faceList.size()<<endl;
    for (fIt = faceList.begin(); fIt < faceList.end(); fIt++)
    {
        getFaceNormal(*fIt);
    }

    // cout<<"vertTable size: "<<vertList.size()<<endl;
    if (!isPolyline)
    {
        for (vIt = vertList.begin(); vIt != vertList.end(); vIt++)
        {
            //std::cout <<"Now calculating vertex with ID: "<< *vIt <<std::endl;
            if ((*vIt)->oneEdge == nullptr)
            {
                continue;
            }
            getVertexNormal(*vIt);
        }
    }
}


std::vector<Edge*> Mesh::boundaryEdgeList()
{
    std::vector<Edge*> boundaryEdgeList;
    std::unordered_map<Vertex*, std::vector<Edge*>>::iterator vIt;
    // cout<<"Edge table size: "<<edgeTable.size()<<endl;
    for (vIt = edgeTable.begin(); vIt != edgeTable.end(); vIt++)
    {
        // cout<<vIt -> first -> ID<<endl;
        std::vector<Edge*> edges = vIt->second;
        std::vector<Edge*>::iterator eIt;
        std::vector<Edge*> newEdges;
        for (eIt = edges.begin(); eIt < edges.end(); eIt++)
        {
            if ((*eIt)->fb == NULL)
            {
                boundaryEdgeList.push_back(*eIt);
            }
        }
    }
    // cout<<"size: " << boundaryEdgeList.size()<<endl;
    return boundaryEdgeList;
}

Vertex Mesh::centerPoint(const Face* f) 
{ 
    Vertex vert = Vertex();
    vert.position.x = 0;
    vert.position.y = 0;
    vert.position.z = 0;
    for (auto v : f->vertices)
    {
        vert.position.x += v->position.x;
        vert.position.y += v->position.y;
        vert.position.z += v->position.z;
    }
    vert.position.x /= f->vertices.size();
    vert.position.y /= f->vertices.size();
    vert.position.z /= f->vertices.size();
    return vert;
}
bool Mesh::isEmpty() { return vertList.size() == 0 && faceList.size() == 0; }

void Mesh::clear()
{
    for (Edge*& e : edgeList)
    {
        delete e;
    }

    for (Vertex*& v : vertList)
    {
        delete v;
    }

    for (Face*& f : faceList)
    {
        delete f;
    }

    nameToFace.clear();
    nameToVert.clear();
    idToVert.clear();

    vertList.clear();
    faceList.clear();
    edgeList.clear();
    edgeTable.clear();
    randyedgeTable.clear();
}

void Mesh::clearAndDelete()
{
    for (Edge*& e : edgeList)
    {
        delete e;
    }

    for (Vertex*& v : vertList)
    {
        delete v;
    }

    for (Face*& f : faceList)
    {
        delete f;
    }

    nameToFace.clear();
    nameToVert.clear();
    idToVert.clear();

    vertList.clear();
    faceList.clear();
    edgeList.clear();
    edgeTable.clear();
    randyedgeTable.clear();
}

//void Mesh::setColor(QColor color) { this->color = color; }


// test function
//what this does is define a Mesh class function outside of the Mesh class
/*
Mesh Mesh::newMakeCopy(std::string copy_mesh_name, bool isPolyline)
{
    // cout<<"Creating a copy of the current map.\n";
    Mesh newMesh;
    if (copy_mesh_name == "")
    {
        newMesh.name = this->name;
    }
    else
    {
        newMesh.name = copy_mesh_name;
    }
    newMesh.clear();
    std::map<Vertex*, Vertex*> oldToNewVertMap;

    std::vector<Vertex*>::iterator vIt;
    for (vIt = vertList.begin(); vIt < vertList.end(); vIt++)
    {
      
        Vertex* vertCopy = new Vertex;
        vertCopy->ID = (*vIt)->ID;
        vertCopy->name = (*vIt)->name;
        vertCopy->position = (*vIt)->position;
        vertCopy->sharpness = (*vIt)->sharpness;
        newMesh.addVertex(vertCopy);
        oldToNewVertMap[*vIt] = vertCopy; // <-- CRITICAL: Map the pointer, not the ID

    }
    std::vector<Face*>::iterator fIt;
    std::vector<Vertex*> vertices;
    for (fIt = faceList.begin(); fIt < faceList.end(); fIt++)
    {
        Face* tempFace = *fIt;
        if (tempFace == nullptr || tempFace->oneEdge == nullptr)
        {
            // Skip faces that are null or have no starting edge (corrupted topology)
            continue; 
        }
        Edge* firstEdge = tempFace->oneEdge;
        Edge* currEdge = firstEdge;
        Edge* nextEdge;
        Vertex* tempv;
        // Aaron's notes
        // This code is basically trying to gauge whether the 
        // face is made up of either 3 or 4 vertices
        //I.e., whether the face is a triangle or not.
        // If the face is a triangle, then by definition
        //we can put the face straight into the 
        //newmesh. However, if the face is a 4 sided shape
        //then we know that the vertices of the face must be ordered
        // in some way, and the way we figure that out is by using the 
        // loop below. 

        Edge* prevEdge;
        int c=0;
        vertices.clear();
        int isTriangle = 0;
        std::vector<Vertex*>::iterator vIt;
        int mvId[6], mvP=0, i;
        for (vIt = tempFace->vertices.begin(); vIt < tempFace->vertices.end(); vIt++)
        {
            for (Vertex* oldVert : tempFace->vertices)
            {
                // Use the map to find the correct, newly-created vertex
                if (oldToNewVertMap.count(oldVert))
                {
                    Vertex* newVert = oldToNewVertMap[oldVert];

                    // Since newVert was added sequentially, its ID (index) is safe.
                    vertices.push_back(newVert);
                }
                else
                {
                    // This handles corrupt faces pointing to vertices that don't exist
                    std::cout << "CRITICAL ERROR: Face references unmapped vertex, skipping face."
                              << std::endl;
                    break;
                }
            }
            Vertex* vt = *vIt;
            int newData = 1;
            
            for (i = 0; i < mvP && i < 5; i++)
            {
                if (mvId[i] == vt->ID)
                {
                    isTriangle = 1;
                    newData = 0;
                }
            }
            if (newData)
            {
                if(vt->ID < newMesh.vertList.size())
                {
                    vertices.push_back(newMesh.vertList[vt->ID]);
                }
                else
                {
                    // This is the source of the crash! Handle the error.
                    std::cout << "CRITICAL ERROR: Vertex ID " << vt->ID << " out of range in copy!"
                              << std::endl;
                    break; // Exit the loop and skip the face
                }
                    mvId[mvP++] = vt->ID;                
            }
            //std::cout << "mvP: " << mvP << ", i: " << i << "\n";
        }

        if (!isTriangle)
        {
            vertices.clear();

            do
            {
                
                if (currEdge == nullptr)
                {
                    // This is the source of the crash. The topology is corrupt or open.
                    std::cout
                        << "WARNING: Encountered null edge pointer during copy. Skipping face."
                        << std::endl;
                    break;
                }
                if (tempFace == currEdge->fa)
                {
                    tempv = currEdge->vb;
                    nextEdge = currEdge->nextVbFa;
                    c = 1;
                }
                else
                { 
                    assert(tempFace==currEdge->fb);
                    if (currEdge->mobius)
                    {
                        tempv = currEdge->vb;
                        nextEdge = currEdge->nextVbFb;
                        c = 2;
                    }
                    else
                    {
                        tempv = currEdge->va;
                        nextEdge = currEdge->nextVaFb;
                        c = 3;
                    }
                }
                vertices.push_back(newMesh.vertList[tempv->ID]);
                //Aaron's edit
#if 0
                if (currEdge == nextEdge)
                {
                    /*if (prevEdge)
                    {
                        switch (c)
                        {
                            case 1:
                                prevEdge->setNextEdge(tempv, currEdge->fa, nextEdge);
                            case 2:
                                prevEdge->setNextEdge(tempv, currEdge->fb, nextEdge);
                            case 3:
                                prevEdge->setNextEdge(tempv, currEdge->fb, nextEdge);
                            default:
                                prevEdge->setNextEdge(tempv, currEdge->fa, nextEdge);
                        }       
                    }// 
                    std::cout << "ERROR: Current Edge is isolated, equal to the next Edge" << std::endl;
                    if (tempFace == currEdge->fa)
                    {
                        if (currEdge == currEdge->nextVbFa)
                        {
                            nextEdge = currEdge->nextVbFb;
                        }
                        else
                        {
                            nextEdge = currEdge->nextVbFa;
                        }
                    }
                    else
                    {
                        if (currEdge == currEdge->nextVaFa)
                        {
                            nextEdge = currEdge->nextVaFb;
                        }
                        else
                        {
                            nextEdge = currEdge->nextVaFa;
                        }
                    }
                }
                //prevEdge = currEdge;
            
#endif
                currEdge = nextEdge;
            } while (currEdge != firstEdge);
        }

        newMesh.addFace(vertices);
        newMesh.faceList[newMesh.faceList.size() - 1]->user_defined_color =
            (*fIt)->user_defined_color;
        newMesh.faceList[newMesh.faceList.size() - 1]->color = (*fIt)->color;
        newMesh.faceList[newMesh.faceList.size() - 1]->backcolor = (*fIt)->backcolor;
        newMesh.faceList[newMesh.faceList.size() - 1]->name = (*fIt)->name;
        newMesh.faceList[newMesh.faceList.size() - 1]->surfaceName =
            (*fIt)->surfaceName; // Randy added this
        newMesh.faceList[newMesh.faceList.size() - 1]->backfaceName =
            (*fIt)->backfaceName; // Randy added this
    }

    std::vector<Edge*>::iterator eItr;

    for (eItr = newMesh.edgeList.begin(); eItr != newMesh.edgeList.end(); eItr++) {
        Edge* edge = findEdge((*eItr)->v0()->name, (*eItr)->v1()->name, false);
        if (edge)
        {
            (*eItr)->sharpness = edge->sharpness;
        }
        else
        {
            std::cout << "Error:  Edge Not Found" + (*eItr)->v0()->name << std::endl;
            (*eItr)->sharpness = 0.0;
        }
        
    }
    
    newMesh.buildBoundary();
    newMesh.computeNormals(isPolyline);
    return newMesh;
}
*/
Mesh Mesh::newMakeCopy(std::string copy_mesh_name, bool isPolyline)
{
    // cout<<"Creating a copy of the current map.\n";
    Mesh newMesh;
    if (copy_mesh_name == "")
    {
        newMesh.name = this->name;
    }
    else
    {
        newMesh.name = copy_mesh_name;
    }
    newMesh.clear();

    // CRITICAL: Map from old Vertex* (in 'this' mesh) to new Vertex* (in newMesh)
    std::map<Vertex*, Vertex*> oldToNewVertMap;

    std::vector<Vertex*>::iterator vIt;
    for (vIt = vertList.begin(); vIt < vertList.end(); vIt++)
    {
        Vertex* vertCopy = new Vertex;
        // IMPORTANT: The ID is set sequentially inside newMesh.addVertex()
        vertCopy->name = (*vIt)->name;
        vertCopy->position = (*vIt)->position;
        vertCopy->sharpness = (*vIt)->sharpness;
        vertCopy->normal = (*vIt)->normal;

        // This MUST set the new ID based on newMesh.vertList.size()
        newMesh.addVertex(vertCopy);
        oldToNewVertMap[*vIt] = vertCopy; // Map the old pointer to the new pointer
    }

    std::vector<Face*>::iterator fIt;

    for (fIt = faceList.begin(); fIt < faceList.end(); fIt++)
    {
        Face* tempFace = *fIt;

        if (tempFace == nullptr || tempFace->oneEdge == nullptr)
        {
            continue;
        }

        // Use a flag to skip the final addFace call if corruption is found
        bool skipFace = false;
        std::vector<Vertex*> vertices;

        // --- 1. HALF-EDGE TRAVERSAL FOR ORDERED VERTEX LIST ---

        // The half-edge traversal (do/while loop) is the standard and correct way
        // to get ordered vertices for the new face, as the simple tempFace->vertices
        // may be unordered after some operations. We must use the half-edge structure.

        Edge* firstEdge = tempFace->oneEdge;
        Edge* currEdge = firstEdge;
        Edge* nextEdge = nullptr;
        Vertex* tempv = nullptr;

        size_t loop_count = 0;
        size_t max_loops = tempFace->vertices.size() + 2; // Safety guard

        do
        {
            if (currEdge == nullptr)
            {
               
               
                std::cout << "WARNING: Null edge pointer during half-edge traversal. Skipping face."          
                    << std::endl;
                skipFace = true;
                break;
            }
            if (loop_count++ >= max_loops)
            {
                std::cout << "ERROR: Infinite loop detected in face traversal. Skipping face."
                          << std::endl;
                skipFace = true;
                break;
            }

            // Determine the next vertex (tempv) and the next edge (nextEdge)
            if (tempFace == currEdge->fa)
            {
                tempv = currEdge->vb;
                nextEdge = currEdge->nextVbFa;
            }
            else if (tempFace == currEdge->fb)
            {
                    // We rely on the assert(tempFace==currEdge->fb) being true here
                    if (currEdge->mobius)
                {
                    tempv = currEdge->vb;
                    nextEdge = currEdge->nextVbFb;
                }
                else
                {
                    tempv = currEdge->va;
                    nextEdge = currEdge->nextVaFb;
                }
            }
            else
            {
                // The face in the half-edge structure is neither fa nor fb (CRITICAL CORRUPTION)
                std::cout << "ERROR: Corrupt Face/Edge reference. Skipping face." << std::endl;
                std::cout << tempFace->name << ", " << currEdge->fa->name << ", "
                          << currEdge->fb->name << "\n";
                if (currEdge->fa && currEdge->fb)
                    std::cout << "";
                else
                    std::cout << "\nEdge missing an fa/fb\n";
                if (currEdge->nextVaFa && currEdge->nextVaFb)
                    std::cout << "";
                else
                    std::cout << "\nEdge missing an nextVaFa/Fb\n";
                if (currEdge->nextVbFa && currEdge->nextVbFb)
                    std::cout << "";
                else
                    std::cout << "\nEdge missing an nextVbFa/Fb\n";
        
                skipFace = true;
                break;
            }

            // CRITICAL LOOKUP: Map the old Vertex* (tempv) to the new Vertex*
            if (oldToNewVertMap.count(tempv))
            {
                // Push the new, safe vertex pointer onto the list
                vertices.push_back(oldToNewVertMap[tempv]);
            }
            else
            {
                std::cout
                    << "CRITICAL ERROR: Unmapped vertex found in half-edge list. Skipping face."
                    << std::endl;
                skipFace = true;
                break;
            }

            currEdge = nextEdge;
        } while (currEdge != firstEdge);

        // --- 2. ADD FACE AND TRANSFER PROPERTIES ---

        if (!skipFace && vertices.size() >= 3)
        {
            size_t old_size = newMesh.faceList.size();

            // Your original code contained a redundant/buggy loop that used tempFace->vertices
            // followed by the correct half-edge loop. We only use the half-edge list now.

            newMesh.addFace(vertices); // This may fail silently

            // --- FINAL CRASH CHECK ---
            if (newMesh.faceList.size() > old_size)
            {
                // Success! Now it's safe to access the last element.
                Face* newFace = newMesh.faceList.back();

                newFace->user_defined_color = (*fIt)->user_defined_color;
                newFace->color = (*fIt)->color;
                newFace->backcolor = (*fIt)->backcolor;
                newFace->name = (*fIt)->name;
                newFace->surfaceName = (*fIt)->surfaceName;
                newFace->backfaceName = (*fIt)->backfaceName;
            }
            else
            {
                // addFace silently failed (e.g., topology error from vertices list)
                std::cout << "WARNING: addFace silently failed in copy, skipping property transfer."
                          << std::endl;
            }
        }
    } // End of Face loop

    // ... (The Edge Sharpness transfer loop uses findEdge, which relies on the vertex map,
    //      but should be safe now that vertices are correctly mapped and indexed) ...

   // Transfer edge sharpness from the original mesh to the copied mesh.
// This must happen after all copied faces have been added, because addFace()
// is what rebuilds newMesh.edgeList.
for (Edge* newEdge : newMesh.edgeList)
{
    if (!newEdge || !newEdge->v0() || !newEdge->v1())
    {
        continue;
    }

    Edge* oldEdge = findEdge(newEdge->v0()->name, newEdge->v1()->name, false);

    if (!oldEdge)
    {
        std::cout << "[copy] Could not transfer sharpness; old edge not found: "
                  << newEdge->v0()->name << " - "
                  << newEdge->v1()->name << std::endl;

        newEdge->sharpness = 0.0f;
        newEdge->isSharp = false;
        continue;
    }

    newEdge->sharpness = oldEdge->sharpness;
    newEdge->isSharp = oldEdge->sharpness > 0.0f;

    if (newEdge->sharpness > 0.0f)
    {
        std::cout << "[copy] transferred sharpness "
                  << newEdge->sharpness << " to edge "
                  << newEdge->v0()->name << " - "
                  << newEdge->v1()->name << std::endl;
    }
}

    newMesh.buildBoundary();
    newMesh.computeNormals(isPolyline);
    return newMesh;
}

Edge* findMeshinClass(Face* tempFace, Edge* currEdge) {
    Vertex* tempv;
    Edge* nextEdge;
    int c;
    if (tempFace == currEdge->fa)
    {
        tempv = currEdge->vb;
        nextEdge = currEdge->nextVbFa;
        c = 1;
    }
    else
    {
        assert(tempFace == currEdge->fb);
        if (currEdge->mobius)
        {
            tempv = currEdge->vb;
            nextEdge = currEdge->nextVbFb;
            c = 2;
        }
        else
        {
            tempv = currEdge->va;
            nextEdge = currEdge->nextVaFb;
            c = 3;
        }
    }
    return nextEdge;
}
void Mesh::setGlobalParameter(std::unordered_map<std::string, Parameter>* params) { this->params = params; }

void Mesh::addParam(Parameter* param) { influencingParams.push_back(param); }

Vertex* Mesh::findVertexInThisMesh(std::string name)
{
    for (Vertex*& v : vertList)
    {
        // cout << v->name << endl;
        // cout << name << endl;
        if (v->name == name)
        {
            return v;
        }
    }
    return NULL;
}

bool Mesh::deleteFaceInThisMesh(std::string name)
{
    for (Face*& f : faceList)
    {
        // cout<<f->name<<" ";
        if (f->name == name)
        {
            deleteFace(f);
            return true;
        }
    }
    return false;
}

void Mesh::deleteVertex(Vertex* v)
{
    bool foundThisVertex = false;
    int counter = 0;
    for (Vertex* nextVert : vertList)
    {
        if (foundThisVertex)
        {
            nextVert->ID -= 1;
        }
        else if (nextVert == v)
        {
            foundThisVertex = true;
        }
        else
        {
            counter++;
        }
    }
    if (foundThisVertex)
    {
        vertList.erase(vertList.begin() + counter);
    }
    delete v;
}

void Mesh::deleteFace(Face* face)
{
    bool foundThisFace = false;
    int counter = 0;
    for (Face* nextFace : faceList)
    {
        if (foundThisFace)
        {
            nextFace->id -= 1;
        }
        else if (nextFace == face)
        {
            foundThisFace = true;
        }
        else
        {
            counter++;
        }
    }
    if (foundThisFace)
    {
        faceList.erase(faceList.begin() + counter);
    }
    std::vector<Edge*> removeEdgeList;
    removeEdgeList.clear();
    Edge* firstEdge = face->oneEdge;
    Edge* currEdge = firstEdge;
    Edge* nextEdge;
    do
    {
        if (face == currEdge->fa)
        {
            nextEdge = currEdge->nextVbFa;
            if (currEdge->fb != NULL)
            {
                if (currEdge->mobius)
                {
                    currEdge->nextVaFa = currEdge->nextVaFb;
                    currEdge->nextVbFa = currEdge->nextVbFb;
                    currEdge->mobius = false;
                    // Maybe we need to remark the end vertex non-Mobius here.
                }
                else
                {
                    /*Switch the va and vb, also need to change in the edgetable. */
                    std::unordered_map<Vertex*, std::vector<Edge*>>::iterator vIt;
                    std::vector<Edge*>::iterator eIt;
                    vIt = edgeTable.find(currEdge->va);
                    if (vIt != edgeTable.end())
                    {
                        for (eIt = (vIt->second).begin(); eIt < (vIt->second).end(); eIt++)
                        {
                            if ((*eIt) == currEdge)
                            {
                                (vIt->second).erase(eIt);
                                break;
                            }
                        }
                    }
                    else
                    {
                        std::cout << "Error, there is a bug in the program!" << std::endl;
                    }
                    vIt = edgeTable.find(currEdge->vb);
                    if (vIt != edgeTable.end())
                    {
                        (vIt->second).push_back(currEdge);
                    }
                    else
                    {
                        std::vector<Edge*> edges;
                        edges.push_back(currEdge);
                        edgeTable[currEdge->vb] = edges;
                    }
                    Vertex* temp = currEdge->va;
                    currEdge->va = currEdge->vb;
                    currEdge->vb = temp;
                    currEdge->nextVaFa = currEdge->nextVbFb;
                    currEdge->nextVbFa = currEdge->nextVaFb;
                }
                currEdge->fa = currEdge->fb;
                currEdge->nextVaFb = NULL;
                currEdge->nextVbFb = NULL;
                currEdge->fb = NULL;
            }
            else
            {
                currEdge->nextVaFa = NULL;
                currEdge->nextVbFa = NULL;
                currEdge->fa = NULL;
                removeEdgeList.push_back(currEdge);
            }
        }
        else
        {
            if (currEdge->mobius)
            {
                nextEdge = currEdge->nextVbFb;
                currEdge->mobius = false;
            }
            else
            {
                nextEdge = currEdge->nextVaFb;
            }
            currEdge->nextVaFb = NULL;
            currEdge->nextVbFb = NULL;
            currEdge->fb = NULL;
        }
        currEdge = nextEdge;
    } while (currEdge != firstEdge);
    for (Edge* edge : removeEdgeList)
    {
        deleteEdge(edge);
    }
    delete face;
    return;
}

void Mesh::deleteEdge(Edge* edge)
{
    std::vector<Edge*>::iterator eIt;
    for (eIt = edgeList.begin(); eIt != edgeList.end(); eIt++) {
        if (*eIt == edge)
            edgeList.erase(eIt);
    }
    std::unordered_map<Vertex*, std::vector<Edge*>>::iterator vIt;
    vIt = edgeTable.find(edge->va);
    bool foundEdge = false;
    if (vIt != edgeTable.end())
    {
        for (eIt = (vIt->second).begin(); eIt < (vIt->second).end(); eIt++)
        {
            if ((*eIt) == edge)
            {
                foundEdge = true;
                (vIt->second).erase(eIt);
                break;
            }
        }
    }
    if (!foundEdge)
    {
        std::cout << "Error: You can't delete this edge. Check the program!" << std::endl;
    }
    else
    {
        /* Also need to settle edgeTable here. We also need to set the oneEdge pointer
         * for the affected vertex when edge is deleted. */
        if (edge->va->oneEdge == edge)
        {
            foundEdge = false;
            if ((vIt->second).size() > 0)
            {
                edge->va->oneEdge = *((vIt->second).begin());
                foundEdge = true;
            }
            else
            {
                for (vIt = edgeTable.begin(); vIt != edgeTable.end(); vIt++)
                {
                    for (eIt = (vIt->second).begin(); eIt < (vIt->second).end(); eIt++)
                    {
                        if ((*eIt)->vb == edge->va)
                        {
                            edge->va->oneEdge = (*eIt);
                            foundEdge = true;
                            break;
                        }
                    }
                }
            }
            if (!foundEdge)
            {
                std::cout << "Warning: Your deletion has deleted a vertex." << std::endl;
                edge->va->oneEdge = NULL;
                deleteVertex(edge->va);
            }
        }
        if (edge->vb->oneEdge == edge)
        {
            foundEdge = false;
            vIt = edgeTable.find(edge->vb);
            if (vIt != edgeTable.end() && (vIt->second).size() > 0)
            {
                edge->vb->oneEdge = *((vIt->second).begin());
                foundEdge = true;
            }
            else
            {
                for (vIt = edgeTable.begin(); vIt != edgeTable.end(); vIt++)
                {
                    for (eIt = (vIt->second).begin(); eIt < (vIt->second).end(); eIt++)
                    {
                        if ((*eIt)->vb == edge->vb)
                        {
                            edge->vb->oneEdge = (*eIt);
                            foundEdge = true;
                            break;
                        }
                    }
                }
            }
            if (!foundEdge)
            {
                std::cout << "Warning: Your deletion has deleted a vertex." << std::endl;
                edge->vb->oneEdge = NULL;
                deleteVertex(edge->vb);
            }
        }
    }

    /* Also need to remove the key in edgeTable if it does not exist anymore. */
    delete (edge);
    return;
}

void Mesh::updateVertListAfterDeletion()
{
    vertList.clear();
    bool foundVa;
    bool foundVb;
    std::unordered_map<Vertex*, std::vector<Edge*>>::iterator vIt;
    for (vIt = edgeTable.begin(); vIt != edgeTable.end(); vIt++)
    {
        for (Edge* edge : (vIt->second))
        {
            foundVa = false;
            foundVb = false;
            for (Vertex* v : vertList)
            {
                if (v == edge->va)
                {
                    foundVa = true;
                }
                if (v == edge->vb)
                {
                    foundVb = true;
                }
                if (foundVa && foundVb)
                {
                    break;
                }
            }
            if (!foundVa)
            {
                vertList.push_back(edge->va);
            }
            if (!foundVb)
            {
                vertList.push_back(edge->vb);
            }
        }
    }
    return;
}

void Mesh::setBoundaryEdgeToNull(Vertex* v)
{
    /* Traverse around v and find boundary edges.
     * Set the nextFb pointers to NULL for those edges. */
    std::vector<Edge*> edgeAtThisPoint;
    std::unordered_map<Vertex*, std::vector<Edge*>>::iterator vIt;
    vIt = edgeTable.find(v);
    if (vIt != edgeTable.end())
    {
        edgeAtThisPoint = vIt->second;
        for (Edge* e : edgeAtThisPoint)
        {
            if ((e->fb) == NULL)
            {
                e->nextVaFb = NULL;
                e->nextVbFb = NULL;
            }
        }
    }
    else
    {
        std::cout << "Error: The Vertex doesn't belongs to this Mesh. Debug here." << std::endl;
    }
}
int Mesh::n_faces() {
    return faceList.size();
}
int Mesh::n_vertices() {
    return vertList.size();
}
std::vector<Face*> Mesh::faces() {
    return faceList;
}
int Mesh::n_edges() {
    return edgeList.size();
}
std::vector<Edge*> Mesh::edges() {
    return edgeList;
}

