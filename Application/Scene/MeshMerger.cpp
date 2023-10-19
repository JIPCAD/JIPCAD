#include "MeshMerger.h"
#include "OffsetRefiner.h"
#include "ShellRefiner.h"
#include "Subdivision.h"

#include <unordered_map>
using namespace std;


namespace Nome::Scene
{
DEFINE_META_OBJECT(CMeshMerger)
{
    BindNamedArgument(&CMeshMerger::Level, "sd_level", 0);
    BindNamedArgument(&CMeshMerger::Height, "height", 0);
    BindNamedArgument(&CMeshMerger::Width, "width", 0);

}

inline static const float Epsilon = 0.01f;

void CMeshMerger::UpdateEntity()
{
    if (!IsDirty())
        return;
    subdivisionLevel = Level.GetValue(0);
    Super::UpdateEntity();
    // Update is manual, so this entity has a dummy update method

    SetValid(true);
}

void CMeshMerger::ExportAsStl(QString filename) {
    ofstream file;
    file.open (filename.toStdString());
    file << "solid\n";
    vector<Face*>::iterator fIt;
    for(fIt = currMesh.faceList.begin(); fIt < currMesh.faceList.end(); fIt++) {
        Face * currFace = (*fIt);
        Edge * firstEdge = currFace -> oneEdge;
        Edge * currEdge;
        if(firstEdge == NULL) {
            cout<<"ERROR: This face does not have a sideEdge."<<endl;
            exit(0);
        }
        Vertex * v0, * v1, * v2;
        if(currFace == firstEdge -> fa) {
            v0 = firstEdge -> va;
            currEdge = firstEdge -> nextVbFa;
        } else {
            if(firstEdge -> mobius) {
                v0 = firstEdge -> va;
                currEdge = firstEdge -> nextVbFb;
            } else {
                v0 = firstEdge -> vb;
                currEdge = firstEdge -> nextVaFb;
            }
        }
        tc::Vector3 p0 = v0 -> position;
        if(currEdge == NULL) {
            cout<<"ERROR: This face contains only one edge and can not be drawn."<<endl;
        }
        do {
            Edge * nextEdge;
            if(currFace == currEdge -> fa) {
                v1 = currEdge -> va;
                v2 = currEdge -> vb;
                nextEdge = currEdge -> nextVbFa;
            } else {
                if(currEdge -> mobius) {
                    v1 = currEdge -> va;
                    v2 = currEdge -> vb;
                    nextEdge = currEdge -> nextVbFb;
                } else {
                    v1 = currEdge -> vb;
                    v2 = currEdge -> va;
                    nextEdge = currEdge -> nextVaFb;
                }
            }
            if(v2 != v0) {
                tc::Vector3 faceNormal = getNormal3Vertex(v0->position, v1->position, v2->position);
                file << "  facet normal "<<faceNormal.x<<" "<<faceNormal.y<<" "<<faceNormal.z<<"\n";
                file << "    outer loop\n";
                tc::Vector3 p1 = v1 -> position;
                tc::Vector3 p2 = v2 -> position;
                file << "      vertex " << p0.x << " "<< p0.y << " " << p0.z<<"\n";
                file << "      vertex " << p1.x << " "<< p1.y << " " << p1.z<<"\n";
                file << "      vertex " << p2.x << " "<< p2.y << " " << p2.z<<"\n";
                file << "    endloop\n";
                file << "  endfacet\n";
            }
            currEdge = nextEdge;
        } while (currEdge != firstEdge);
    }
    file << "endsolid\n";
}
std::vector<std::string> CMeshMerger::splitString(const std::string& str, const char delim)
{
    std::vector<std::string> result;
    std::string::size_type start = 0;
    std::string::size_type end = str.find(delim);
    while (end != std::string::npos)
    {
        result.push_back(str.substr(start, end - start));
        start = end + 1;
        end = str.find(delim, start);
    }
    result.push_back(str.substr(start));
    return result;
}
void CMeshMerger::Shell(std::string f) { 
    DSMesh otherMesh = MergedMesh.newMakeCopy();
    Face* shellFace;
    bool selected = false;
    std::vector<std::string> strList = splitString(f, '.');
    std::string fName = strList.at(strList.size() - 1);
    for (auto flt = otherMesh.faceList.begin(); flt < otherMesh.faceList.end(); flt++)
    {
        Face* shell = *flt;
        if (shell->name == fName)
        {
            shellFace = shell;
            selected = true;
        }
    }
    if (!selected)
    {
        shellFace = otherMesh.faceList.at(0);
    }
    doShell(otherMesh, shellFace);
    currMesh = otherMesh.newMakeCopy();
    try
    {
        currMesh.computeNormals();
        currMesh.buildBoundary();
        std::cout << "Build completed successfully. Done with everything." << std::endl;
    }
    catch (std::exception& e)
    {
        std::cout << " shell build failed: Please do one of the following:"
                  << std::endl;
    }
}
void CMeshMerger::doShell(DSMesh & _m, Face* f) {
    double height = Height.GetValue(shellH);
    double width = Width.GetValue(shellW);
    if (height <= 0 && width <= 0)
    {
        return;
    }
    try
    {
        _m.deleteFace(f);
    }
    catch (std::exception& e)
    {
        std::cout << "face deletion failed, there is no face available to be deleted." << std::endl;
    }
    
    CShellRefiner shellRefiner(_m);
    shellRefiner.Refine(height, width);
    _m.clear(); // TODO: is this not doing anyhting???

    std::vector<Vertex*> vertices = shellRefiner.GetVertices();
    std::vector<Face*> faces = shellRefiner.getFaces();

    // Offset verts and faces
    printf("============ output verts and faces ======\n"); // TODO: debug below...
    // for (int index = 0; index < faces.size(); index++)
    for (auto face : faces)
    {
        std::vector<Vertex*> newVerts;
        for (int i = 0; i < face->vertices.size(); i++)
        {
            auto vert = face->vertices[i];
            Vertex* newVert = new Vertex(vert->position.x, vert->position.y, vert->position.z,
                                         _m.vertList.size());
            newVert->name =
                "shellVert" + std::to_string(i); // Randy this was the bug. Need to name the Vert
                                                  // before adding it! Fix this logic.
            _m.addVertex(newVert);
            newVerts.push_back(newVert);
        }
        _m.addFace(newVerts);
    }

    //_m.buildBoundary(); // Randy added this on 2/26
    //_m.computeNormals();

}
void CMeshMerger::Catmull()
{
    bool needSubdivision = subdivisionLevel != 0;
    //bool needOffset = (Width.GetValue(0) != 0 || Height.GetValue(0) != 0);
    bool needOffset = offsetIdent;
    // std::cout << std::to_string(Width.GetValue(0)).c_str() << '\n' << std::endl;
    // std::cout << std::to_string(Height.GetValue(0)).c_str() << std::endl;

    if ((!needSubdivision && !needOffset) || MergedMesh.vertList.empty() && currMesh.isEmpty())//.vertices_empty()) Randy changed the commented out method
    {
        //nothing to do
        return;
    }
    WireFrames.clear();
    // OpenMesh::Subdivider::Uniform::CatmullClarkT<CMeshImpl> catmull; //
    // https://www.graphics.rwth-aachen.de/media/openmesh_static/Documentations/OpenMesh-4.0-Documentation/a00020.html
    // Execute 2 subdivision steps
    DSMesh otherMesh = MergedMesh.newMakeCopy();
    // catmull.attach(otherMesh);
    // prepare(otherMesh);

    if (needSubdivision)
    {
        subdivide(otherMesh, subdivisionLevel); //, isSharp); // Randy commented this out for now. add back asap 
        std::cout << "Apply catmullclark subdivision, may take some time..." << std::endl;
    }
    if (needOffset)
    {
        offset(otherMesh, h, w);
        std::cout << "Apply offset, may take some time..." << std::endl;
    }
    currMesh = otherMesh.newMakeCopy();
    //subdivide(currMesh, subdivisionLevel);
    // ccSubdivision(3);
    try
    {
        currMesh.buildBoundary();
        currMesh.computeNormals();
        //MergedMesh = merge
        std::cout << "done with everything" << std::endl;
    }
    catch (std::exception& e)
    {
        std::cout << "catmul clark subdivision failed: Please do one of the following:" << std::endl;

    }
    
    
}


void CMeshMerger::MergeIn(CMeshInstance& meshInstance, bool shouldMergePoints)
{
    auto tf = meshInstance.GetSceneTreeNode()->L2WTransform.GetValue(
        tc::Matrix3x4::IDENTITY); // The transformation matrix is the identity matrix by default
    auto& otherMesh = meshInstance.GetDSMesh(); // Getting OpeshMesh implementation of a mesh. This

    // allows us to traverse the mesh's vertices/faces
    auto meshClass =
        meshInstance.GetSceneTreeNode()->GetOwner()->GetEntity()->GetMetaObject().ClassName();

    if (meshClass == "CPolyline")
    {
        std::cout << "found Polyline entity" << std::endl;
        return; // skip for now, dont merge polyline entities
    }
    if (meshClass == "CBSpline")
    {
        std::cout << "found Bspline entity" << std::endl;
        return; // skip for now, dont merge polyline related entities
    }

    // Copy over all the vertices and check for overlapping
    std::unordered_map<Vertex*, Vertex*> vertMap;
    for (auto otherVert : otherMesh.vertList) // Iterate through all the vertices in the mesh (the non-merger mesh,
        // aka the one you're trying copy vertices from)
    {
        Vector3 localPos = otherVert->position; // localPos is position before transformations
        Vector3 worldPos = tf * localPos; // worldPos is the actual position you see in the grid
        auto [closestVert, distance] = FindClosestVertex(
            worldPos); // Find closest vertex already IN MERGER mesh, not the actual mesh. This is

        // to prevent adding two merger vertices in the same location!

        if (distance < Epsilon && shouldMergePoints)
        { // this is to check for cases where there is an overlap (two vertices lie in the exact
            // same world space coordinate). We only want to create one merger vertex at this
            // location!
            vertMap[otherVert] =
                closestVert; // just set vi to the closestVert (which is a merger vertex
            // in the same location added in a previous iteration)
            closestVert->sharpness =
                std::max(closestVert->sharpness, otherVert->sharpness);
            printf("set sharpness: %f\n", closestVert->sharpness);
        }
        else // Else, we haven't added a vertex at this location yet. So lets add_vertex to the
            // merger mesh.
        {
            Vertex* copiedVert =
                new Vertex(worldPos.x, worldPos.y, worldPos.z, MergedMesh.nameToVert.size()); // project add offset
            copiedVert->name = "copiedVert" + std::to_string(MergedMesh.nameToVert.size()); // Randy this was causing the bug!!!!!!! the name
            // was the same. so nameToVert remained size == 1
            MergedMesh.addVertex(copiedVert); // Project AddOffset
            vertMap[otherVert] = copiedVert; // Map actual mesh vertex to merged vertex.This
            // dictionary is useful for add face later.
            std::string vName = "v" + std::to_string(VertCount);
            ++VertCount; // VertCount is an attribute for this merger mesh. Starts at 0.
            copiedVert->sharpness = otherVert->sharpness;
        }
    }

    // Add faces and create a face mesh for each
    for (auto otherFace : otherMesh.faceList) // Iterate through all the faces in the mesh (that is, the non-merger
        // mesh, aka the one you're trying to copy faces from)
    {
        std::vector<Vertex*> verts;
        for (auto vert : otherFace->vertices) // otherMesh vertices
        { // iterate through all the vertices on this face
            verts.emplace_back(vertMap[vert]);
        } // Add the vertex handles
        MergedMesh.addFace(verts); // Project AddOffset
        std::string fName = "v" + std::to_string(FaceCount);
        FaceCount++;
    }


    for (auto edge : otherMesh.edges()) //Iterate through all the edges in the mesh
    {
        auto* mergedEdge = MergedMesh.findEdge(vertMap[edge->v0()], vertMap[edge->v1()], false);

        std::vector<Vertex*> MergedEdgeVertices;
        MergedEdgeVertices.push_back(vertMap[edge->v0()]);
        MergedEdgeVertices.push_back(vertMap[edge->v1()]);
        WireFrames.push_back(MergedEdgeVertices);

        try
        {
            //Aaron's edit
            if (mergedEdge == nullptr)
            {
                mergedEdge = new Edge(edge->v0(), edge->v1());
                mergedEdge->sharpness = edge->sharpness;
            }
            else
            {
                mergedEdge->sharpness = std::max(edge->sharpness, mergedEdge->sharpness);
            }
            
        }
        catch (int e)
        {
            std::cerr << "When try to merge in sharpness the edges don't match" << e << '\n';
        }
    }
    otherMesh.visible = false;
    MergedMesh.buildBoundary();
    MergedMesh.computeNormals();
    currMesh = MergedMesh.newMakeCopy();
}


// Find closest vertex in current mesh's vertices
std::pair<Vertex*, float> CMeshMerger::FindClosestVertex(const tc::Vector3& pos)
{
    Vertex* result;
    float minDist = std::numeric_limits<float>::max();
    // TODO: linear search for the time being
    for (const auto& v : MergedMesh.vertList) // Project AddOffset
    {
        Vector3 pp = v->position;
        float dist = pos.DistanceToPoint(pp);
        if (dist < minDist)
        {
            minDist = dist;
            result = v;
        }
    }
    return { result, minDist };
}



// offset only added here
// Randy changed it to use DSMesh
bool CMeshMerger::offset(DSMesh & _m)
{

    double height = Height.GetValue(h);
    double width = Width.GetValue(w);
    if (height <= 0 && width <= 0)
    {
        return true;
    }
    COffsetRefiner offsetRefiner(_m, offsetFlag);
    offsetRefiner.Refine(height, width);
    _m.clear(); // TODO: is this not doing anyhting???

    std::vector<Vertex*> vertices = offsetRefiner.GetVertices();
    std::vector<Face*> faces = offsetRefiner.GetFaces();


    // Offset verts and faces
    printf("============ output verts and faces ======\n"); // TODO: debug below...
    // for (int index = 0; index < faces.size(); index++)
    for (auto face : faces)
    {
        std::vector<Vertex*> newVerts;
        for (int i = 0; i < face->vertices.size(); i++)
        {
            auto vert = face->vertices[i];
            Vertex* newVert = new Vertex(vert->position.x, vert->position.y, vert->position.z,
                                         _m.vertList.size());
            newVert->name = "offsetVert" + std::to_string(i); // Randy this was the bug. Need to name the Vert before adding it! Fix this logic.
            _m.addVertex(newVert);
            newVerts.push_back(newVert);
        }
        _m.addFace(newVerts);
    }


   //_m.buildBoundary(); // Randy added this on 2/26
    //_m.computeNormals();
    return true;
}

bool CMeshMerger::offset(DSMesh& _m, double height, double width)
{

    if (height <= 0 && width <= 0)
    {
        return true;
    }
    COffsetRefiner offsetRefiner(_m, offsetFlag);
    offsetRefiner.Refine(height, width);
    _m.clear(); // TODO: is this not doing anyhting???

    std::vector<Vertex*> vertices = offsetRefiner.GetVertices();
    std::vector<Face*> faces = offsetRefiner.GetFaces();

    // Offset verts and faces
    printf("============ output verts and faces ======\n"); // TODO: debug below...
    // for (int index = 0; index < faces.size(); index++)
    for (auto face : faces)
    {
        std::vector<Vertex*> newVerts;
        for (int i = 0; i < face->vertices.size(); i++)
        {
            auto vert = face->vertices[i];
            Vertex* newVert = new Vertex(vert->position.x, vert->position.y, vert->position.z,
                                         _m.vertList.size());
            newVert->name =
                "offsetVert" + std::to_string(i); // Randy this was the bug. Need to name the Vert
                                                  // before adding it! Fix this logic.
            _m.addVertex(newVert);
            newVerts.push_back(newVert);
        }
        _m.addFace(newVerts);
    }

    //_m.buildBoundary(); // Randy added this on 2/26
    //_m.computeNormals();
    return true;
}

void CMeshMerger::MergeClear() {
    currMesh.clear();
    MergedMesh.clear();
}


bool CMeshMerger::subdivide(DSMesh& _m, unsigned int n)
{
    // Instantiate a Far::TopologyRefiner from the descriptor
    Far::TopologyRefiner * refiner = GetRefiner(_m, isSharp);

    Far::TopologyRefiner::UniformOptions uniop(n);
    uniop.orderVerticesFromFacesFirst = true;
    refiner->RefineUniform(uniop);

    std::vector<Vertex> vbuffer(refiner->GetNumVerticesTotal());
    Vertex * verts = &vbuffer[0];

    for (auto v_itr : _m.vertList) {
        verts[v_itr->ID].SetPosition(v_itr->position.x, v_itr->position.y, v_itr->position.z);
    }

    // Interpolate vertex primvar data
    Far::PrimvarRefiner primvarRefiner(*refiner);

    Vertex * src = verts;
    for (int level = 1; level <= n; ++level) {
        Vertex * dst = src + refiner->GetLevel(level-1).GetNumVertices();
        primvarRefiner.Interpolate(level, src, dst);
        src = dst;
    }
    _m.clear();
    { // Output OBJ of the highest level refined -----------
        /// to debug
        Far::TopologyLevel const & refLastLevel = refiner->GetLevel(n);
        int nverts = refLastLevel.GetNumVertices();
        int nfaces = refLastLevel.GetNumFaces();

        // Print vertex positions
        int firstOfLastVerts = refiner->GetNumVerticesTotal() - nverts;

        for (int vert = 0; vert < nverts; ++vert) {
            float const * pos = verts[vert + firstOfLastVerts].GetPosition();
            _m.addVertex(pos[0], pos[1], pos[2]);
        }

        // Print faces
        for (int face = 0; face < nfaces; ++face) {
            Far::ConstIndexArray fverts = refLastLevel.GetFaceVertices(face);

            // all refined Catmark faces should be quads
            assert(fverts.size()==4);
            std::vector<Vertex*> vertices;
            for (int i = 0; i < 4; ++i)
            {
                vertices.push_back(_m.vertList.at(fverts[i]));
            }
            _m.addFace(vertices);
            WireFrames.push_back(vertices);
        }
    }
    return true;
}

/// TODO: temporary add the old cc subdivision to subdivide nun-manifold shapes
using namespace std;
// Randy changed from DSMesh to void
void CMeshMerger::ccSubdivision(int level)
{
    DSMesh newMesh;
    for (int i = 0; i < level; i++)
    {
        makeFacePoints(newMesh.vertList);
        makeEdgePoints(newMesh.vertList);
        makeVertexPointsD(newMesh.vertList);
        compileNewMesh(newMesh.faceList);
        setAllNewPointPointersToNull();

        // Horribly messy. Organize this better in the future so subdivision cleans up all data structures accordingly
        for (int i = 0; i < newMesh.faceList.size(); i++)
        {
            Face* currFace = newMesh.faceList[i];
            std::string faceName = "subdivFace" + std::to_string(i);
            currFace->name = faceName;
            newMesh.nameToFace[faceName] = newMesh.faceList[i];
        }

        for (int i = 0; i < newMesh.vertList.size(); i++)
        {
            Vertex * currVert = newMesh.vertList[i];
            std::string vertName = "subdivVert" + std::to_string(i);
            currVert->name = vertName;
            newMesh.nameToVert[vertName] = newMesh.vertList[i];
        }

        currMesh.clear();
        currMesh = newMesh.newMakeCopy();
        newMesh.clear();
    }
    //return currMesh;
}

void CMeshMerger::makeFacePoints(vector<Vertex*>& newVertList)
{
    vector<Face*>::iterator fIt;
    for (fIt = currMesh.faceList.begin(); fIt < currMesh.faceList.end(); fIt++)
    {
        Vertex* newFacePoint = new Vertex;
        Vector3 newFacePointPosition = Vector3(0, 0, 0);
        Face* currFace = (*fIt);
        Edge* firstEdge = currFace->oneEdge;
        if (firstEdge == NULL)
        {
            cout << "ERROR: This face (with ID) does not have a sideEdge." << endl;
            exit(1);
        }
        Edge* currEdge = firstEdge;
        uint counter = 0;
        Vertex* currVert;
        do
        {
            if (currFace == currEdge->fa)
            {
                currVert = currEdge->vb;
                currEdge = currEdge->nextVbFa;
            }
            else if (currFace == currEdge->fb)
            {
                if (currEdge->mobius)
                {
                    currVert = currEdge->vb;
                    currEdge = currEdge->nextVbFb;
                }
                else
                {
                    currVert = currEdge->va;
                    currEdge = currEdge->nextVaFb;
                }
            }
            newFacePointPosition += currVert->position;
            counter += 1;
        } while (currEdge != firstEdge);
        newFacePointPosition /= counter;
        newFacePoint->position = newFacePointPosition;
        newFacePoint->ID = newVertList.size();
        currFace->facePoint = newFacePoint;
        newVertList.push_back(newFacePoint);
    }
}

void CMeshMerger::makeEdgePoints(vector<Vertex*>& newVertList)
{
    vector<Face*>::iterator fIt;
    for (fIt = currMesh.faceList.begin(); fIt < currMesh.faceList.end(); fIt++)
    {
        Face* currFace = (*fIt);
        Edge* firstEdge = currFace->oneEdge;
        Edge* currEdge = firstEdge;
        Vertex* currVert;
        do
        {
            Vertex* newEdgePoint = new Vertex;
            if (currEdge->edgePoint == NULL)
            {
                if (currEdge->isSharp)
                {
                    newEdgePoint->position =
                        (currEdge->va->position + currEdge->vb->position) / (float)2.0;
                }
                else
                {
                    Vertex* faceVert1 = currEdge->fa->facePoint;
                    Vertex* edgeVert1 = currEdge->va;
                    if (currEdge->fb)
                    {
                        Vertex* edgeVert2 = currEdge->vb;
                        Vertex* faceVert2 = currEdge->fb->facePoint;
                        newEdgePoint->position = (faceVert1->position + faceVert2->position
                                                  + edgeVert1->position + edgeVert2->position)
                            / (float)4.0;
                    } else {
                        newEdgePoint->position = (faceVert1->position + edgeVert1->position)
                                / (float)2.0;
                    }
                }
                currEdge->edgePoint = newEdgePoint;
                newEdgePoint->ID = newVertList.size();
                newVertList.push_back(newEdgePoint);
                // cout<<"New Edge Point: ID: "<< newEdgePoint -> ID <<" Position: "<< (newEdgePoint
                // -> position)[0]<<" "<<(newEdgePoint -> position)[1]<<" "<<(newEdgePoint ->
                // position)[2]<<endl;
            }
            if (currFace == currEdge->fa)
            {
                currVert = currEdge->vb;
                currEdge = currEdge->nextVbFa;
            }
            else if (currFace == currEdge->fb)
            {
                if (currEdge->mobius)
                {
                    currVert = currEdge->vb;
                    currEdge = currEdge->nextVbFb;
                }
                else
                {
                    currVert = currEdge->va;
                    currEdge = currEdge->nextVaFb;
                }
            }
        } while (currEdge != firstEdge);
    }
}

void CMeshMerger::makeVertexPointsD(vector<Vertex*>& newVertList)
{
    vector<Vertex*>::iterator vIt;
    Vertex* currVert;
    Vertex* newVertexPoint;
    for (vIt = currMesh.vertList.begin(); vIt < currMesh.vertList.end(); vIt++)
    {
        // cout<<"New Vertex!"<<endl;
        currVert = (*vIt);
        newVertexPoint = new Vertex;
        // cout<<"vertexID: "<<currVert -> ID<<endl;
        Edge* firstEdge = currVert->oneEdge;
        Edge* currEdge = firstEdge;
        Face* currFace = currEdge->fa;
        int sharpEdgeCounter = 0;
        Edge* sharpEdgeI;
        Edge* sharpEdgeK;
        Vector3 facePointAvgPosition = Vector3(0, 0, 0);
        Vector3 edgePointAvgPoistion = Vector3(0, 0, 0);
        int n = 0;
        do
        {
            // cout<<"Now the sharp edge counter is "<<sharpEdgeCounter<<endl;
            // cout<<"here"<<endl<<nextOutEdge -> end -> ID<<endl;
            edgePointAvgPoistion += currEdge->edgePoint->position;
            facePointAvgPosition += currFace->facePoint->position;
            n += 1;
            if (currEdge->isSharp)
            {
                // cout<<"A"<<endl;
                sharpEdgeCounter += 1;
                if (sharpEdgeCounter == 1)
                {
                    sharpEdgeI = currEdge;
                }
                else if (sharpEdgeCounter == 2)
                {
                    sharpEdgeK = currEdge;
                }
                currFace = currEdge->theOtherFace(currFace);
                if (currFace == NULL)
                {
                    // cout<<"A1"<<endl;
                    currEdge = currEdge->nextEdge(currVert, currFace);
                    currFace = currEdge->theOtherFace(currFace);
                    edgePointAvgPoistion += currEdge->edgePoint->position;
                    sharpEdgeCounter += 1;
                    if (sharpEdgeCounter == 2)
                    {
                        sharpEdgeK = currEdge;
                    }
                }
                currEdge = currEdge->nextEdge(currVert, currFace);
            }
            else
            {
                currFace = currEdge->theOtherFace(currFace);
                currEdge = currEdge->nextEdge(currVert, currFace);
            }
        } while (currEdge != firstEdge);
        if (sharpEdgeCounter <= 1)
        {
            facePointAvgPosition /= n;
            edgePointAvgPoistion /= n;
            newVertexPoint->position =
                ((float)(n - 2) * currVert->position + edgePointAvgPoistion + facePointAvgPosition)
                / (float)n;
            // cout<<"this is a normal vertex! "<<newVertexPoint -> position[0] << newVertexPoint ->
            // position [1]<< newVertexPoint -> position[2]<<endl;
        }
        else if (sharpEdgeCounter == 2)
        {
            Vertex* pointI = sharpEdgeI->theOtherVertex(currVert);
            Vertex* pointK = sharpEdgeK->theOtherVertex(currVert);
            newVertexPoint->position =
                (pointI->position + pointK->position + 6.0f * currVert->position) / 8.0f;
            // cout<<"this is a crease vertex! "<<newVertexPoint -> position[0] << newVertexPoint ->
            // position [1]<< newVertexPoint -> position[2]<<endl;;
        }
        else
        {
            newVertexPoint->position = currVert->position;
            // cout<<"this is a conner vertex! "<<newVertexPoint -> position[0] << newVertexPoint ->
            // position [1]<< newVertexPoint -> position[2]<<endl;
        }
        newVertexPoint->ID = newVertList.size();
        currVert->vertexPoint = newVertexPoint;
        newVertList.push_back(newVertexPoint);
        // cout<<"New Vertex Point: ID: "<< newVertexPoint -> ID <<" Position: "<< (newVertexPoint
        // -> position)[0]<<" "<<(newVertexPoint -> position)[1]<<" "<<(newVertexPoint ->
        // position)[2]<<endl;
    }
}

void CMeshMerger::compileNewMesh(vector<Face*>& newFaceList)
{
    vector<Face*>::iterator fIt;
    for (fIt = currMesh.faceList.begin(); fIt < currMesh.faceList.end(); fIt++)
    {
        Face* currFace = (*fIt);
        Edge* firstEdge = currFace->oneEdge;
        Edge* currEdge = firstEdge;
        Edge* nextEdge;
        Edge* previousB;
        Edge* previousEF;
        Edge* edgeA;
        Edge* edgeB;
        Edge* edgeEF;
        Face* newFace;
        bool notFirstFace = false;
        // Split the edges and create "in and out" edges.`
        do
        {
            newFace = new Face;
            // Create edge and set va and vb
            if (currEdge->firstHalf == NULL)
            {
                Edge* newFirstHalf = new Edge;
                Edge* newSecondHalf = new Edge;
                newFirstHalf->va = currEdge->va->vertexPoint;
                newFirstHalf->vb = currEdge->edgePoint;
                newSecondHalf->va = currEdge->edgePoint;
                newSecondHalf->vb = currEdge->vb->vertexPoint;
                currEdge->firstHalf = newFirstHalf;
                currEdge->secondHalf = newSecondHalf;
                newFirstHalf->va->oneEdge = newFirstHalf;
                newSecondHalf->vb->oneEdge = newSecondHalf;
                currEdge->edgePoint->oneEdge = newFirstHalf;
            }
            edgeEF = new Edge;
            edgeEF->va = currEdge->edgePoint;
            edgeEF->vb = currFace->facePoint;
            if (notFirstFace)
            {
                edgeEF->fa = newFace;
                previousEF->fb = newFace;
            }
            if (currFace == currEdge->fa)
            {
                edgeA = currEdge->firstHalf;
                edgeB = currEdge->secondHalf;
                edgeA->nextVbFa = edgeEF;
                edgeB->nextVaFa = edgeEF;
                if (notFirstFace)
                {
                    edgeA->fa = newFace;
                    edgeA->nextVaFa = previousB;
                    if (previousB->vb == edgeA->va)
                    {
                        if (previousB->mobius)
                        {
                            previousB->nextVbFb = edgeA;
                            previousB->fb = newFace;
                        }
                        else
                        {
                            previousB->nextVbFa = edgeA;
                            previousB->fa = newFace;
                        }
                    }
                    else
                    {
                        previousB->nextVaFb = edgeA;
                        previousB->fb = newFace;
                    }
                }
                nextEdge = currEdge->nextVbFa;
                if (currEdge->fb == NULL)
                {
                    edgeA->nextVbFb = edgeB;
                    edgeB->nextVaFb = edgeA;
                    Edge* neighbourboundaryA = currEdge->nextVaFb;
                    Edge* neighbourboundaryB = currEdge->nextVbFb;
                    if (neighbourboundaryA->firstHalf != NULL && edgeA->nextVaFb == NULL)
                    {
                        if (neighbourboundaryA->vb == currEdge->va)
                        {
                            edgeA->nextVaFb = neighbourboundaryA->secondHalf;
                            neighbourboundaryA->secondHalf->nextVbFb = edgeA;
                        }
                        else
                        {
                            edgeA->nextVaFb = neighbourboundaryA->firstHalf;
                            neighbourboundaryA->firstHalf->nextVaFb = edgeA;
                        }
                    }
                    if (neighbourboundaryB->firstHalf != NULL && edgeB->nextVbFb == NULL)
                    {
                        if (neighbourboundaryB->va == currEdge->vb)
                        {
                            edgeB->nextVbFb = neighbourboundaryB->firstHalf;
                            neighbourboundaryB->firstHalf->nextVaFb = edgeB;
                        }
                        else
                        {
                            edgeB->nextVbFb = neighbourboundaryB->secondHalf;
                            neighbourboundaryB->secondHalf->nextVbFb = edgeB;
                        }
                    }
                }
            }
            else
            {
                if (currEdge->mobius)
                {
                    edgeA = currEdge->firstHalf;
                    edgeB = currEdge->secondHalf;
                    edgeA->mobius = true;
                    edgeB->mobius = true;
                    edgeA->va->onMobius = true;
                    edgeB->vb->onMobius = true;
                    edgeA->vb->onMobius = true;
                    nextEdge = currEdge->nextVbFb;
                    edgeA->nextVbFb = edgeEF;
                    edgeB->nextVaFb = edgeEF;
                    if (notFirstFace)
                    {
                        edgeA->fb = newFace;
                        edgeA->nextVaFb = previousB;
                        if (previousB->vb == edgeA->va)
                        {
                            if (previousB->mobius)
                            {
                                previousB->nextVbFb = edgeA;
                                previousB->fb = newFace;
                            }
                            else
                            {
                                previousB->nextVbFa = edgeA;
                                previousB->fa = newFace;
                            }
                        }
                        else
                        {
                            previousB->nextVaFb = edgeA;
                            previousB->fb = newFace;
                        }
                    }
                }
                else
                {
                    edgeA = currEdge->secondHalf;
                    edgeB = currEdge->firstHalf;
                    nextEdge = currEdge->nextVaFb;
                    edgeA->nextVaFb = edgeEF;
                    edgeB->nextVbFb = edgeEF;
                    if (notFirstFace)
                    {
                        edgeA->fb = newFace;
                        edgeA->nextVbFb = previousB;
                        if (previousB->vb == edgeA->vb)
                        {
                            if (previousB->mobius)
                            {
                                previousB->nextVbFb = edgeA;
                                previousB->fb = newFace;
                            }
                            else
                            {
                                previousB->nextVbFa = edgeA;
                                previousB->fa = newFace;
                            }
                        }
                        else
                        {
                            previousB->nextVaFb = edgeA;
                            previousB->fb = newFace;
                        }
                    }
                }
            }
            if (currEdge->isSharp)
            {
                edgeA->isSharp = true;
                edgeB->isSharp = true;
            }
            edgeEF->nextVaFa = edgeA;
            edgeEF->nextVaFb = edgeB;
            if (notFirstFace)
            {
                edgeEF->nextVbFa = previousEF;
                previousEF->nextVbFb = edgeEF;
            }
            currEdge = nextEdge;
            previousB = edgeB;
            previousEF = edgeEF;
            if (notFirstFace)
            {
                newFace->oneEdge = edgeA;
                newFace->id = newFaceList.size();
                newFaceList.push_back(newFace);
            }
            notFirstFace = true;
        } while (currEdge != firstEdge);
        newFace = new Face;
        previousEF->fb = newFace;
        if (currFace == currEdge->fa)
        {
            edgeA = currEdge->firstHalf;
            edgeEF = edgeA->nextVbFa;
            edgeA->fa = newFace;
            edgeA->nextVaFa = previousB;
            if (previousB->vb == edgeA->va)
            {
                if (previousB->mobius)
                {
                    previousB->nextVbFb = edgeA;
                    previousB->fb = newFace;
                }
                else
                {
                    previousB->nextVbFa = edgeA;
                    previousB->fa = newFace;
                }
            }
            else
            {
                previousB->nextVaFb = edgeA;
                previousB->fb = newFace;
            }
        }
        else
        {
            if (currEdge->mobius)
            {
                edgeA = currEdge->firstHalf;
                edgeEF = edgeA->nextVbFb;
                edgeA->fb = newFace;
                edgeA->nextVaFb = previousB;
                if (previousB->vb == edgeA->va)
                {
                    if (previousB->mobius)
                    {
                        previousB->nextVbFb = edgeA;
                        previousB->fb = newFace;
                    }
                    else
                    {
                        previousB->nextVbFa = edgeA;
                        previousB->fa = newFace;
                    }
                }
                else
                {
                    previousB->nextVaFb = edgeA;
                    previousB->fb = newFace;
                }
            }
            else
            {
                edgeA = currEdge->secondHalf;
                edgeEF = edgeA->nextVaFb;
                edgeA->fb = newFace;
                edgeA->nextVbFb = previousB;
                if (previousB->vb == edgeA->vb)
                {
                    if (previousB->mobius)
                    {
                        previousB->nextVbFb = edgeA;
                        previousB->fb = newFace;
                    }
                    else
                    {
                        previousB->nextVbFa = edgeA;
                        previousB->fa = newFace;
                    }
                }
                else
                {
                    previousB->nextVaFb = edgeA;
                    previousB->fb = newFace;
                }
            }
        }
        edgeEF->nextVbFa = previousEF;
        edgeEF->fa = newFace;
        previousEF->nextVbFb = edgeEF;
        newFace->oneEdge = edgeA;
        newFace->id = newFaceList.size();
        newFaceList.push_back(newFace);
        currFace->facePoint->oneEdge = previousEF;
    }
}

void CMeshMerger::setAllNewPointPointersToNull()
{
    for (Vertex* v : currMesh.vertList)
    {
        v->vertexPoint = NULL;
    }
    for (Face* f : currMesh.faceList)
    {
        f->facePoint = NULL;
        Edge* firstEdge = f->oneEdge;
        Edge* currEdge = firstEdge;
        Vertex* currVert;
        do
        {
            currEdge->edgePoint = NULL;
            currEdge->firstHalf = NULL;
            currEdge->secondHalf = NULL;
            if (f == currEdge->fa)
            {
                currVert = currEdge->vb;
                currEdge = currEdge->nextVbFa;
            }
            else if (f == currEdge->fb)
            {
                if (currEdge->mobius)
                {
                    currVert = currEdge->vb;
                    currEdge = currEdge->nextVbFb;
                }
                else
                {
                    currVert = currEdge->va;
                    currEdge = currEdge->nextVaFb;
                }
            }
        } while (currEdge != firstEdge);
    }
}
}
