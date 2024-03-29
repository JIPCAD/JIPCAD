#include "Mesh.h"
// Render related
#include "SceneGraph.h"
#include <StringPrintf.h>
#include <StringUtils.h>
#include <QInputDialog>
#include <QObject>

namespace Nome::Scene
{

void DrawLines(IDebugDraw* draw, std::vector<std::vector<Vertex*>> Lines) {
    if (!Lines.empty())
    {
        // Randy added below for loop on 12/27 to draw multiple line strips
        for (auto Line : Lines)
        {
            std::vector<Vector3> positions;
            for (auto vHandle : Line)
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


    for (int i = 0; i < SharpPoints.GetSize(); ++i)
    {
        auto* sharp = SharpPoints.GetValue(i, nullptr);
        if (!sharp->AddSharpnessIntoMesh(this))
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

    // These two calls below are very important for constructing our mesh DS and hence representing our scene
    currMesh.buildBoundary();

    auto className = this->GetMetaObject().ClassName();
    std::set<std::string> polylineClassNames = { "CPolyline", "CBSpline", "CBezierSpline",
                                                 "CSweepPath" };
    bool isPolyline = polylineClassNames.count(className) > 0;
    currMesh.computeNormals(isPolyline); // only compute vert normals for non-polyline classes

    Super::UpdateEntity();
    SetValid(isValid);
}


void CMesh::Draw(IDebugDraw* draw, bool toggle_wireframe)
{
    CEntity::Draw(draw, false);
    DrawLines(draw, LineStrips);
    if (toggle_wireframe)
        DrawLines(draw, WireFrames);
}


Vertex* CMesh::AddVertex(const std::string& name, tc::Vector3 pos, float sharpness)
{
    // Silently fail if the name already exists
    if (HasVertex(name))
        return currMesh.nameToVert.at(name);

    Vertex* currVert = new Vertex(pos.x, pos.y, pos.z, name, currMesh.vertList.size()); // Project SwitchDS // Vertex::Vertex(float x, float y, float z, string assignedName, unsigned long ID)
    currVert->sharpness = sharpness;
    currMesh.addVertex(currVert); // Project SwitchDS

    return currVert;

}

Vector3 CMesh::GetVertexPos(const std::string& name) const
{
    Vertex* vertex = currMesh.nameToVert.find(name)->second;
    return vertex->position;
}

// Called from AddFaceIntoMesh
void CMesh::AddFace(const std::string& name, const std::vector<std::string>& facePointNames,
                    std::string faceSurfaceIdent, std::string faceBackfaceIdent)
{
    std::vector<Vertex*> faceVertices;
    for (const std::string& pointName : facePointNames)
    {
        faceVertices.push_back(currMesh.nameToVert.at(pointName));
    }
    AddFace(name, faceVertices, faceSurfaceIdent, faceBackfaceIdent);
    WireFrames.push_back(faceVertices);
}

void CMesh::AddFace(const std::string& name, const std::vector<Vertex*>& faceDSVerts,
                    std::string faceSurfaceIdent, std::string faceBackfaceIdent)
{
    Face * newFace = currMesh.addFace(faceDSVerts, false); // Project SwitchDS . Check if need to reverseOrder = true or false?
    newFace->surfaceName = faceSurfaceIdent;
    newFace->backfaceName = faceBackfaceIdent;
}

void CMesh::AddLineStrip(const std::string& name,
                         const std::vector<Vertex*>& points)
{
    LineStrips.push_back(points);
}

void CMesh::ClearMesh()
{
    currMesh.clear();
    LineStrips.clear();
    WireFrames.clear();
}

// WARNING this function is not currently used and outdated. leaving here for future use
void CMesh::SetFromData(CMeshImpl mesh, std::map<std::string, Vertex*> vnames,
                        std::map<std::string, Face*> fnames)
{
    Mesh = std::move(mesh);
}

void CMesh::AddEdgeSharpness(Vertex* e1, Vertex* e2, float sharpness)
{
    Edge* edge = currMesh.findEdge(e1, e2, false);
    if (edge) {
        edge->sharpness = sharpness;
    } else {
        std::cout << "can't find edge with vertex " << e1->ID << ", " << e2->ID << std::endl;
    }
}

void CMesh::AddPointSharpness(Vertex* p, float sharpness)
{
    p->sharpness = sharpness;
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
        [this] { MarkOnlyDownstreamDirty(); });
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
    Super::UpdateEntity();
    SetValid(MeshGenerator->IsEntityValid());
}

void CMeshInstance::Draw(IDebugDraw* draw, bool toggle_wireframe) { MeshGenerator->Draw(draw, toggle_wireframe); }

CVertexSelector* CMeshInstance::CreateVertexSelector(const std::string& name,
                                                     const std::string& outputName)
{
    auto* selector = new CVertexSelector(name, outputName);
    SelectorSignal.Connect(selector->MeshInstance);
    return selector;
}

// Conceptually, we are copying from the CMesh object. A mesh instance is basically just a copy plus the scene tree node's transformation matrix
void CMeshInstance::CopyFromGenerator()
{

    auto className = MeshGenerator->GetMetaObject().ClassName();
    std::set<std::string> polylineClassNames = { "CPolyline", "CBSpline", "CBezierSpline", "CSweepPath", "CSweepMorphVisualizer"};
    bool isPolyline = polylineClassNames.count(className) > 0;
    auto newMesh =MeshGenerator->currMesh.newMakeCopy("", isPolyline); // make DSMesh copy for Instance 

    currMesh = newMesh; 

    // Bit weird, but we assign face coloring for mesh instances here. These colors are directly used in DSMeshToQGeometry.cpp.
    CScene* scene = GetSceneTreeNode()->GetOwner()->GetScene();

    for (auto currFace : currMesh.faceList) {
        if (currFace->surfaceName != "") {
            CSurface* surface = dynamic_cast<CSurface*>(scene->FindEntity(currFace->surfaceName).Get());
            currFace->color = {surface->ColorR.GetValue(0.f), surface->ColorG.GetValue(0.f),
                                surface->ColorB.GetValue(0.f)};
        }
        if (currFace->backfaceName != "") {
            CBackface* backface = dynamic_cast<CBackface*>(scene->FindEntity(currFace->backfaceName).Get());
            currFace->backcolor = {backface->ColorR.GetValue(0.f), backface->ColorG.GetValue(0.f),
                               backface->ColorB.GetValue(0.f)};
        }
    }
}

// TODO: Randy removed this function's contents on 1/31 to make changing to winged edge easier. Add back asap
std::vector<std::string> CMeshInstance::RemoveFace(const std::vector<std::string>& faceNames) // Randy added
{
    std::vector<std::string> removedVertName;
    return removedVertName;
}

std::vector<std::pair<float, std::string>> CMeshInstance::PickFaces(const tc::Ray& localRay)
{
    std::vector<std::pair<float, std::string>> result;
    auto instPrefix = GetSceneTreeNode()->GetPath() + "."; // path Name
    std::vector<float> hitDistances;
    for (Face* currFace : currMesh.faceList)
    {
        // WARNING: Doesn't give all combinations. Naive method only works with convex polygons
        for (int i = 0; i < currFace->vertices.size(); i++) {
            for (int j = i + 1; j < currFace->vertices.size(); j++)
            {
                for (int k = j + 1; k < currFace->vertices.size(); k++) {
                    Vertex* firstPoint = currFace->vertices[i];
                    Vertex* secondPoint = currFace->vertices[j];
                    Vertex* thirdPoint = currFace->vertices[k];
                    auto testdist1 = localRay.HitDistance(firstPoint->position, secondPoint->position, thirdPoint->position);
                    hitDistances.push_back(testdist1);
                }
            }
        }
        // Now that we're checking intersection with triangulation of the face, see if it intersected any triangle
        auto minDist = *std::min_element(hitDistances.begin(), hitDistances.end());
        std::cout << "Triangle hit distance:  " + std::to_string(minDist) << std::endl;
        auto instPrefix = GetSceneTreeNode()->GetPath() + ".";
        if (minDist < 100) // minDist of 10 did not work for cases where we zoom away from the scene
        {
            result.emplace_back(minDist, instPrefix + currFace->name);
        }
    }
    // std::sort(result.begin(), result.end());
    for (const auto& sel : result)
    {
        std::cout << "Face added to pick list" << std::endl;
        printf("t=%.3f v=%s\n", sel.first, sel.second.c_str());
    }
    return result;
}


std::vector<std::pair<float, Vector3>> CMeshInstance::GetHitPoint(const tc::Ray& localRay)
{
    std::vector<std::pair<float, Vector3>> result;
    std::unordered_map<float, Vector3> distToHitPoint;
    auto instPrefix = GetSceneTreeNode()->GetPath() + "."; // path Name
    std::vector<float> hitDistances;
    for (Face* currFace : currMesh.faceList)
    {
        // WARNING: Doesn't give all combinations. Naive method only works with convex polygons
        for (int i = 0; i < currFace->vertices.size(); i++)
        {
            for (int j = i + 1; j < currFace->vertices.size(); j++)
            {
                for (int k = j + 1; k < currFace->vertices.size(); k++)
                {
                    Vertex* firstPoint = currFace->vertices[i];
                    Vertex* secondPoint = currFace->vertices[j];
                    Vertex* thirdPoint = currFace->vertices[k];
                    auto testdist1 = localRay.HitDistance(
                        firstPoint->position, secondPoint->position, thirdPoint->position);
                    distToHitPoint[testdist1] = localRay.HitPoint(firstPoint->position, secondPoint->position,
                                                        thirdPoint->position);
                    hitDistances.push_back(testdist1);
                }
            }
        }
        // Now that we're checking intersection with triangulation of the face, see if it
        // intersected any triangle
        auto minDist = *std::min_element(hitDistances.begin(), hitDistances.end());
        //std::cout << "Triangle hit distance:  " + std::to_string(minDist) << std::endl;
        auto instPrefix = GetSceneTreeNode()->GetPath() + ".";
        if (minDist < 100)
        {
            result.emplace_back(minDist, distToHitPoint[minDist]);
        }
    }
    // std::sort(result.begin(), result.end());
 //   for (const auto& sel : result)
 //   {
       //std::cout << "Get HitPoint" << std::endl;
       // printf("t=%.3f\n", sel.first);
 //   }
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
        std::vector<Vertex*> edgeVertsOnly = currMesh.vertList;
        for (int i = 0; i < edgeVertsOnly.size(); i++) // adjacent elements are the edge verts
        {
            std::vector<float> hitdistances;
            std::map<float, std::vector<std::string>> distToNames;
            Vertex* firstPoint;
            Vertex* secondPoint;
            if (i != edgeVertsOnly.size() - 1)
            { 
                firstPoint = edgeVertsOnly[i];
                secondPoint = edgeVertsOnly[i + 1];
            }
            else // else if at the last point, the last edge is last point connected to first point
            {
                firstPoint = edgeVertsOnly[i];
                secondPoint = edgeVertsOnly[0];
            }
                tc::Vector3 pos1 = firstPoint->position;
                tc::Vector3 pos2 = secondPoint->position;
                // Extremely naive method to do ray-edge intersection, extend the plane out of the line in 6 directions a little
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
                auto firstPointNameWithPathInfo = instPrefix + firstPoint->name;
                auto secondPointNameWithPathInfo = instPrefix + secondPoint->name;
                distToNames.insert({ testdist1, { firstPointNameWithPathInfo,
                                                   secondPointNameWithPathInfo }});
            auto mindist = *std::min_element(hitdistances.begin(), hitdistances.end());
                std::cout << "Triangle hit distance for edge:  " + std::to_string(mindist)
                          << std::endl;
            if (mindist < 100)
            {
                //SelectNode will change its color
                GetSceneTreeNode()->GetOwner()->SelectNode();
                MarkDirty();
            }
        }
    }

    for (const auto& sel : result)
    {
        printf("t=%.3f v1=%s v2 =%s\n", sel.first, sel.second[0].c_str(), sel.second[1].c_str());
    }
    return result;
}

// Pick edges return a vector containing hit distance and the pair of edge vertex names
std::vector<std::pair<float, std::vector<std::string>>>
CMeshInstance::PickEdges(const tc::Ray& localRay)
{ 
    // Randy deleted this function's contents on 1/31/2021 to simplify the process of changing DS. TODO: add this back asap
    std::vector<std::pair<float, std::vector<std::string>>> result;
    return result;
}

std::vector<std::pair<float, std::string>> CMeshInstance::PickVertices(const tc::Ray& localRay)
{
    std::vector<std::pair<float, std::string>> result;
    auto instPrefix = GetSceneTreeNode()->GetPath() + ".";
    for (Vertex * currVert : currMesh.vertList)
    {
        tc::Vector3 pos = currVert->position;
        tc::Vector3 projected = localRay.Project(pos);
        auto dist = (pos - projected).Length();
        auto t = (localRay.Origin - projected).Length();
        if (dist < std::min(0.01f * t, 0.25f))
            result.emplace_back(t, instPrefix + currVert->name);
    }
    std::sort(result.begin(), result.end());
    for (const auto& sel : result)
        printf("t=%.3f v=%s\n", sel.first, sel.second.c_str());
    return result;
}

std::vector<Face*>
CMeshInstance::GetSelectedFaceHandles() // Randy added on 10/11 to assist with face coloring after
                                        // selection
{
    return CurrSelectedDSFace;
}


// Randy removed contents on 1/31/2021 to make changing data structures easier. Add back later
std::vector<std::string> CMeshInstance::GetFaceVertexNames(
    std::vector<std::string> faceNames) // Randy added on 10/19 to return face vert names
{
    std::vector<std::string> vertnames;

    return vertnames;
}

void CMeshInstance::MarkFaceAsSelected(const std::set<std::string>& faceNames, bool bSel, float sharpness)
{
    auto instPrefix = GetSceneTreeNode()->GetPath() + ".";
    size_t prefixLen = instPrefix.length();
    for (const auto& name : faceNames)
    {
        auto iter = currMesh.nameToFace.find(name.substr(prefixLen));
        if (iter == currMesh.nameToFace.end())
            continue;


        Face* currFace = iter->second;
        if (!currFace->selected)
        {
            currFace->selected = true;
            if (sharpness >= 0) {

                Edge* firstEdge = currFace->oneEdge;
                Edge* currEdge = firstEdge;
                Edge* nextEdge;
                do
                {

                    if (currFace == currEdge->fa)
                    {
                        nextEdge = currEdge->nextVbFa;
                    }
                    else
                    {
                        if (currEdge->mobius)
                        {
                            nextEdge = currEdge->nextVbFb;
                        }
                        else
                        {
                            nextEdge = currEdge->nextVaFb;
                        }
                    }
                    currEdge->sharpness = sharpness;
                    currEdge = nextEdge;
                } while (currEdge != firstEdge);
            }
        }
        else // if already selected, deselect (face returns to original color)
        {
            currFace->selected = false;   
        }
    }
    GetSceneTreeNode()->SetEntityUpdated(true);
}

// TODO: Edge selection. Create Edge Handle data structures later.
void CMeshInstance::MarkEdgeAsSelected(const std::set<std::string>& vertNames, bool bSel, float sharpness)
{
    // Work in progress
    auto instPrefix = GetSceneTreeNode()->GetPath() + ".";
    size_t prefixLen = instPrefix.length();
    std::vector<Vertex*> edgeVerts; 

    // vertName is guaranteed to be of length 2. the start and end points of an edge
    for (const auto& name : vertNames)
    {
        auto iter = currMesh.nameToVert.find(name.substr(prefixLen));
        if (iter == currMesh.nameToVert.end())
            continue;

        Vertex* currVert = iter->second;
        if (!currVert->selected)
        {
            currVert->selected = true;
            if (sharpness >= 0)
                currVert->sharpness = sharpness;
        }

        else
        {
            currVert->selected = false;
        }
    }

    //if (!currEdge->selected)
    //    currEdge->selected = true;
    //else
    //{
    //    currEdge->selected = false;
    //}
    GetSceneTreeNode()->SetEntityUpdated(true);
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
void CMeshInstance::MarkVertAsSelected(const std::set<std::string>& vertNames, float sharpness)
{
    auto instPrefix = GetSceneTreeNode()->GetPath() + ".";
    size_t prefixLen = instPrefix.length();
    for (const auto& name : vertNames)
    {
        auto iter = currMesh.nameToVert.find(name.substr(prefixLen));
        if (iter == currMesh.nameToVert.end())
            continue;
        auto DSvert = iter->second;
        if (std::find(CurrSelectedVertNamesWithPrefix.begin(), CurrSelectedVertNamesWithPrefix.end(), name) == CurrSelectedVertNamesWithPrefix.end())
        { // if hasn't been selected before
            if (sharpness >= 0)
                DSvert->sharpness = sharpness;
            DSvert->selected = true;
            CurrSelectedVertNames.push_back(name.substr(prefixLen));
            CurrSelectedVertNamesWithPrefix.push_back(name);
            CurrSelectedDSVerts.push_back(DSvert);
        }
        else // it has already been selected, then reset to default color
        {
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
    for (auto vert : currMesh.vertList) {
        vert->selected = false;
        GetSceneTreeNode()->SetEntityUpdated(true);
    }

    for (auto face : currMesh.faceList) {
        face->selected = false;
        GetSceneTreeNode()->SetEntityUpdated(true);
    }
    
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
//    CurrSelectedfaceNames.clear();
//    CurrSelectedfaceNamesWithPrefix.clear();
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
    auto iter = mi->currMesh.nameToVert.find(TargetName);

    if (iter == mi->currMesh.nameToVert.end())
    {

        printf("Vertex %s does not exist in entity %s\n", TargetName.c_str(),
               mi->GetName().c_str());
        return;
    }
    auto vertHandle = iter->second;

    const auto& p = mi->currMesh.findVertexInThisMesh(TargetName); // mi->Mesh.point(vertHandle);
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
