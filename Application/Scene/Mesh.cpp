#include "Mesh.h"
// Render related
#include "SceneGraph.h"
#include <StringPrintf.h>
#include <StringUtils.h>

namespace Nome::Scene
{

DEFINE_META_OBJECT(CMesh)
{
    // `mesh` command has no properties
    // `object` is currently unhandled
}


CMesh::CMesh() = default;

CMesh::CMesh(std::string name)
    : CEntity(std::move(name))
{
}

void CMesh::MarkDirty()
{
    if (IsDirty())
        return;

    Super::MarkDirty();

    for (auto* inst : InstanceSet)
        inst->MarkDirty();
}

void CMesh::UpdateEntity()
{
    cout << "CMesh's UpdateEntity. Here is Mesh's name: " + GetName() << endl;
    if (!IsDirty())
        return;

    ClearMesh();
    bool isValid = true;
    for (size_t i = 0; i < Faces.GetSize(); i++)
    {
        // We assume the nullptr value is never returned, of course
        auto* face = Faces.GetValue(i, nullptr);
        bool successful = face->AddFaceIntoMesh(this);
        if (!successful)
        {
            isValid = false;
        }
    }

    // Randy added below loop on 12/5. Do I need to add point->AddPointIntoMesh(this)?
    for (size_t i = 0; i < Points.GetSize(); i++)
    {
        auto* point = Points.GetValue(i, nullptr);
        this->AddVertex(point->Name, point->Position);
    }


    // TODO: REMOVE BELOW IN THE FUTURE. ALTHOUGH SHOULD PROBABLY KEEP CURRMESH.BUILDBOUNDARY()
    // Project SwitchDS subdivison debug 
    // Face
    std::cout << "right before subdivison debug" << std::endl;


    currMesh.buildBoundary();
    currMesh.computeNormals(); // Randy added on 1/28. Need this for face normals

    vector<Face*>::iterator fIt;
    int counter = 0;
    for (fIt = currMesh.faceList.begin(); fIt < currMesh.faceList.end(); fIt++)
    {

        Vertex* newFacePoint = new Vertex;
        tc::Vector3 newFacePointPosition = tc::Vector3(0, 0, 0);
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
        newFacePoint->ID = counter;
        cout << "New Face Point: ID: " << newFacePoint->ID
             << " Position: " << (newFacePoint->position).x << " " << (newFacePoint->position).y
             << " " << (newFacePoint->position).z << endl;
        currFace->facePoint = newFacePoint;
        // newVertList.push_back(newFacePoint); temp comment out. this would be the subdivided face
        // point
        counter += 1;
    }

    std::cout << "edge point debug" << std::endl;
    vector<Face*>::iterator fIt2;
    int edgecounter = 0;
    for (fIt2 = currMesh.faceList.begin(); fIt2 < currMesh.faceList.end(); fIt2++)
    {
       // cout << "here" << endl;
        Face* currFace = (*fIt2);
        Edge* firstEdge = currFace->oneEdge;
        Edge* currEdge = firstEdge;
        Vertex* currVert;
      //  cout << "here1" << endl;
        do
        {
           // cout << "here2" << endl;
            edgecounter += 1;
            Vertex* newEdgePoint = new Vertex;
            if (currEdge->edgePoint == NULL)
            {
               // cout << "here3" << endl;
                if (currEdge->isSharp)
                {
                    //cout << "here3.1" << endl;
                    newEdgePoint->position =
                        (currEdge->va->position + currEdge->vb->position) / (float)2.0;
                }
                else
                {

                    Vertex* faceVert1 = currEdge->fa->facePoint;
                   // cout << "here3.21" << endl;
                    Vertex* edgeVert1 = currEdge->va;
                    //cout << "here3.22" << endl;
                    Vertex* edgeVert2 = currEdge->vb;
                    //cout << "here3.23" << endl;
                    cout << (currEdge->fb == NULL) << endl;
                    //cout << " pa" << endl;
                    //cout << currEdge->fb->id << endl;
                    //cout << "here3.230" << endl;
                    Vertex* faceVert2 = currEdge->fb->facePoint; // this causing it to crash
                    //cout << "here3.24" << endl;
                    newEdgePoint->position = (faceVert1->position + faceVert2->position
                                              + edgeVert1->position + edgeVert2->position)
                        / (float)4.0;
                   // cout << "here3.25" << endl;
                }
               // cout << "here4" << endl;
                currEdge->edgePoint = newEdgePoint;
                newEdgePoint->ID = edgecounter;
               //newVertList.push_back(newEdgePoint); // Randy temp comment this out
                // cout<<"New Edge Point: ID: "<< newEdgePoint -> ID <<" Position: "<< (newEdgePoint
                // -> position)[0]<<" "<<(newEdgePoint -> position)[1]<<" "<<(newEdgePoint ->
                // position)[2]<<endl;
            }
            //cout << "here5" << endl;
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
          //  cout << "here6" << endl;
        } while (currEdge != firstEdge);
       // cout << "done with edge point" << endl;
    }

    
    std::cout << "vert point debug removed cause was causing NOME to crash for hierarchnametest. fix in the future" << std::endl;

    // vector<Vertex*>::iterator vIt2;
    //Vertex facePointAvg;
    //Vertex edgePointAvg;
    //Vertex* currVert;
    //Vertex* newVertexPoint;
    //int vertcounter = 0;
    //for (vIt2 = currMesh.vertList.begin(); vIt2 < currMesh.vertList.end(); vIt2++)
    //{
    //    // cout<<"New Vertex!"<<endl;
    //    currVert = (*vIt2);
    //    newVertexPoint = new Vertex;
    //    // cout<<"vertexID: "<<currVert -> ID<<endl;
    //    Edge* firstEdge = currVert->oneEdge;
    //    Edge* currEdge = firstEdge;
    //    Face* currFace = currEdge->fa;
    //    int sharpEdgeCounter = 0;
    //    Edge* sharpEdgeI;
    //    Edge* sharpEdgeK;
    //    tc::Vector3 facePointAvgPosition = tc::Vector3(0, 0, 0);
    //    tc::Vector3 midPointAvgPoistion = tc::Vector3(0, 0, 0);
    //    int n = 0;
    //    do
    //    {
    //        vertcounter += 1;
    //        // cout<<"Now the sharp edge counter is "<<sharpEdgeCounter<<endl;
    //        // cout<<"here"<<endl<<nextOutEdge -> end -> ID<<endl;
    //        midPointAvgPoistion += (currEdge->va->position + currEdge->vb->position) / 2.0f;
    //        facePointAvgPosition += currFace->facePoint->position;
    //        n += 1;
    //        if (currEdge->isSharp)
    //        {
    //            // cout<<"A"<<endl;
    //            sharpEdgeCounter += 1;
    //            if (sharpEdgeCounter == 1)
    //            {
    //                sharpEdgeI = currEdge;
    //            }
    //            else if (sharpEdgeCounter == 2)
    //            {
    //                sharpEdgeK = currEdge;
    //            }
    //            currFace = currEdge->theOtherFace(currFace);
    //            if (currFace == NULL)
    //            {
    //                // cout<<"A1"<<endl;
    //                currEdge = currEdge->nextEdge(currVert, currFace);
    //                currFace = currEdge->theOtherFace(currFace);
    //                midPointAvgPoistion += (currEdge->va->position + currEdge->vb->position) / 2.0f;
    //                sharpEdgeCounter += 1;
    //                if (sharpEdgeCounter == 2)
    //                {
    //                    sharpEdgeK = currEdge;
    //                }
    //            }
    //            currEdge = currEdge->nextEdge(currVert, currFace);
    //        }
    //        else
    //        {
    //            currFace = currEdge->theOtherFace(currFace);
    //            currEdge = currEdge->nextEdge(currVert, currFace);
    //        }
    //    } while (currEdge != firstEdge);
    //    if (sharpEdgeCounter <= 1)
    //    {
    //        facePointAvgPosition /= n;
    //        midPointAvgPoistion /= n;
    //        newVertexPoint->position = ((float)(n - 3) * currVert->position
    //                                    + 2.0f * midPointAvgPoistion + facePointAvgPosition)
    //            / (float)n;
    //        // cout<<"this is a normal vertex! "<<newVertexPoint -> position[0] << newVertexPoint ->
    //        // position [1]<< newVertexPoint -> position[2]<<endl;
    //    }
    //    else if (sharpEdgeCounter == 2)
    //    {
    //        Vertex* pointI = sharpEdgeI->theOtherVertex(currVert);
    //        Vertex* pointK = sharpEdgeK->theOtherVertex(currVert);
    //        newVertexPoint->position =
    //            (pointI->position + pointK->position + 6.0f * currVert->position) / 8.0f;
    //        // cout<<"this is a crease vertex! "<<newVertexPoint -> position[0] << newVertexPoint ->
    //        // position [1]<< newVertexPoint -> position[2]<<endl;;
    //    }
    //    else
    //    {
    //        newVertexPoint->position = currVert->position;
    //        // cout<<"this is a conner vertex! "<<newVertexPoint -> position[0] << newVertexPoint ->
    //        // position [1]<< newVertexPoint -> position[2]<<endl;
    //    }
    //    newVertexPoint->ID = vertcounter;
    //    currVert->vertexPoint = newVertexPoint;
    //    //newVertList.push_back(newVertexPoint); temporary push this back
    //    // cout<<"New Vertex Point: ID: "<< newVertexPoint -> ID <<" Position: "<< (newVertexPoint
    //    // -> position)[0]<<" "<<(newVertexPoint -> position)[1]<<" "<<(newVertexPoint ->
    //    // position)[2]<<endl;
    //}

    std::cout << "done with subdivision debug" << std::endl;


    // Randy added below loop on 12/29
    //for (size_t i = 0; i < Points.GetSize(); i++) {
    //    auto* polyline = Polylines.GetValue(i, nullptr); 

    //    
    //}
    Super::UpdateEntity();
    SetValid(isValid);
    std::cout << "doone with mesh updateentity" << std::endl;
}

void CMesh::Draw(IDebugDraw* draw)
{
    CEntity::Draw(draw);

    if (!LineStrips.empty())
    {
        // Randy added below for loop on 12/27 to draw multiple line strips
        for (auto LineStrip : LineStrips)
        {
            std::vector<Vector3> positions;
            for (auto vHandle : LineStrip)
            {
                const auto& vPos = vHandle->position; // Mesh.point(vHandle);
                positions.emplace_back(vPos.x, vPos.y, vPos.z);
            }

            for (size_t i = 1; i < positions.size(); i++)
            {
                draw->LineSegment(positions[i - 1], positions[i]);
            }
        }
    }
}

Vertex * CMesh::AddVertex(const std::string& name, tc::Vector3 pos)
{
    // Silently fail if the name already exists
    auto iter = NameToVert.find(name);
    if (iter != NameToVert.end())
        return iter->second;

    //Vertex vertex;

    // Comment out for Project SwitchDS
    //vertex = Mesh.add_vertex(CMeshImpl::Point(pos.x, pos.y, pos.z));


    Vertex * currVert = new Vertex(pos.x, pos.y, pos.z, name, NameToVert.size()); // Project SwitchDS // Vertex::Vertex(float x, float y, float z, string assignedName, unsigned long ID)
  //  currVert->position = tc::Vector3(pos.x, pos.y, pos.z); // Project SwitchDS
    //currVert->ID = NameToVert.size(); // Project SwitchDS . hacky implementation
    //currVert->name = name; // ADded this AND FIXED THE BUG AFTER 2 DAYS. THIS WAS IT AND DUE TOCVertexSelector:PointUpdate not finding name match
   
    currMesh.addVertex(currVert); // Project SwitchDS

    // Project SwitchDS
    NameToVert.emplace(name, currVert);
    VertToName.emplace(currVert, name); // Randy added on 10/15


    return currVert;
}

Vector3 CMesh::GetVertexPos(const std::string& name) const
{
    auto iter = NameToVert.find(name);
    Vertex * vertex = iter->second;
    //const auto& pos = Mesh.point(vertex);
    auto pos = vertex->position;
    return Vector3(pos.x, pos.y, pos.z);
}

// Called from AddFaceIntoMesh
void CMesh::AddFace(const std::string& name, const std::vector<std::string>& facePointNames,
                    std::string faceSurfaceIdent)
{
    std::vector<Vertex *> faceVertices;
    for (const std::string& pointName : facePointNames)
    {
        faceVertices.push_back(NameToVert[pointName]);
    }

    AddFace(name, faceVertices, faceSurfaceIdent);
}

void CMesh::AddFace(const std::string& name, const std::vector<Vertex*>& faceDSVerts,
                    std::string faceSurfaceIdent)
{
    Face * newFace = currMesh.addPolygonFace(faceDSVerts, false); // Project SwitchDS . Check if need to reverseOrder = true or false?
    
    //// Randy added this on 12/12 for face entity coloring
     if (faceSurfaceIdent != "") // if the face has a surface color associated with it
        DSFaceWithColor.emplace(newFace, faceSurfaceIdent);   
   
    //Project SwitchDS 
    FaceVertsToFace.emplace(faceDSVerts,newFace); 
    FaceToFaceVerts.emplace(newFace,faceDSVerts); 
    NameToFace.emplace(name, newFace);
    FaceToName.emplace(newFace, name); 
}

void CMesh::AddLineStrip(const std::string& name,
                         const std::vector<Vertex*>& points)
{
    LineStrips.push_back(points);
}

void CMesh::ClearMesh()
{
    // Mesh.clear(); Project SwitchDS
    NameToVert.clear();
    VertToName.clear();
    NameToFace.clear();
    FaceToName.clear(); // Randy added
    FaceVertsToFace.clear(); // Randy added
    FaceToFaceVerts.clear(); // Randy added
    LineStrips.clear();
}

// WARNING this function is not currently used and outdated. leaving here for future use
void CMesh::SetFromData(CMeshImpl mesh, std::map<std::string, Vertex*> vnames,
                        std::map<std::string, Face*> fnames)
{
    Mesh = std::move(mesh);
    NameToVert = std::move(vnames);
    NameToFace = std::move(fnames);
}

bool CMesh::IsInstantiable() { return true; }

CEntity* CMesh::Instantiate(CSceneTreeNode* treeNode) { return new CMeshInstance(this, treeNode); }

AST::ACommand* CMesh::SyncToAST(AST::CASTContext& ctx, bool createNewNode)
{
    if (!createNewNode)
        throw "unimplemented";
    auto* node = ctx.Make<AST::ACommand>(ctx.MakeToken("mesh"), ctx.MakeToken("endmesh"));
    node->PushPositionalArgument(ctx.MakeIdent(GetName()));
    size_t numFaces = Faces.GetSize();
    for (size_t i = 0; i < numFaces; i++)
    {
        this->GetName();
        auto* pFace = Faces.GetValue(i, nullptr);
        // Test whether the face is a sub-entity of mine
        //   For any sub-entity, we also serialize their AST
        //   Otherwise, this should have been an `object` command, which is unimplemented rn
        if (!tc::FStringUtils::StartsWith(pFace->GetName(), this->GetName()))
            throw std::runtime_error("Mesh's child faces corruption");
        node->AddSubCommand(pFace->MakeCommandNode(ctx, node));
    }
    return node;
}

std::string CMeshInstancePoint::GetPointPath() const
{
    return Owner->GetSceneTreeNode()->GetPath() + "." + GetName();
}

CMeshInstance::CMeshInstance(CMesh* generator, CSceneTreeNode* stn)
    : MeshGenerator(generator) // MeshGenerator is the mesh the mesh instance refers to
    , SceneTreeNode(stn)
{
    SetName(tc::StringPrintf("_%s_%s", MeshGenerator->GetName().c_str(), GetName().c_str()));
    MeshGenerator->InstanceSet.insert(this);

    // We listen to the transformation changes of the associated tree node
    TransformChangeConnection = SceneTreeNode->OnTransformChange.Connect(
        std::bind(&CMeshInstance::MarkOnlyDownstreamDirty, this));
}

CMeshInstance::~CMeshInstance()
{
    // TODO: handle this circular reference stuff
    // SceneTreeNode->OnTransformChange.Disconnect(TransformChangeConnection);
    MeshGenerator->InstanceSet.erase(this);
}

void CMeshInstance::MarkDirty()
{
    Super::MarkDirty();
    SelectorSignal.MarkDirty();
}

void CMeshInstance::MarkOnlyDownstreamDirty() { SelectorSignal.MarkDirty(); }

void CMeshInstance::UpdateEntity()
{
    if (!IsDirty())
        return;
    MeshGenerator->UpdateEntity();
    CopyFromGenerator();
    //Mesh.request_vertex_status();
    //Mesh.request_vertex_colors();
    //Mesh.request_edge_status();
    //Mesh.request_face_status();
    //Mesh.request_face_colors(); // Randy added on 10/10 for face selection. May be useful in the
    //                            // future for flat coloring. Currently doesn't do anything
    //for (auto vH : Mesh.vertices())
    //{
    //    Mesh.set_color(vH, { VERT_COLOR });
    //}
    // don't need to set face color because it gets overwritten by material's instancecolor (shader)

    // Add back to delete face later. I think ~100 hundred lines of code deleted here due to incompability with new data structure. 
    // Please look at previous commits to add back DeleteFace
    

    //if (!Mesh.faces_empty())
    //{
    //    Mesh.request_face_normals();
    //    Mesh.request_vertex_normals();
    //    Mesh.update_face_normals();
    //    Mesh.update_vertex_normals();
    //    // Update visual layer geometry
    //}
    //else
    //{
    //    // std::cout << "no faces left in mesh. handle this else condition in the future" <<
    //    // should probably implement this logic in the future OR else may be further bugs
    //}
    FacesToDelete.clear(); // Randy added this on 10/18. Clear it, we have finished deleting them.

    // Randy commented the below section on 10/10. i don't think it does anything ???
    // Construct interactive points
    // CScene* scene = GetSceneTreeNode()->GetOwner()->GetScene();
    // for (auto pair : PickingVerts)
    //{
    //    scene->GetPickingMgr()->UnregisterObj(pair.second.second);
    //    delete pair.second.first;
    //}
    // PickingVerts.clear();

    // for (const auto& pair : NameToVert)
    //{
    //    // Create a picking proxy point for each vertex
    //    CMeshInstancePoint* ip = new CMeshInstancePoint(this);
    //    ip->SetName(pair.first);
    //    const auto& p = Mesh.point(pair.second);
    //    Vector3 pos = { p[0], p[1], p[2] };
    //    ip->SetPosition(GetSceneTreeNode()->L2WTransform.GetValue(Matrix3x4::IDENTITY) * pos);
    //    // Register this picking point with the scene picking mgr
    //    uint32_t id = scene->GetPickingMgr()->RegisterObj(ip);
    //    PickingVerts.emplace(pair.first, std::make_pair(ip, id));
    //}
    Super::UpdateEntity();
    SetValid(MeshGenerator->IsEntityValid());
    std::cout << "DONE with CMeshInstance update entity" << std::endl;
}

void CMeshInstance::Draw(IDebugDraw* draw) { MeshGenerator->Draw(draw); }

CVertexSelector* CMeshInstance::CreateVertexSelector(const std::string& name,
                                                     const std::string& outputName)
{
    auto* selector = new CVertexSelector(name, outputName);
    SelectorSignal.Connect(selector->MeshInstance);
    return selector;
}

// Conceptually, we are copying from the CMesh object
void CMeshInstance::CopyFromGenerator()
{
    //Mesh.clear(); Project SwitchDS
    NameToVert.clear();
    NameToFace.clear();
    FaceToName.clear(); // Randy added
    FaceVertsToFace.clear();
    FaceToFaceVerts.clear(); // Randy added

    //Mesh = MeshGenerator->Mesh; ProjectSwitchDS
    currMesh = MeshGenerator->currMesh; // Project SwitchDS CRUCIAL STEP

    // Since the handle only contains an index, we can just copy
    NameToVert = MeshGenerator->NameToVert;
    VertToName = MeshGenerator->VertToName; // Randy added on 10/15
    NameToFace = MeshGenerator->NameToFace;
    FaceToName = MeshGenerator->FaceToName; // Randy added
    FaceVertsToFace = MeshGenerator->FaceVertsToFace; // Randy added
    FaceToFaceVerts = MeshGenerator->FaceToFaceVerts; // Randy added

    CScene* scene = GetSceneTreeNode()->GetOwner()->GetScene();

    // Randy added on 12/12
    for (auto& pair : MeshGenerator->DSFaceWithColor)
    {
        auto surfaceIdentifier = pair.second;
        if (surfaceIdentifier != "")
        {
            auto surfaceEntity = scene->FindEntity(surfaceIdentifier);
            if (surfaceEntity)
            {
                CSurface* surfaceObject = dynamic_cast<CSurface*>(surfaceEntity.Get());
                std::array<float, 3> color = { surfaceObject->ColorR.GetValue(1.0f),
                                               surfaceObject->ColorG.GetValue(1.0f),
                                               surfaceObject->ColorB.GetValue(1.0f) };
                DSFaceWithColorVector.emplace(pair.first, color);
            }
            else
            {
                std::cout << "Invalid surface color name" << std::endl;
            }
        }
    }
}

// Warning: Still buggy. Use with caution.
std::vector<std::string>
CMeshInstance::RemoveFace(const std::vector<std::string>& faceNames) // Randy added
{
    std::vector<std::string> removedVertName;
    auto instPrefix = GetSceneTreeNode()->GetPath() + ".";

    for (auto faceName : faceNames)
    {
        // if faceName contains instPrefix, then the face is a part of this mesh instance
        if (faceName.find(instPrefix) != std::string::npos)
        {
            // the mesh's face name is the faceName - instprefix
            auto start_position_to_erase = faceName.find(instPrefix);
            auto meshfaceName = faceName.erase(start_position_to_erase, instPrefix.length());

            // if meshfaceName is actually a face within this mesh
            if (NameToFace.find(meshfaceName) != NameToFace.end())
            {
                std::cout << "found face, adding to faces to delete: " + meshfaceName << std::endl;
                FacesToDelete.insert(meshfaceName);
                this->MarkDirty();
                std::vector<std::string> temp;
                temp.push_back(meshfaceName);
                auto temp2 = GetFaceVertexNames(temp);
                removedVertName.insert(removedVertName.end(), temp2.begin(), temp2.end());
            }
        }
    }
    GetSceneTreeNode()->SetEntityUpdated(true); // Randy added this on 10/18. Didn't seem to fix bug?
    return removedVertName;
}

std::vector<std::pair<float, std::string>> CMeshInstance::PickFaces(const tc::Ray& localRay)
{
    std::vector<std::pair<float, std::string>> result;
    auto instPrefix = GetSceneTreeNode()->GetPath() + ".";
    for (const auto& pair : FaceVertsToFace)
    {
        auto points = pair.first;

        std::string facename = FaceToName.at(pair.second);
        auto firstpoint = points[0];
        auto secondpoint = points[1];
        auto thirdpoint = points[2];

        tc::Vector3 pos1 { firstpoint->position.x, firstpoint->position.y, firstpoint->position.z };
        tc::Vector3 pos2 { secondpoint->position.x, secondpoint->position.y, secondpoint->position.z };
        tc::Vector3 pos3 { thirdpoint->position.x, thirdpoint->position.y, thirdpoint->position.z };
        auto testplane = new tc::Plane(pos1, pos2, pos3);

        // tc::Vector3 projected = localRay.Project(pos);
        // Randy note: They all have the same position because the local ray is transformed
        // based on the instance scene node

        auto testdist = localRay.HitDistance(*testplane);

        // WARNING: Doesn't give all combinations. Naive method only works with convex polygons
        // TODO: Optimize this to handle concave polygons
        std::vector<float> hitdistances;

        for (int i = 0; i < points.size(); i++)
        {
            for (int j = i + 1; j < points.size(); j++)
            {
                for (int k = j + 1; k < points.size(); k++)
                {
                    auto firstpoint = points[i];
                    auto secondpoint = points[j];
                    auto thirdpoint = points[k];
                    tc::Vector3 pos1 { firstpoint->position.x, firstpoint->position.y,
                                       firstpoint->position.z };
                    tc::Vector3 pos2 { secondpoint->position.x, secondpoint->position.y,
                                       secondpoint->position.z };
                    tc::Vector3 pos3 { thirdpoint->position.x, thirdpoint->position.y,
                                       thirdpoint->position.z };
                    auto testdist1 = localRay.HitDistance(pos1, pos2, pos3);
                    hitdistances.push_back(testdist1);
                }
            }
        }
        auto mindist = *std::min_element(hitdistances.begin(), hitdistances.end());
        std::cout << "Triangle hit distance:  " + std::to_string(mindist) << std::endl;
        auto instPrefix = GetSceneTreeNode()->GetPath() + ".";
        if (mindist < 100)
        {
            result.emplace_back(mindist, instPrefix + facename);
        }
    }
    // std::sort(result.begin(), result.end());
    for (const auto& sel : result)
    {
        std::cout << "face added to pick list" << std::endl;
        printf("t=%.3f v=%s\n", sel.first, sel.second.c_str());
    }
    return result;
}

std::vector<std::pair<float, std::vector<std::string>>> CMeshInstance::PickPolylines(const tc::Ray& localRay)
{
    std::vector<std::pair<float, std::vector<std::string>>> result;

    auto meshClass = GetSceneTreeNode()->GetOwner()->GetEntity()->GetMetaObject().ClassName();
    auto meshName = GetSceneTreeNode()->GetOwner()->GetName();

    // If it's an already selected edge
    if (meshClass == "CPolyline" || meshClass == "CBSpline")
    {
        auto instPrefix = GetSceneTreeNode()->GetPath() + ".";

        std::vector<Vertex*> edgeVertsOnly;
        for (const auto& pair : VertToName)
            edgeVertsOnly.push_back(pair.first);
        for (int i = 0; i < edgeVertsOnly.size(); i++) // adjacent elements are the edge verts
        {
            std::vector<float> hitdistances;
            std::map<float, std::vector<std::string>> distToNames;

            if (i == edgeVertsOnly.size() - 1)
            { // if at the last point, the last edge is last point connected to first point
                auto firstpoint = edgeVertsOnly[i];
                auto secondpoint = edgeVertsOnly[0];
                tc::Vector3 pos1 { firstpoint->position.x, firstpoint->position.y,
                                   firstpoint->position.z };
                tc::Vector3 pos2 { secondpoint->position.x, secondpoint->position.y,
                                   secondpoint->position.z };


                // naive method, extend the plane out of the line in 6 directions a little
                tc::Vector3 dummy1 = (pos1 + pos2) / 2;
                float offset = 0.170; // TODO: Optimize this to vary by scene
                float testdist1 = std::min(
                    { localRay.HitDistance(pos1, pos2, dummy1 + Vector3 { 0, offset, 0 }),
                      localRay.HitDistance(pos1, pos2, dummy1 + Vector3 { offset, 0, 0 }),
                      localRay.HitDistance(pos1, pos2, dummy1 + Vector3 { 0, 0, offset }),
                      localRay.HitDistance(pos1, pos2, dummy1 + Vector3 { 0, offset, 0 }),
                      localRay.HitDistance(pos1, pos2, dummy1 + Vector3 { offset, 0, 0 }),
                      localRay.HitDistance(pos1, pos2, dummy1 + Vector3 { 0, 0, offset }) });
                hitdistances.push_back(testdist1);
                std::cout << "testdist1a: " + std::to_string(testdist1) << std::endl;
                auto vertname1 = instPrefix + VertToName.at(firstpoint);
                auto vertname2 = instPrefix + VertToName.at(secondpoint);
                std::vector<std::string> names;
                names.push_back(vertname1);
                names.push_back(vertname2);
                distToNames.insert({ testdist1, names });
            }
            else
            {
                auto firstpoint = edgeVertsOnly[i];
                auto secondpoint = edgeVertsOnly[i+1];
                tc::Vector3 pos1 { firstpoint->position.x, firstpoint->position.y,
                                   firstpoint->position.z };
                tc::Vector3 pos2 { secondpoint->position.x, secondpoint->position.y,
                                   secondpoint->position.z };

                // naive method, extend the plane out of the line in 6 directions a little
                tc::Vector3 dummy1 = (pos1 + pos2) / 2; 
                float offset = 0.170; // TODO: Optimize this to vary by scene
                float testdist1 = std::min(
                    { localRay.HitDistance(pos1, pos2, dummy1 + Vector3 { 0, offset, 0 }),
                      localRay.HitDistance(pos1, pos2, dummy1 + Vector3 { offset, 0, 0 }),
                      localRay.HitDistance(pos1, pos2, dummy1 + Vector3 { 0, 0, offset }),
                      localRay.HitDistance(pos1, pos2, dummy1 + Vector3 { 0, offset, 0 }),
                      localRay.HitDistance(pos1, pos2, dummy1 + Vector3 { offset, 0, 0 }),
                      localRay.HitDistance(pos1, pos2, dummy1 + Vector3 { 0, 0, offset }) });
                hitdistances.push_back(testdist1);
                std::cout << "testdist1b: " + std::to_string(testdist1) << std::endl;
                auto vertname1 = instPrefix + VertToName.at(firstpoint);
                auto vertname2 = instPrefix + VertToName.at(secondpoint);
                std::vector<std::string> names;
                names.push_back(vertname1);
                names.push_back(vertname2);
                distToNames.insert({ testdist1, names });
            }
            auto mindist = *std::min_element(hitdistances.begin(), hitdistances.end());
            std::cout << "Triangle hit distance for edge:  " + std::to_string(mindist) << std::endl;

            if (mindist < 100)
            {
                auto hitpointnames = distToNames.at(mindist); // Guaranteed to be two names
                auto name1withoutinstprefix = hitpointnames[0].substr(instPrefix.length());
                auto name2withoutinstprefix = hitpointnames[1].substr(instPrefix.length());

                auto point1 = NameToVert.at(name1withoutinstprefix);
                auto point2 = NameToVert.at(name2withoutinstprefix);

                GetSceneTreeNode()->GetOwner()->SelectNode();
                MarkDirty();
            }
        }
    }
    
    // std::sort(result.begin(), result.end());
    for (const auto& sel : result)
    {
        printf("t=%.3f v1=%s v2 =%s\n", sel.first, sel.second[0].c_str(), sel.second[1].c_str());
    }
    return result;
}

// Pick edges return a vector containing hit distance and the pair of edge vertex names
// NOTE: PickEdges is the buggiest among Pick functions
// Optimize in the future
std::vector<std::pair<float, std::vector<std::string>>>
CMeshInstance::PickEdges(const tc::Ray& localRay)
{
    std::vector<std::pair<float, std::vector<std::string>>> result;

    auto meshClass = GetSceneTreeNode()->GetOwner()->GetEntity()->GetMetaObject().ClassName();
    auto meshName = GetSceneTreeNode()->GetOwner()->GetName();

    // If it's an already selected edge
    if (meshClass == "CPolyline")
    {
        auto instPrefix = GetSceneTreeNode()->GetPath() + ".";
        std::vector<Vertex*> edgeVertsOnly;
        for (const auto& pair : VertToName)
            edgeVertsOnly.push_back(pair.first);
        std::vector<float> hitdistances;
        std::map<float, std::vector<std::string>> distToNames;
        auto firstpoint = edgeVertsOnly[0];
        auto secondpoint = edgeVertsOnly[1];
        tc::Vector3 pos1 { firstpoint->position.x, firstpoint->position.y, firstpoint->position.z };
        tc::Vector3 pos2 { secondpoint->position.x, secondpoint->position.y,
                           secondpoint->position.z };

             
        // naive method, extend the plane out of the line in 6 directions a little
        tc::Vector3 dummy1 = (pos1 + pos2) / 2; 
        float offset = 0.170; // TODO: Optimize this to vary by scene
        float testdist1 = std::min(
            { localRay.HitDistance(pos1, pos2, dummy1 + Vector3 { 0, offset, 0 }),
              localRay.HitDistance(pos1, pos2, dummy1 + Vector3 { offset, 0,  0 }),
              localRay.HitDistance(pos1, pos2, dummy1 + Vector3 { 0, 0, offset}),
              localRay.HitDistance(pos1, pos2, dummy1 + Vector3 { 0, offset, 0 }),
                       localRay.HitDistance(pos1, pos2, dummy1 + Vector3 { offset, 0 , 0}),
                       localRay.HitDistance(pos1, pos2, dummy1 + Vector3 { 0, 0, offset }) });
        hitdistances.push_back(testdist1);
        std::cout << "testdist1b: " + std::to_string(testdist1) << std::endl;
        auto vertname1 = instPrefix + VertToName.at(firstpoint);
        auto vertname2 = instPrefix + VertToName.at(secondpoint);
        std::vector<std::string> names;
        names.push_back(vertname1);
        names.push_back(vertname2);
        distToNames.insert({ testdist1, names });
            
        auto mindist = *std::min_element(hitdistances.begin(), hitdistances.end());
        std::cout << "Triangle hit distance for edge:  " + std::to_string(mindist) << std::endl;

        if (mindist < 100)
        {
            auto hitpointnames = distToNames.at(mindist); // Guaranteed to be two names
            auto name1withoutinstprefix = hitpointnames[0].substr(instPrefix.length());
            auto name2withoutinstprefix = hitpointnames[1].substr(instPrefix.length());

            auto point1 = NameToVert.at(name1withoutinstprefix);
            auto point2 = NameToVert.at(name2withoutinstprefix);

            if (meshName.find("SELECTED") == std::string::npos)
            {
                GetSceneTreeNode()->GetOwner()->SelectNode();
                MarkDirty();
            }
            else // this is a temp selection polyline, deselect by adding to results
            {
                result.emplace_back(mindist, hitpointnames);
            }
        }
        
    }

    // Else, if it's not an already selected polyline
    auto instPrefix = GetSceneTreeNode()->GetPath() + ".";

    for (const auto& pair :
         FaceVertsToFace) // if CPolyline or CBspline, this will silently get skipped
    {
        auto points = pair.first;

        std::string facename = FaceToName.at(pair.second);
        std::cout << "We are currently in " + GetSceneTreeNode()->GetPath() + "'s Pick Edges. "
                  << std::endl;
        std::cout << facename << std::endl;

        // WARNING: Doesn't give all combinations. Naive method only works with convex polygons
        std::vector<float> hitdistances;
        std::map<float, std::vector<std::string>> distToNames;
        // find which edge is intersected
        for (int i = 0; i < points.size(); i++)
        {
            if (i == points.size() - 1)
            { // if at the last point, the last edge is last point connected to first point
                auto firstpoint = points[i];
                auto secondpoint = points[0];
                tc::Vector3 pos1 { firstpoint->position.x, firstpoint->position.y,
                                   firstpoint->position.z };
                tc::Vector3 pos2 { secondpoint->position.x, secondpoint->position.y,
                                   secondpoint->position.z };
                // TODO: Fix this naive method. Naive method extend the plane out of the line in 6
                // directions
                tc::Vector3 dummy1 = (pos1 + pos2) / 2;
                float offset = 0.170; // TODO: Optimize this to vary by scene
                float testdist1 = std::min(
                    { localRay.HitDistance(pos1, pos2, dummy1 + Vector3 { 0, offset, 0 }),
                      localRay.HitDistance(pos1, pos2, dummy1 + Vector3 { offset, 0, 0 }),
                      localRay.HitDistance(pos1, pos2, dummy1 + Vector3 { 0, 0, offset }),
                      localRay.HitDistance(pos1, pos2, dummy1 + Vector3 { 0, offset, 0 }),
                      localRay.HitDistance(pos1, pos2, dummy1 + Vector3 { offset, 0, 0 }),
                      localRay.HitDistance(pos1, pos2, dummy1 + Vector3 { 0, 0, offset }) });
                hitdistances.push_back(testdist1);
                auto vertname1 = instPrefix + VertToName.at(firstpoint);
                auto vertname2 = instPrefix + VertToName.at(secondpoint);
                std::vector<std::string> names;
                names.push_back(vertname1);
                names.push_back(vertname2);
                distToNames.insert({ testdist1, names });
            }
            else
            {
                auto firstpoint = points[i];
                auto secondpoint = points[i+1];
                tc::Vector3 pos1 { firstpoint->position.x, firstpoint->position.y,
                                   firstpoint->position.z };
                tc::Vector3 pos2 { secondpoint->position.x, secondpoint->position.y,
                                   secondpoint->position.z };


                // TODO: Fix \this naive method. Naive method extend the plane out of the line in 6
                // directions
                tc::Vector3 dummy1 = (pos1 + pos2) / 2;
                float offset = 0.170; // TODO: Optimize this to vary by scene
                float testdist1 = std::min(
                    { localRay.HitDistance(pos1, pos2, dummy1 + Vector3 { 0, offset, 0 }),
                      localRay.HitDistance(pos1, pos2, dummy1 + Vector3 { offset, 0, 0 }),
                      localRay.HitDistance(pos1, pos2, dummy1 + Vector3 { 0, 0, offset }),
                      localRay.HitDistance(pos1, pos2, dummy1 + Vector3 { 0, offset, 0 }),
                      localRay.HitDistance(pos1, pos2, dummy1 + Vector3 { offset, 0, 0 }),
                      localRay.HitDistance(pos1, pos2, dummy1 + Vector3 { 0, 0, offset }) });
                hitdistances.push_back(testdist1);
                auto vertname1 = instPrefix + VertToName.at(firstpoint);
                auto vertname2 = instPrefix + VertToName.at(secondpoint);
                std::vector<std::string> names;
                names.push_back(vertname1);
                names.push_back(vertname2);
                distToNames.insert({ testdist1, names });
            }
        }
        auto mindist = *std::min_element(hitdistances.begin(), hitdistances.end());
        std::cout << "Triangle hit distance for edge:  " + std::to_string(mindist) << std::endl;

        // This is outside of the for loop! incorrect!!
        if (mindist < 100)
        {
            auto hitpointnames = distToNames.at(mindist); // Guaranteed to be two names
            auto name1withoutinstprefix = hitpointnames[0].substr(instPrefix.length());
            auto name2withoutinstprefix = hitpointnames[1].substr(instPrefix.length());

            auto point1 = NameToVert.at(name1withoutinstprefix);
            auto point2 = NameToVert.at(name2withoutinstprefix);

            std::cout << mindist << std::endl;
            result.emplace_back(mindist, hitpointnames);
        }
    }
    // std::sort(result.begin(), result.end());
    for (const auto& sel : result)
    {
        printf("t=%.3f v1=%s v2 =%s\n", sel.first, sel.second[0].c_str(), sel.second[1].c_str());
    }
    return result;
}

std::vector<std::pair<float, std::string>> CMeshInstance::PickVertices(const tc::Ray& localRay)
{
    // Randy look at this for delete face
    std::vector<std::pair<float, std::string>> result;
    auto instPrefix = GetSceneTreeNode()->GetPath() + ".";
    for (const auto& pair : NameToVert)
    {
        tc::Vector3 pos = pair.second->position;
        tc::Vector3 projected = localRay.Project(pos);
        auto dist = (pos - projected).Length();
        auto t = (localRay.Origin - projected).Length();
        if (dist < std::min(0.01f * t, 0.25f))
        {
            //std::cout << pos.x << "  " <<  pos.y << "  " << pos.z << std::endl;
            //std::cout << projected.x << "  " << projected.y << "  " << projected.z << std::endl;
            //std::cout << dist << std::endl;
            //std::cout << t << std::endl;
            //std::cout << instPrefix + pair.first  + " DEBUG PICK VERTICVES " << std::endl;
            result.emplace_back(t, instPrefix + pair.first);
        }
    }
    std::sort(result.begin(), result.end());

    for (const auto& sel : result)
    {
        printf("t=%.3f v=%s\n", sel.first, sel.second.c_str());
    }
    return result;
}

std::vector<Face*>
CMeshInstance::GetSelectedFaceHandles() // Randy added on 10/11 to assist with face coloring after
                                        // selection
{
    return CurrSelectedFaceHandles;
}

std::vector<std::string> CMeshInstance::GetFaceVertexNames(
    std::vector<std::string> facenames) // Randy added on 10/19 to return face vert names
{
    std::vector<std::string> vertnames;
    for (auto facename : facenames)
    {
        if (NameToFace.find(facename) != NameToFace.end())
        {
            auto tempfh = NameToFace[facename];
            auto temp1 = FaceToFaceVerts[tempfh];
            std::vector<std::string> temp2;
            for (auto vert : temp1)
            {
                auto vertname = VertToName[vert];
                auto instPrefix = GetSceneTreeNode()->GetPath() + ".";
                auto instVertName = instPrefix + vertname;
                temp2.push_back(instVertName);
            }
            vertnames.insert(vertnames.end(), temp2.begin(), temp2.end());
        }
        else
        {
            vertnames.clear();
            return vertnames;
        }
    }

    return vertnames;
}

void CMeshInstance::MarkFaceAsSelected(const std::set<std::string>& faceNames, bool bSel)
{
    //auto instPrefix = GetSceneTreeNode()->GetPath() + ".";
    //size_t prefixLen = instPrefix.length();
    //for (const auto& name : faceNames)
    //{
    //    auto iter = NameToFace.find(name.substr(prefixLen));
    //    if (iter == NameToFace.end())
    //        continue;

    //    auto handle = iter->second;
    //    const auto& original = Mesh.color(handle);
    //    printf("Before: %d %d %d\n", original[0], original[1], original[2]);
    //    auto iter0 = std::find(CurrSelectedFaceNamesWithPrefix.begin(),
    //                           CurrSelectedFaceNamesWithPrefix.end(), name);
    //    if (iter0 == CurrSelectedFaceNamesWithPrefix.end())
    //    {

    //        CurrSelectedFaceNames.push_back(name.substr(prefixLen));
    //        CurrSelectedFaceNamesWithPrefix.push_back(name);
    //        CurrSelectedFaceHandles.push_back(handle); // Randy added on 10/11 for face selection
    //    }
    //    else // it has already been selected, then deselect
    //    {
    //        auto iter1 = std::find(CurrSelectedFaceNames.begin(), CurrSelectedFaceNames.end(),
    //                               name.substr(prefixLen));
    //        CurrSelectedFaceNames.erase(iter1); // Randy added this on 10/15
    //        auto iter2 = std::find(CurrSelectedFaceNamesWithPrefix.begin(),
    //                               CurrSelectedFaceNamesWithPrefix.end(), name);
    //        CurrSelectedFaceNamesWithPrefix.erase(iter2);
    //        auto iter3 =
    //            std::find(CurrSelectedFaceHandles.begin(), CurrSelectedFaceHandles.end(), handle);
    //        CurrSelectedFaceHandles.erase(iter3); // Randy added on 10/11 for face selection
    //    }
    //}
    //GetSceneTreeNode()->SetEntityUpdated(true);
}

// TODO: Edge selection. Create Edge Handle data structures later.
void CMeshInstance::MarkEdgeAsSelected(const std::set<std::string>& vertNames, bool bSel)
{
//    auto instPrefix = GetSceneTreeNode()->GetPath() + ".";
//
//    // TODO 11/5 work on this, make sure the currselected stuff is actually working . also, fix
//    // Nome3DView edge deselection
//    std::vector<std::string> forEdge(vertNames.begin(), vertNames.end());
//    auto name1withoutinstprefix = forEdge[0].substr(instPrefix.length());
//    auto name2withoutinstprefix = forEdge[1].substr(instPrefix.length());
//    auto point1 = NameToVert.at(name1withoutinstprefix);
//    auto point2 = NameToVert.at(name2withoutinstprefix);
//    // https://stackoverflow.com/questions/64243444/openmesh-find-edge-connecting-two-vertices
//    CurrSelectedHalfEdgeHandles.push_back(
//        Mesh.find_halfedge(point1, point2)); // not used yet, but can be used for sharpening edges
//    ///////////////////////////////////////////////////////////////////
//
//    size_t prefixLen = instPrefix.length();
//    for (const auto& name : vertNames)
//    {
//        auto iter = NameToVert.find(name.substr(prefixLen));
//        if (iter == NameToVert.end())
//            continue;
//
//        auto handle = iter->second;
//        const auto& original = Mesh.color(handle);
//        printf("Before: %d %d %d\n", original[0], original[1], original[2]);
//        auto iter0 = std::find(CurrSelectedEdgeVertNamesWithPrefix.begin(),
//                               CurrSelectedEdgeVertNamesWithPrefix.end(), name);
//        if (iter0 == CurrSelectedEdgeVertNamesWithPrefix.end())
//        {
//            CurrSelectedEdgeVertNames.push_back(name.substr(prefixLen));
//            CurrSelectedEdgeVertNamesWithPrefix.push_back(name);
//            CurrSelectedEdgeVertHandles.push_back(handle);
//        }
//        else // it has already been selected, then deselect
//        {
//            auto iter1 = std::find(CurrSelectedEdgeVertNames.begin(),
//                                   CurrSelectedEdgeVertNames.end(), name.substr(prefixLen));
//            if (iter1 != CurrSelectedEdgeVertNames.end())
//            { // erase once
//                CurrSelectedEdgeVertNames.erase(iter1);
//            }
//            auto iter2 = std::find(CurrSelectedEdgeVertNamesWithPrefix.begin(),
//                                   CurrSelectedEdgeVertNamesWithPrefix.end(), name);
//            CurrSelectedEdgeVertNamesWithPrefix.erase(iter2);
//            auto iter3 = std::find(CurrSelectedEdgeVertHandles.begin(),
//                                   CurrSelectedEdgeVertHandles.end(), handle);
//            CurrSelectedEdgeVertHandles.erase(iter3);
//        }
//    }
//    GetSceneTreeNode()->SetEntityUpdated(true);
}

//// Vertex selection
void CMeshInstance::MarkVertAsSelected(const std::set<std::string>& vertNames)
{
    std::cout << "switch DS MarkVertAsSelected" << std::endl;
    for (auto name : vertNames)
        std::cout << name << std::endl;

    auto instPrefix = GetSceneTreeNode()->GetPath() + ".";
    size_t prefixLen = instPrefix.length();
    for (const auto& name : vertNames)
    {
        auto iter = NameToVert.find(name.substr(prefixLen));
        if (iter == NameToVert.end())
            continue;
        auto DSvert = iter->second;
        if (std::find(CurrSelectedVertNamesWithPrefix.begin(), CurrSelectedVertNamesWithPrefix.end(), name) == CurrSelectedVertNamesWithPrefix.end())
        { // if hasn't been selected before
            std::cout << "setting vert to selected" + iter->first << std::endl;
            DSvert->selected = true;
            CurrSelectedVertNames.push_back(name.substr(prefixLen));
            CurrSelectedVertNamesWithPrefix.push_back(name);
            CurrSelectedDSVerts.push_back(DSvert);
        }
        else // it has already been selected, then reset to default color
        {
            std::cout << "deselecting vert" + iter->first << std::endl;
            DSvert->selected = false;
            auto iter1 = std::find(CurrSelectedVertNames.begin(), CurrSelectedVertNames.end(),
                                    name.substr(prefixLen));
            if (iter1 != CurrSelectedVertNames.end())
            { // erase once
                CurrSelectedVertNames.erase(iter1);
            }
            auto iter2 = std::find(CurrSelectedVertNamesWithPrefix.begin(),
                                    CurrSelectedVertNamesWithPrefix.end(), name);
            CurrSelectedVertNamesWithPrefix.erase(iter2);
            auto iter3 =
                std::find(CurrSelectedDSVerts.begin(), CurrSelectedDSVerts.end(), DSvert);
            CurrSelectedDSVerts.erase(iter3);
        }
    }
    GetSceneTreeNode()->SetEntityUpdated(true);
}

void CMeshInstance::DeselectAll()
{
//    for (const auto& name : CurrSelectedVertNames)
//    {
//        auto handle = NameToVert[name];
//        Mesh.set_color(handle, { VERT_COLOR });
//        GetSceneTreeNode()->SetEntityUpdated(true);
//    }
//    CurrSelectedVertNames.clear();
//    CurrSelectedVertNamesWithPrefix.clear(); // added 10/3
//
//    // added below on 10/10 for face deselection
//
//    CurrSelectedFaceNames.clear();
//    CurrSelectedFaceNamesWithPrefix.clear();
}


void CVertexSelector::PointUpdate()
{
    // Assume MeshInstance is connected
    auto* mi = MeshInstance.GetValue(nullptr);
    if (!mi)
    {
        printf("Vertex %s does not have a mesh instance\n", TargetName.c_str());
        return;
    }
    auto iter = mi->NameToVert.find(TargetName);

    if (iter == mi->NameToVert.end())
    {
        printf("Vertex %s does not exist in entity %s\n", TargetName.c_str(),
               mi->GetName().c_str());
        return;
    }
    auto vertHandle = iter->second;

    const auto& p = mi->currMesh.findVertexInThisMesh(TargetName); // mi->Mesh.point(vertHandle);
    std::cout << p->position.x << p->position.y << p->position.x << std::endl;
    VI.Position = { p->position.x, p->position.y, p->position.z }; // Project SwitchDS
    VI.Position = mi->GetSceneTreeNode()->L2WTransform.GetValue(Matrix3x4::IDENTITY) * VI.Position;
    Point.UpdateValue(&VI);
}

std::string CVertexSelector::GetPath() const
{
    auto* mi = MeshInstance.GetValue(nullptr);

    if (!mi)
        throw std::runtime_error("Vertex selector cannot find its mesh instance");
    return mi->GetSceneTreeNode()->GetPath() + "." + TargetName;
}

}
