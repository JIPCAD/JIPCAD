#include "MeshMerger.h"
#include "OffsetRefiner.h"
#include "ShellRefiner.h"
#include "Subdivision.h"

#include <cmath>
#include <unordered_map>
#include <algorithm>
#include <vector>
#include <limits>


#ifdef PI
#undef PI
#endif

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

void CMeshMerger::ExportAsStl(QString filename)
{
    ofstream file;
    file.open(filename.toStdString());
    file << "solid\n";
    vector<Face*>::iterator fIt;
    for (fIt = currMesh.faceList.begin(); fIt < currMesh.faceList.end(); fIt++)
    {
        Face* currFace = (*fIt);
        Edge* firstEdge = currFace->oneEdge;
        Edge* currEdge;
        if (firstEdge == NULL)
        {
            cout << "ERROR: This face does not have a sideEdge." << endl;
            exit(0);
        }
        Vertex *v0, *v1, *v2;
        if (currFace == firstEdge->fa)
        {
            v0 = firstEdge->va;
            currEdge = firstEdge->nextVbFa;
        }
        else
        {
            if (firstEdge->mobius)
            {
                v0 = firstEdge->va;
                currEdge = firstEdge->nextVbFb;
            }
            else
            {
                v0 = firstEdge->vb;
                currEdge = firstEdge->nextVaFb;
            }
        }
        tc::Vector3 p0 = v0->position;
        if (currEdge == NULL)
        {
            cout << "ERROR: This face contains only one edge and can not be drawn." << endl;
        }
        do
        {
            Edge* nextEdge;
            if (currFace == currEdge->fa)
            {
                v1 = currEdge->va;
                v2 = currEdge->vb;
                nextEdge = currEdge->nextVbFa;
            }
            else
            {
                if (currEdge->mobius)
                {
                    v1 = currEdge->va;
                    v2 = currEdge->vb;
                    nextEdge = currEdge->nextVbFb;
                }
                else
                {
                    v1 = currEdge->vb;
                    v2 = currEdge->va;
                    nextEdge = currEdge->nextVaFb;
                }
            }
            if (v2 != v0)
            {
                tc::Vector3 faceNormal = getNormal3Vertex(v0->position, v1->position, v2->position);
                file << "  facet normal " << faceNormal.x << " " << faceNormal.y << " "
                     << faceNormal.z << "\n";
                file << "    outer loop\n";
                tc::Vector3 p1 = v1->position;
                tc::Vector3 p2 = v2->position;
                file << "      vertex " << p0.x << " " << p0.y << " " << p0.z << "\n";
                file << "      vertex " << p1.x << " " << p1.y << " " << p1.z << "\n";
                file << "      vertex " << p2.x << " " << p2.y << " " << p2.z << "\n";
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
void CMeshMerger::Shell(std::string f)
{
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
        std::cout << " shell build failed: Please do one of the following:" << std::endl;
    }
}
void CMeshMerger::doShell(DSMesh& _m, Face* f)
{
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
        _m.addFace(newVerts, f->color);
    }

    //_m.buildBoundary(); // Randy added this on 2/26
    //_m.computeNormals();
}
void CMeshMerger::Catmull2(CMeshInstance& meshInstance, bool shouldMergePoints = true)
{
    bool needSubdivision = subdivisionLevel != 0;
    // bool needOffset = (Width.GetValue(0) != 0 || Height.GetValue(0) != 0);
    bool needOffset = offsetIdent;
    // std::cout << std::to_string(Width.GetValue(0)).c_str() << '\n' << std::endl;
    // std::cout << std::to_string(Height.GetValue(0)).c_str() << std::endl;
    if ((!needSubdivision && !needOffset)
        || MergedMesh.vertList.empty()
            && currMesh.isEmpty()) //.vertices_empty()) Randy changed the commented out method
    {
        // nothing to do
        return;
    }
    WireFrames.clear();
    ClearLineStrips();

    // OpenMesh::Subdivider::Uniform::CatmullClarkT<CMeshImpl> catmull; //
    // https://www.graphics.rwth-aachen.de/media/openmesh_static/Documentations/OpenMesh-4.0-Documentation/a00020.html
    // Execute 2 subdivision steps
    DSMesh otherMesh = MergedMesh.newMakeCopy();
    // catmull.attach(otherMesh);
    // prepare(otherMesh);
    bool didOffset = false;
    if (needSubdivision)
    {
        // std::cout << "\nsubdivLevel again: " << subdivisionLevel << "\n";
        subdivide(otherMesh, 1); //, isSharp); // Randy commented this out for now. add back asap
        // 4/30/2025 - Robert made this 1 level at a time, see note at the end of the Catmull
        // function.
        std::cout << "Apply catmullclark subdivision, may take some time..." << std::endl;
        subdivisionLevel--;
    }
    if (needOffset)
    {
        // offset(otherMesh);
        offset(otherMesh, h, w, _outerRimSurface, _innerRimSurface, _outerRimHidden, _innerRimHidden);
        std::cout << "Apply offset, may take some time..." << std::endl;
        didOffset = true;
    }
    currMesh = otherMesh.newMakeCopy();

    if (didOffset)
    {
        // currMeshInstance->GetDSMesh().faceList = currMesh.faceList;
        // currMeshInstance->GetDSMesh().edgeList = currMesh.edgeList;
        // currMeshInstance->GetDSMesh().edgeTable = currMesh.edgeTable;
        // CMeshInstance cmi = CMeshInstance();
        // currMeshInstance->currMesh = otherMesh;
        // MergedMesh = currMesh.newMakeCopy();
        // MergeCurr();
        // MergeIn(*currMeshInstance, true);
    }
    MergedMesh = currMesh.newMakeCopy();

    // MergeCurr();
    // std::cout << "";
    // MergedMesh = otherMesh.newMakeCopy();

    // subdivide(currMesh, subdivisionLevel);
    //  ccSubdivision(3);
    try
    {
        // currMesh.buildBoundary();
        // currMesh.computeNormals();
        //  MergedMesh = currMesh.newMakeCopy();

        // MergeIn(currMesh.newMakeCopy(), false);
    }
    catch (std::exception& e)
    {
        std::cout << "catmul clark subdivision failed: Please do one of the following:"
                  << std::endl;
    }

    // MergeCurr();

    // Added by Robert - 4/30/2025
    // This in combination with just doing 1 subdivision level at a time
    // allows for the coloring to be consistent throughout the mesh.
    // When applying multiple levels at once, you will have coloring clipping between faces
    // and other undefined coloring behavior.
    if (subdivisionLevel > 0)
        Catmull2(meshInstance, shouldMergePoints);
    // MergeIn(meshInstance, shouldMergePoints);
}

void CMeshMerger::Catmull()
{
    bool needSubdivision = subdivisionLevel != 0;
    // bool needOffset = (Width.GetValue(0) != 0 || Height.GetValue(0) != 0);
    bool needOffset = offsetIdent;
    // std::cout << std::to_string(Width.GetValue(0)).c_str() << '\n' << std::endl;
    // std::cout << std::to_string(Height.GetValue(0)).c_str() << std::endl;
    if ((!needSubdivision && !needOffset)
        || MergedMesh.vertList.empty()
            && currMesh.isEmpty()) //.vertices_empty()) Randy changed the commented out method
    {
        // nothing to do
        return;
    }
    WireFrames.clear();
    ClearLineStrips();
    LineStrips.clear();
    DSFaceWithColor.clear();
    // OpenMesh::Subdivider::Uniform::CatmullClarkT<CMeshImpl> catmull; //
    // https://www.graphics.rwth-aachen.de/media/openmesh_static/Documentations/OpenMesh-4.0-Documentation/a00020.html
    // Execute 2 subdivision steps
    DSMesh otherMesh = MergedMesh.newMakeCopy();
    MergedMesh.clearAndDelete();
    // catmull.attach(otherMesh);
    // prepare(otherMesh);
    bool didOffset = false;
    if (needSubdivision)
    {
        // std::cout << "\nsubdivLevel again: " << subdivisionLevel << "\n";
        subdivide(otherMesh, 1); //, isSharp); // Randy commented this out for now. add back asap
        // 4/30/2025 - Robert made this 1 level at a time, see note at the end of the Catmull
        // function.
        std::cout << "Apply catmullclark subdivision, may take some time..." << std::endl;
        subdivisionLevel--;
    }
    if (needOffset)
    {
        // offset(otherMesh);
        offset(otherMesh, h, w, _outerRimSurface, _innerRimSurface, _outerRimHidden, _innerRimHidden);
        std::cout << "Apply offset, may take some time..." << std::endl;
        didOffset = true;
    }
    currMesh = otherMesh.newMakeCopy();

    if (didOffset)
    {
        // currMeshInstance->GetDSMesh().faceList = currMesh.faceList;
        // currMeshInstance->GetDSMesh().edgeList = currMesh.edgeList;
        // currMeshInstance->GetDSMesh().edgeTable = currMesh.edgeTable;
        // CMeshInstance cmi = CMeshInstance();
        // currMeshInstance->currMesh = otherMesh;
        // MergedMesh = currMesh.newMakeCopy();
        // MergeCurr();
        // MergeIn(*currMeshInstance, true);
    }

    // MergeCurr();
    // std::cout << "";
    // MergedMesh = otherMesh.newMakeCopy();

    // subdivide(currMesh, subdivisionLevel);
    //  ccSubdivision(3);
    try
    {
        MergedMesh = otherMesh.newMakeCopy();
        // if (!didOffset)
        MergedMesh.computeNormals();
        MergedMesh.buildBoundary();

        currMesh = MergedMesh.newMakeCopy();
        std::cout << "DSMesh: v=" << currMesh.n_vertices() << " f=" << currMesh.n_faces() << "\n"
                  << "OpenMesh: v=" << Mesh.n_vertices() << " f=" << Mesh.n_faces() << "\n";
        std::cout << "\n Curr Mesh Properties (the one that's rendered): ";
        std::cout << "vertList=" << currMesh.vertList.size()
                  << " faceList=" << currMesh.faceList.size()
                  << " edgeList=" << currMesh.edgeList.size()
                  << " nameToVert=" << currMesh.nameToVert.size() // if exists
                  << " nameToFace=" << currMesh.nameToFace.size() // if exists
                  << "\n";
        // MergedMesh = currMesh.newMakeCopy();

        // MergeIn(currMesh.newMakeCopy(), false);
    }
    catch (std::exception& e)
    {
        std::cout << "catmul clark subdivision failed: Please do one of the following:"
                  << std::endl;
    }

    // MergeCurr();

    // Added by Robert - 4/30/2025
    // This in combination with just doing 1 subdivision level at a time
    // allows for the coloring to be consistent throughout the mesh.
    // When applying multiple levels at once, you will have coloring clipping between faces
    // and other undefined coloring behavior.
    if (subdivisionLevel > 0)
        Catmull();
    // MergeIn(meshInstance, shouldMergePoints);
}

// Both of the below functions are used in the ASTSceneAdapter for creating
// the normal vectors when using tags facenormal and vertexnormal
// when instantiating a mesh

// Creates the normal vectors of the currMesh - Robert added 4/8/2025
void CMeshMerger::CreateNormalsCurr(bool faceNormals, float faceNormalMultiplier,
                                    bool vertexNormals, float vertexNormalMultiplier)
{
    CreateNormals(currMesh.newMakeCopy(), faceNormals, faceNormalMultiplier, vertexNormals,
                  vertexNormalMultiplier);
}

void CMeshMerger::changeColors(std::string surfaceName, std::string backfaceName)
{
    // std::cout << "\nran color change\n";
    std::vector<Face*> myFaceList = MergedMesh.faceList;
    if (!surfaceName.empty())
    {
        for (Face* f : myFaceList)
        {
            if (f->surfaceName.empty())
                f->surfaceName = surfaceName;
        }
    }
    if (!backfaceName.empty())
    {
        for (Face* f : myFaceList)
        {
            if (f->backfaceName.empty())
                f->backfaceName = backfaceName;
        }
    }
}

// Robert added in March 2025
// Robert modified in March 2026 to use Newell's for face normals and Angle-Weighting for vertex
// normals
void CMeshMerger::CreateNormals(DSMesh& ds, bool faceNormals, float faceNormalMultiplier,
                                bool vertexNormals, float vertexNormalMultiplier)
{
    std::vector<std::vector<Vertex*>> tmp = WireFrames;
    ClearLineStrips();
    for (auto i : tmp)
    {
        WireFrames.push_back(i);
    } // This preserves the WireFrame as the original is deleted from ClearLineStrips, needed to
      // remove previous normals
    auto& otherMesh = ds;
    // otherMesh.computeNormals();
    std::vector<Vertex*> currNormal = {};
    if (faceNormals)
    {
        std::vector<Face*> faceList = otherMesh.faceList;
        int i = 0;
        for (auto* f : faceList)
        {
            Vertex* center = new Vertex();
            Vertex* distant = new Vertex();
            center->position = otherMesh.centerPoint(f).position; // Gets center point of face
            distant->position = f->normal; // Get the point the normal points to
            distant->position.Normalize();
            distant->position = distant->position * faceNormalMultiplier;
            distant->position += center->position;
            currNormal.push_back(center);
            currNormal.push_back(distant);
            AddLineStrip("face_normal_" + std::to_string(i), currNormal);
            currNormal.clear();
            i++;
        }
    }
    if (vertexNormals)
    {
        /*
        std::map<Vertex*, Vector3> vertNormalMappings;
        std::vector<Face*> faceList = otherMesh.faceList;
        int i = 0;
        for (auto* f : faceList)
        {
            std::vector<Vertex*> vertList = f->vertices;
            Vertex* vCurr = f->vertices[i];
            Vertex* vPrev = f->vertices[(i - 1 + f->vertices.size()) % f->vertices.size()];
            Vertex* vNext = f->vertices[(i + 1) % f->vertices.size()];
            Vector3 e1 = (vPrev->position - vCurr->position);
            e1.Normalize();
            Vector3 e2 = (vNext->position - vCurr->position);
            e2.Normalize();
            double angle = acos(std::clamp(std::double_t(e1.DotProduct(e2)), -1.0, 1.0));
            vCurr->normal += (f->normal * angle);
        }
        for (const auto& pair : vertNormalMappings)
        {
            std::vector<Vertex*> v = {};
            v.push_back(pair.first);
            Vertex* distantVert = new Vertex();
            distantVert->SetPosition(pair.second.x, pair.second.y, pair.second.z);
            distantVert->position -= pair.first->position;
            distantVert->position.Normalize();
            distantVert->position = distantVert->position * vertexNormalMultiplier;
            distantVert->position += pair.first->position;
            v.push_back(distantVert);
            AddLineStrip("vert_normal_" + i, v);
            i++;
        }*/
        int i = 0;
        for (auto* v : otherMesh.vertList)
        {
            Vertex* center = new Vertex();
            Vertex* distant = new Vertex();
            center->position = v->position; // Gets center point of face
            distant->position = v->normal; // Get the point the normal points to
            distant->position.Normalize();
            distant->position = distant->position * vertexNormalMultiplier;
            std::cout << "Added normal (" << distant->position.x << ", " << distant->position.y
                      << ", " << distant->position.z << ")\n";
            distant->position += center->position;
            currNormal.push_back(center);
            currNormal.push_back(distant);
            // std::vector<Vertex*> temp = { v->position, v->normal };
            AddLineStrip("vert_normal_" + std::to_string(i), currNormal);
            currNormal.clear();
            i++;
        }
    }
}
tc::Matrix3x4 CMeshMerger::getMergedMeshTf() { return MergedMeshTf; }

void CMeshMerger::MergeCurr()
{
    DSMesh otherMesh = currMesh.newMakeCopy();

    MergedMesh.clear();

    bool shouldMergePoints = true;

    std::unordered_map<Vertex*, Vertex*> vertMap;

    // 1. Copy vertices from currMesh into MergedMesh.
    for (auto* otherVert : otherMesh.vertList)
    {
        if (!otherVert)
        {
            continue;
        }

        Vector3 localPos = otherVert->position;

        Vertex* closestVert = nullptr;
        float distance = std::numeric_limits<float>::max();

        if (!MergedMesh.vertList.empty())
        {
            auto closestResult = FindClosestVertex(localPos);
            closestVert = closestResult.first;
            distance = closestResult.second;
        }

        if (distance < Epsilon && shouldMergePoints && otherVert != nullptr
            && closestVert != nullptr)
        { // this is to check for cases where there is an overlap (two vertices lie in the exact
            // same world space coordinate). We only want to create one merger vertex at this
            // location!
            vertMap[otherVert] =
                closestVert; // just set vi to the closestVert (which is a merger vertex
            // in the same location added in a previous iteration)
            closestVert->sharpness = std::max(closestVert->sharpness, otherVert->sharpness);

            if (otherVert->sharpness > 0.0f)
            {
                std::cout << "[mergeCurr] merged vertex sharpness "
                          << otherVert->sharpness
                          << " into "
                          << closestVert->name
                          << std::endl;
            }
        }
        else
        {
            Vertex* copiedVert = new Vertex(
                localPos.x,
                localPos.y,
                localPos.z,
                MergedMesh.nameToVert.size()
            );

            copiedVert->name =
                "copiedVert" + std::to_string(MergedMesh.nameToVert.size());

            copiedVert->sharpness = otherVert->sharpness;
            copiedVert->normal = otherVert->normal;
            copiedVert->source_vertex = otherVert;

            MergedMesh.addVertex(copiedVert);

            vertMap[otherVert] = copiedVert;

            ++VertCount;
        }
    }

    // 2. Copy faces. This is what creates the actual merged edges.
    for (auto* otherFace : otherMesh.faceList)
    {
        if (!otherFace)
        {
            continue;
        }

        std::vector<Vertex*> verts;

        for (auto* vert : otherFace->vertices)
        {
            if (!vert)
            {
                continue;
            }

            auto it = vertMap.find(vert);

            if (it != vertMap.end())
            {
                verts.emplace_back(it->second);
            }
        }

        if (verts.size() < 3)
        {
            std::cout << "[mergeCurr] skipped face with fewer than 3 verts" << std::endl;
            continue;
        }

        Face* newFace = MergedMesh.addFace(
            verts,
            otherFace->color,
            otherFace->surfaceName,
            otherFace->backfaceName
        );

        if (newFace)
        {
            newFace->user_defined_color = otherFace->user_defined_color;
            newFace->color = otherFace->color;
            newFace->backcolor = otherFace->backcolor;
            newFace->surfaceName = otherFace->surfaceName;
            newFace->backfaceName = otherFace->backfaceName;
        }

        ++FaceCount;
    }

    // 3. Transfer edge sharpness.
    // Do not manually create Edge objects here.
    // The real edges were already created by MergedMesh.addFace(...).
    for (auto* edge : otherMesh.edges())
    {
        if (!edge || !edge->v0() || !edge->v1())
        {
            continue;
        }

        auto it0 = vertMap.find(edge->v0());
        auto it1 = vertMap.find(edge->v1());

        if (it0 == vertMap.end() || it1 == vertMap.end())
        {
            std::cout << "[mergeCurr] missing copied vertex for source edge "
                      << edge->v0()->name << " - "
                      << edge->v1()->name << std::endl;
            continue;
        }

        Vertex* mergedV0 = it0->second;
        Vertex* mergedV1 = it1->second;

        WireFrames.push_back({ mergedV0, mergedV1 });

        Edge* mergedEdge = MergedMesh.findEdge(mergedV0, mergedV1, false);

        if (!mergedEdge)
        {
            std::cout << "[mergeCurr] could not find merged edge for "
                      << edge->v0()->name << " - "
                      << edge->v1()->name << std::endl;
            continue;
        }

        if (edge->sharpness > 0.0f)
        {
            mergedEdge->sharpness = std::max(mergedEdge->sharpness, edge->sharpness);
            mergedEdge->isSharp = true;

            mergedV0->sharpness = std::max(mergedV0->sharpness, edge->sharpness);
            mergedV1->sharpness = std::max(mergedV1->sharpness, edge->sharpness);

            std::cout << "[mergeCurr] transferred sharpness "
                      << mergedEdge->sharpness
                      << " to edge "
                      << mergedEdge->v0()->name << " - "
                      << mergedEdge->v1()->name
                      << std::endl;
        }
    }

    // 4. Final debug count.
    int sharpCount = 0;

    for (auto* edge : MergedMesh.edgeList)
    {
        if (edge && edge->sharpness > 0.0f)
        {
            ++sharpCount;
        }
    }

    std::cout << "[mergeCurr] merged sharp edge count = "
              << sharpCount << std::endl;

    MergedMesh.buildBoundary();
    MergedMesh.computeNormals();

    currMesh = MergedMesh.newMakeCopy();
}
void CMeshMerger::MergeIn(CMeshInstance& meshInstance, bool shouldMergePoints)
{
    // currMeshInstance = (std::make_shared<CMeshInstance>(meshInstance));
    treeNode = meshInstance.GetSceneTreeNode();
    auto tf = meshInstance.GetSceneTreeNode()->L2WTransform.GetValue(
        tc::Matrix3x4::IDENTITY); // The transformation matrix is the identity matrix by default
    MergedMeshTf = tf;
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
    // TODO: Fix dependent vertices ie .iHex.v0
    // TODO: Dependency tree fix.

    // Copy over all the vertices and check for overlapping
    std::unordered_map<Vertex*, Vertex*> vertMap;
    for (auto otherVert :
         otherMesh.vertList) // Iterate through all the vertices in the mesh (the non-merger mesh,
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
            closestVert->sharpness = std::max(closestVert->sharpness, otherVert->sharpness);
            printf("set sharpness: %f\n", closestVert->sharpness);
        }
        else // Else, we haven't added a vertex at this location yet. So lets add_vertex to the
             // merger mesh.
        {
            Vertex* copiedVert = new Vertex(worldPos.x, worldPos.y, worldPos.z,
                                            MergedMesh.nameToVert.size()); // project add offset
            copiedVert->name =
                "copiedVert"
                + std::to_string(
                    MergedMesh.nameToVert.size()); // Randy this was causing the bug!!!!!!! the name
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
    for (auto otherFace :
         otherMesh.faceList) // Iterate through all the faces in the mesh (that is, the non-merger
                             // mesh, aka the one you're trying to copy faces from)
    {
        std::vector<Vertex*> verts;
        for (auto vert : otherFace->vertices) // otherMesh vertices
        { // iterate through all the vertices on this face
            verts.emplace_back(vertMap[vert]);
        } // Add the vertex handles
        // MergedMesh.addFace(verts, otherFace->color, otherFace->surfaceName); // Project AddOffset
        MergedMesh.addFace(verts, otherFace->surfaceName, otherFace->backfaceName);
        std::cout << "facenames:" << otherFace->surfaceName << "\n";
        std::string fName = "v" + std::to_string(FaceCount);
        FaceCount++;
    }


    for (auto* edge : otherMesh.edges())
{
    if (!edge || !edge->v0() || !edge->v1())
    {
        continue;
    }

    auto it0 = vertMap.find(edge->v0());
    auto it1 = vertMap.find(edge->v1());

    if (it0 == vertMap.end() || it1 == vertMap.end())
    {
        std::cout << "[merge] missing copied vertex for source edge "
                  << edge->v0()->name << " - "
                  << edge->v1()->name << std::endl;
        continue;
    }

    Vertex* mergedV0 = it0->second;
    Vertex* mergedV1 = it1->second;

    std::vector<Vertex*> mergedEdgeVertices;
    mergedEdgeVertices.push_back(mergedV0);
    mergedEdgeVertices.push_back(mergedV1);
    WireFrames.push_back(mergedEdgeVertices);

    // Important: do NOT create a new Edge manually here.
    // MergedMesh.addFace(...) already created the real edge and inserted it
    // into edgeList / edgeTable. We need to find and update that edge.
    Edge* mergedEdge = MergedMesh.findEdge(mergedV0, mergedV1, false);

    if (!mergedEdge)
    {
        std::cout << "[merge] could not find merged edge for "
                  << edge->v0()->name << " - "
                  << edge->v1()->name << std::endl;
        continue;
    }

    if (edge->sharpness > 0.0f)
    {
        mergedEdge->sharpness = std::max(mergedEdge->sharpness, edge->sharpness);
        mergedEdge->isSharp = true;

        mergedV0->sharpness = std::max(mergedV0->sharpness, edge->sharpness);
        mergedV1->sharpness = std::max(mergedV1->sharpness, edge->sharpness);

        std::cout << "[merge] transferred sharpness "
                  << mergedEdge->sharpness << " to edge "
                  << mergedEdge->v0()->name << " - "
                  << mergedEdge->v1()->name << std::endl;
    }
}
    //otherMesh.visible = false;
    MergedMesh.buildBoundary();
    MergedMesh.computeNormals();
    currMesh = MergedMesh.newMakeCopy();
}
DSMesh CMeshMerger::getCurrMesh() { return currMesh.newMakeCopy(); }
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

#include <algorithm>
#include <map>
#include <set>
#include <string>
#include <vector>

// normals first, then split vertices, then done
bool CMeshMerger::offset(DSMesh& _m, double height, double width, std::string outerRimSurface,
                         std::string innerRimSurface, bool outerRimHidden, bool innerRimHidden)
{
    if (height < 0 && width < 0)
        return true;
    auto shouldOffsetFace = [](const Face* f)
    {
        if (!f)
            return false;
        return true;
        const std::string& n = f->name;

        // These are visible/generated result faces, but they should not be
        // source faces for the next offset pass.
        if (n.find("_offsetInnerFace") != std::string::npos)
            return false;

        if (n.find("_offsetHoleRibbon") != std::string::npos)
            return false;

        // Offset:
        // - original faces
        // - _offsetOuterFace
        // - _offsetBoundaryRibbon
        return true;
    };
    width = 1 - width;
    if (width >= 1)
        width = 0;
    DSMesh& out = DSMesh();
    DSMesh _m_original = _m.newMakeCopy();
    _m_original.computeNormals();

    std::map<Vertex*, int> normalCount;

    for (auto v : _m_original.vertList)
    {
        v->normal = tc::Vector3(0, 0, 0);
    }

    for (auto f : _m_original.faceList)
    {
        if (!shouldOffsetFace(f))
            continue;

        for (auto v : f->vertices)
        {
            v->normal = v->normal + f->normal;
            normalCount[v]++;
        }
    }

    for (auto v : _m_original.vertList)
    {
        if (normalCount[v] == 0)
            continue;

        double len = std::sqrt(v->normal.x * v->normal.x + v->normal.y * v->normal.y
                               + v->normal.z * v->normal.z);

        if (len > 1e-12)
        {
            v->normal = tc::Vector3(v->normal.x / len, v->normal.y / len, v->normal.z / len);
        }
    }

    _m.clear();
    _m.clearAndDelete();
    _m.faceList.clear();
    _m.vertList.clear();
    _m.updateVertListAfterDeletion();
    _m.edgeList.clear();
    _m.boundaryEdgeList().clear();
    std::map<Vertex*, Vertex*> outerVerts;
    std::map<Vertex*, Vertex*> innerVerts;
    std::map<Vertex*, std::vector<Vertex*>> outerVertsHole;
    std::map<Vertex*, std::vector<Vertex*>> innerVertsHole;
    std::map<Vertex*, std::vector<Face*>> vertToFaces;

    Vertex* outerVert;
    Vertex* innerVert;
    for (auto f : _m_original.faceList)
    {
        if (!shouldOffsetFace(f))
            continue;

        for (auto v : f->vertices)
        {
            vertToFaces[v].push_back(f);
        }
    }
    double d = height / 2.0;

    for (auto v : _m_original.vertList)
    {
        // Get all faces attached to this vertex
        std::vector<Face*> adjFaces = vertToFaces[v];

        // --- MODIFIED VERTEX CALCULATION ---
        double miterLen = d;

        if (!adjFaces.empty())
        {
            double minDot = 1.0;

            // Find the sharpest angle between the vertex normal and adjacent faces
            for (auto f : adjFaces)
            {
                double dotProduct = v->normal.DotProduct(f->normal);
                if (dotProduct < minDot)
                {
                    minDot = dotProduct;
                }
            }

            // Clamp minDot to 0.2 to prevent the mesh from exploding on extremely sharp spikes
            // (caps the max movement at 5x the thickness)
            minDot = std::max(0.2, minDot);

            // Scale the offset distance
            miterLen = d / minDot;
        }

        // Calculate positions STRICTLY along the original normal axis
        tc::Vector3 outerPos(v->position.x + (miterLen * v->normal.x),
                             v->position.y + (miterLen * v->normal.y),
                             v->position.z + (miterLen * v->normal.z));

        tc::Vector3 innerPos(v->position.x - (miterLen * v->normal.x),
                             v->position.y - (miterLen * v->normal.y),
                             v->position.z - (miterLen * v->normal.z));

        // --- 3. CREATE VERTICES ---
        // (This is now safely INSIDE the 'v' loop)
        outerVert = new Vertex(outerPos.x, outerPos.y, outerPos.z, out.vertList.size());
        outerVert->name = v->name + "_offsetOuter"; // Safest naming convention for multi-file
        outerVerts[v] = outerVert;
        outerVert->normal = v->normal;
        out.addVertex(outerVert);

        innerVert = new Vertex(innerPos.x, innerPos.y, innerPos.z, out.vertList.size());
        innerVert->name = v->name + "_offsetInner"; // Safest naming convention for multi-file
        innerVerts[v] = innerVert;
        innerVert->normal = v->normal;
        out.addVertex(innerVert);

        WireFrames.push_back({ outerVert, innerVert });
    } // End of the 'v' loop
    std::vector<Vertex*> faceVertsInner = {};
    std::vector<Vertex*> faceVertsOuter = {};

    Face* addedFace;
    struct TempFace
    {
        std::vector<Vertex*> vertices;
        std::string surfaceName;
        std::string backfaceName;
        std::string name;
    };
    std::vector<TempFace> outerFaces;
    std::vector<TempFace> innerFaces;
    std::vector<Face*> tmpFaceList;
    auto isGeneratedFace = [](const Face* f)
    {
        if (!f)
            return true;
        // return false;
        // return f->name.find("_offsetRibbon") != std::string::npos;

        return f->name.find("_offsetInnerFace") != std::string::npos
            || f->name.find("_offsetHoleRibbon") != std::string::npos;
        //|| f->name.find("_offsetBoundaryRibbon") != std::string::npos;

        /*
            || f->name.find("_offsetOuterFace") != std::string::npos

            || f->name.find("_hole") != std::string::npos
            || f->name.find("_offsetRibbonHoleWall") != std::string::npos;
        */
        return false;
    };

    for (auto f : _m_original.faceList)
    {
        faceVertsInner.clear();
        faceVertsOuter.clear();
        if (!shouldOffsetFace(f))

        {
            continue;
        }

        for (auto v : f->vertices)
        {
            faceVertsOuter.push_back(outerVerts[v]);
            faceVertsInner.push_back(innerVerts[v]);
        }

        // 2. Safely store the data WITHOUT touching the Face class
        innerFaces.push_back({ faceVertsInner, f->surfaceName, f->backfaceName, f->name });
        outerFaces.push_back({ faceVertsOuter, f->surfaceName, f->backfaceName, f->name });
        tmpFaceList.push_back(f);

        if (width == 0)
        {
            out.addFace(faceVertsOuter, f->surfaceName, f->backfaceName);

            // Reverse ONLY when adding the solid face to the mesh
            std::vector<Vertex*> reversedInner = faceVertsInner;
            std::reverse(reversedInner.begin(), reversedInner.end());
            out.addFace(reversedInner, f->surfaceName, f->backfaceName);
        }
    }

    std::map<std::pair<Vertex*, Vertex*>, std::string> directedEdges;

    for (auto f : _m_original.faceList)
    {
        if (!shouldOffsetFace(f))
            continue;

        for (int i = 0; i < f->vertices.size(); ++i)
        {
            Vertex* v1 = f->vertices[i];
            Vertex* v2 = f->vertices[(i + 1) % f->vertices.size()];

            directedEdges[{ v1, v2 }] = f->surfaceName;
        }
    }
    // out.computeNormals();
    //  For each boundary edge, create a ribbon quad
    std::vector<Vertex*> ribbonVerts;
    bool addRibbons = !(std::abs(height) < 1e-8); // true;
    if (addRibbons)
    {
        for (const auto& edge : directedEdges)
        {
            Vertex* v1_src = edge.first.first;
            Vertex* v2_src = edge.first.second;

            if (directedEdges.count({ v2_src, v1_src }) > 0)
            {
                continue;
            }

            Vertex* v1_outer = outerVerts[v1_src];
            Vertex* v2_outer = outerVerts[v2_src];
            Vertex* v1_inner = innerVerts[v1_src];
            Vertex* v2_inner = innerVerts[v2_src];
            std::string rimSurface = edge.second;
            // Create ribbon quad with correct winding to stitch the shells
            // The edge on the outer shell goes v1_outer -> v2_outer.
            // To be a valid neighbor, the ribbon must traverse it backwards: v2_outer -> v1_outer.
            ribbonVerts.clear();
            ribbonVerts.push_back(v2_outer);
            ribbonVerts.push_back(v1_outer);
            ribbonVerts.push_back(v1_inner);
            ribbonVerts.push_back(v2_inner);

            if (true)
            {
                std::string outerHoleSurf = outerRimSurface;
                if (outerHoleSurf.empty())
                    outerHoleSurf = rimSurface;
                Face* t_face = out.addFace(ribbonVerts, outerHoleSurf, "");
                if (outerRimHidden != true)
                    out.faceList.back()->hide = false; // for outer boundary ribbon
                else
                    out.faceList.back()->hide = true;
                out.faceList.back()->name =
                    out.faceList.back()->name + "_offsetBoundaryRibbon"; //"_offsetRibbon"; //
                ribbonVerts.push_back(v2_outer);

                WireFrames.push_back(ribbonVerts);
                ribbonVerts.pop_back();
            }
        }
    }
    auto getMappedCentroid = [](TempFace f)
    {
        tc::Vector3 centroid(0, 0, 0);
        int numVerts = f.vertices.size();

        if (numVerts == 0)
            return centroid; // Safety check

        for (auto v : f.vertices)
        {
            // Use the mapped offset vertex position instead of the original
            centroid = centroid + v->position;
        }

        return tc::Vector3(centroid.x / numVerts, centroid.y / numVerts, centroid.z / numVerts);
    };

    double scale = width; // Hole size (0.5 means the hole is 50% the size of the face)
    if (width > 0)
    {
        for (size_t i = 0; i < outerFaces.size(); ++i)
        {
            TempFace f_out = outerFaces[i];
            TempFace f_in = innerFaces[i];
            Face* f_curr = tmpFaceList[i];
            // Get the centroids using your built-in function
            tc::Vector3 c_out = getMappedCentroid(f_out);
            tc::Vector3 c_in = getMappedCentroid(f_in);

            int numVerts = f_out.vertices.size();

            tc::Vector3 c_orig(0, 0, 0);
            for (auto v : f_curr->vertices)
            {
                c_orig = c_orig + v->position;
            }
            c_orig = tc::Vector3(c_orig.x / numVerts, c_orig.y / numVerts, c_orig.z / numVerts);

            std::vector<Vertex*> outerHoleVerts;
            std::vector<Vertex*> innerHoleVerts;
            // Scale toward the centroids
            auto getCentroid = [](const std::vector<Vertex*>& verts)
            {
                tc::Vector3 centroid(0, 0, 0);
                if (verts.empty())
                    return centroid;

                for (auto v : verts)
                {
                    centroid = centroid + v->position;
                }

                double invNum = 1.0 / static_cast<double>(verts.size());
                return tc::Vector3(centroid.x * invNum, centroid.y * invNum, centroid.z * invNum);
            };
            auto baseName = [](const std::string& name)
            {
                size_t pos = name.rfind("_offset");
                if (pos == std::string::npos)
                    return name;
                return name.substr(0, pos);
            };

            for (int j = 0; j < numVerts; ++j)
            {
                int prev = (j + numVerts - 1) % numVerts;
                int next = (j + 1) % numVerts;

                Vertex* O_curr = f_out.vertices[j];
                Vertex* I_curr = f_in.vertices[j];
                Vertex* V_orig = f_curr->vertices[j];

                tc::Vector3 H_orig_pos = c_orig + (V_orig->position - c_orig) * width;
                /*
                tc::Vector3 H_out_pos = c_out + (O_curr->position - c_out) * width;
                tc::Vector3 H_in_pos = c_in + (I_curr->position - c_in) * width;
                */
                tc::Vector3 H_out_pos(H_orig_pos.x + (d * f_curr->normal.x),
                                      H_orig_pos.y + (d * f_curr->normal.y),
                                      H_orig_pos.z + (d * f_curr->normal.z));

                tc::Vector3 H_in_pos(H_orig_pos.x - (d * f_curr->normal.x),
                                     H_orig_pos.y - (d * f_curr->normal.y),
                                     H_orig_pos.z - (d * f_curr->normal.z));
                Vertex* h_out =
                    new Vertex(H_out_pos.x, H_out_pos.y, H_out_pos.z, out.vertList.size());

                h_out->name =
                    f_out.name + "_holeOut_" + std::to_string(i) + "_" + std::to_string(j);

                h_out->normal = O_curr->normal;

                out.addVertex(h_out);
                outerHoleVerts.push_back(h_out);
                outerVertsHole[O_curr].push_back(h_out);

                Vertex* h_in = new Vertex(H_in_pos.x, H_in_pos.y, H_in_pos.z, out.vertList.size());

                h_in->name = f_in.name + "_holeIn_" + std::to_string(i) + "_" + std::to_string(j);
                h_in->normal = I_curr->normal;

                out.addVertex(h_in);
                innerHoleVerts.push_back(h_in);
                innerVertsHole[I_curr].push_back(h_in);
            }
            innerHoleVerts.push_back(innerHoleVerts.front());
            outerHoleVerts.push_back(outerHoleVerts.front());

            WireFrames.push_back(innerHoleVerts);
            WireFrames.push_back(outerHoleVerts);
            innerHoleVerts.pop_back();
            outerHoleVerts.pop_back();
            std::vector<Vertex*> outVerts = f_out.vertices;
            std::vector<Vertex*> inVerts = f_in.vertices;
            std::string surfOut = f_out.surfaceName;
            std::string backOut = ""; // f_out->backfaceName;
            std::string surfIn = f_in.surfaceName;
            std::string backIn = ""; // f_in->backfaceName;
            bool flatOffset = std::abs(height) < 1e-8;

            // Build the new geometry (The rings of trapezoids + the tube walls)
            for (int j = 0; j < numVerts; ++j)
            {
                int next = (j + 1) % numVerts;
                bool isRibbon = f_curr && f_curr->name.rfind("_offsetRibbon") != std::string::npos;
                bool isBoundaryRibbon = false
                    && (f_curr && f_curr->name.find("_offsetBoundaryRibbon") != std::string::npos);
                // if (isRibbon)
                //   continue;
                // Because we didn't reverse the array, O_curr and I_curr are the EXACT SAME CORNER!
                Vertex* O_curr = outVerts[j];
                Vertex* O_next = outVerts[next];
                Vertex* I_curr = inVerts[j];
                Vertex* I_next = inVerts[next];

                Vertex* H_out_curr = outerHoleVerts[j];
                Vertex* H_out_next = outerHoleVerts[next];
                Vertex* H_in_curr = innerHoleVerts[j];
                Vertex* H_in_next = innerHoleVerts[next];
                if (flatOffset)
                {
                    out.addFace({ O_curr, O_next, H_out_next, H_out_curr }, surfOut, "");
                    out.faceList.back()->name = f_curr->name + "_offsetOuterFace";
                    WireFrames.push_back({ O_curr, O_next, H_out_next, H_out_curr, O_curr });
                    continue;
                }
                // 1. OUTER SHELL (Normal points OUT)
                out.addFace({ O_curr, O_next, H_out_next, H_out_curr }, surfOut, "");
                out.faceList.back()->name = f_curr->name + "_offsetOuterFace_" + std::to_string(i)
                    + "_" + std::to_string(j);
                // out.faceList.back()->name = out.faceList.back()->name + "_offsetRibbon";
                WireFrames.push_back({ O_curr, O_next, H_out_next, H_out_curr, O_curr });

                // 2. INNER SHELL (Normal MUST point IN)
                // Winding is reversed compared to outer shell
                out.addFace({ I_curr, H_in_curr, H_in_next, I_next }, surfIn, "");
                // out.addFace({ I_curr, I_next, H_in_next, H_in_curr }, surfIn, "");

                out.faceList.back()->name = f_curr->name + "_offsetInnerFace_" + std::to_string(i)
                    + "_" + std::to_string(j);
                // out.faceList.back()->name = out.faceList.back()->name + "_offsetRibbon";

                WireFrames.push_back({ I_curr, H_in_curr, H_in_next, I_next, I_curr });

                // 3. TUBE WALL (Straight down!)
                if (!isBoundaryRibbon)
                {
                    // Because O_curr and I_curr match, this bridges cleanly without crisscrossing!
                    std::string innerHoleSurf = innerRimSurface;
                    if (innerHoleSurf.empty())
                        innerHoleSurf = surfOut;
                    out.addFace({ H_out_curr, H_out_next, H_in_next, H_in_curr }, innerHoleSurf,
                                "");
                    out.faceList.back()->name = f_curr->name + "_offsetHoleRibbon_"
                        + std::to_string(i) + "_" + std::to_string(j);
                    if (innerRimHidden != true)
                        out.faceList.back()->hide = false; // for hole ribbon
                    else
                        out.faceList.back()->hide = true;
                    WireFrames.push_back(
                        { H_out_curr, H_out_next, H_in_next, H_in_curr, H_out_curr });
                    WireFrames.push_back({ H_out_curr, H_in_curr });
                }
            }
        }
    }
    /*
    for (size_t i = 0; i < outerFaces.size(); ++i)
    {
        delete outerFaces[i];
        delete innerFaces[i];
    }*/
    // out.computeNormals();
    _m_original.computeNormals();
    for (auto v : _m_original.vertList)
    {
        auto outIt = outerVerts.find(v);
        if (outIt != outerVerts.end())
        {
            Vertex* tmpOut = outIt->second;
            tmpOut->normal = v->normal;

            auto holeIt = outerVertsHole.find(tmpOut);
            if (holeIt != outerVertsHole.end())
            {
                for (auto holeOut : holeIt->second)
                {
                    if (holeOut)
                        holeOut->normal = v->normal;
                }
            }
        }

        auto inIt = innerVerts.find(v);
        if (inIt != innerVerts.end())
        {
            Vertex* tmpIn = inIt->second;
            tmpIn->normal = -1 * v->normal;

            auto holeIt = innerVertsHole.find(tmpIn);
            if (holeIt != innerVertsHole.end())
            {
                for (auto holeIn : holeIt->second)
                {
                    if (holeIn)
                        holeIn->normal = v->normal * -1.0f;
                }
            }
        }
    }
    out.buildBoundary();
    /*
    for (auto v : _m_original.vertList)
    {
        // Restore Outer Shell Normals
        if (outerVerts.count(v->name) > 0)
        {
            outerVerts[v->name]->normal = v->normal;
        }

        // Restore Inner Shell Normals
        if (innerVerts.count(v->name) > 0)
        {
            // Note: If you want the inner shell to face inward,
            // you might want to reverse this: = -(v->normal)
            innerVerts[v->name]->normal = v->normal;
        }
    }*/
    for (auto v : out.vertList)
    {
        if (v)
            v->sharpness = 0.0f;
    }
    for (auto edge : out.edges())
    {
        if (edge != nullptr)
        {
            edge->isSharp = false;
            edge->sharpness = 0.0f;
        }
    }
    _m = out;
    return true;
 
}


void CMeshMerger::MergeClear()
{
    currMesh.clear();
    MergedMesh.clear();
}

struct SharpSegment
{
    tc::Vector3 a;
    tc::Vector3 b;
    float sharpness;
};

static float DistSq(const tc::Vector3& p, const tc::Vector3& q)
{
    float dx = p.x - q.x;
    float dy = p.y - q.y;
    float dz = p.z - q.z;
    return dx * dx + dy * dy + dz * dz;
}

static float DotVec(const tc::Vector3& a, const tc::Vector3& b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

static tc::Vector3 SubVec(const tc::Vector3& a, const tc::Vector3& b)
{
    return tc::Vector3(a.x - b.x, a.y - b.y, a.z - b.z);
}

static bool PointOnSegment(
    const tc::Vector3& p,
    const tc::Vector3& a,
    const tc::Vector3& b,
    float eps,
    float& tOut
)
{
    tc::Vector3 ab = SubVec(b, a);
    tc::Vector3 ap = SubVec(p, a);

    float abLenSq = DotVec(ab, ab);

    if (abLenSq < eps * eps)
    {
        tOut = 0.0f;
        return DistSq(p, a) < eps * eps;
    }

    float t = DotVec(ap, ab) / abLenSq;
    tOut = t;

    if (t < -eps || t > 1.0f + eps)
    {
        return false;
    }

    tc::Vector3 projected(
        a.x + t * ab.x,
        a.y + t * ab.y,
        a.z + t * ab.z
    );

    return DistSq(p, projected) < eps * eps;
}

static void ReapplySharpnessToSubdividedEdges(
    DSMesh& mesh,
    const std::vector<SharpSegment>& oldSharpSegments
)
{
    const float eps = 0.001f;

    int reappliedCount = 0;

    for (auto* edge : mesh.edges())
    {
        if (!edge || !edge->v0() || !edge->v1())
        {
            continue;
        }

        tc::Vector3 p0 = edge->v0()->position;
        tc::Vector3 p1 = edge->v1()->position;

        for (const auto& segment : oldSharpSegments)
        {
            float t0 = 0.0f;
            float t1 = 0.0f;

            bool p0OnSegment = PointOnSegment(p0, segment.a, segment.b, eps, t0);
            bool p1OnSegment = PointOnSegment(p1, segment.a, segment.b, eps, t1);

            if (!p0OnSegment || !p1OnSegment)
            {
                continue;
            }

            if (std::abs(t0 - t1) < 0.0001f)
            {
                continue;
            }

            edge->sharpness = std::max(edge->sharpness, segment.sharpness);
            edge->isSharp = true;

            edge->v0()->sharpness = std::max(edge->v0()->sharpness, segment.sharpness);
            edge->v1()->sharpness = std::max(edge->v1()->sharpness, segment.sharpness);

            ++reappliedCount;

            std::cout << "[subdivide] re-applied sharpness "
                      << edge->sharpness
                      << " to child edge "
                      << edge->v0()->name << " - "
                      << edge->v1()->name
                      << std::endl;

            break;
        }
    }

    std::cout << "[subdivide] re-applied sharpness to "
              << reappliedCount
              << " child edge(s)"
              << std::endl;
}

bool CMeshMerger::subdivide(DSMesh& _m, unsigned int n)
{
    DSMesh myCopy = _m.newMakeCopy();
    std::vector<Face*> faceList = myCopy.faceList;

    std::vector<SharpSegment> oldSharpSegments;

if (isSharp)
{
    for (auto* edge : _m.edges())
    {
        if (!edge || !edge->v0() || !edge->v1())
        {
            continue;
        }

        if (edge->sharpness > 0.0f)
        {
            oldSharpSegments.push_back({
                edge->v0()->position,
                edge->v1()->position,
                edge->sharpness
            });

            std::cout << "[subdivide] saved sharp parent edge "
                      << edge->v0()->name << " - "
                      << edge->v1()->name
                      << " sharpness = "
                      << edge->sharpness
                      << std::endl;
        }
    }

    std::cout << "[subdivide] saved "
              << oldSharpSegments.size()
              << " sharp parent edge(s)"
              << std::endl;
}
  

    // Instantiate a Far::TopologyRefiner from the descriptor
    Far::TopologyRefiner* refiner = GetRefiner(_m, isSharp);

    Far::TopologyRefiner::UniformOptions uniop(n);
    // uniop.orderVerticesFromFacesFirst = true;
    refiner->RefineUniform(uniop);

    std::vector<Vertex> vbuffer(refiner->GetNumVerticesTotal());
    Vertex* verts = &vbuffer[0];

    for (int i = 0; i < (int)_m.vertList.size(); ++i)
    {
        auto* v = _m.vertList[i];
        verts[i].SetPosition(v->position.x, v->position.y, v->position.z);
    }

    // Interpolate vertex primvar data
    Far::PrimvarRefiner primvarRefiner(*refiner);

    Vertex* src = verts;
    for (int level = 1; level <= n; ++level)
    {
        Vertex* dst = src + refiner->GetLevel(level - 1).GetNumVertices();
        primvarRefiner.Interpolate(level, src, dst);
        src = dst;
    }
    _m.clear();
    _m.clearAndDelete();
    _m.updateVertListAfterDeletion();
    _m.faceList.clear();
    _m.vertList.clear();
    _m.edgeList.clear();
    _m.boundaryEdgeList().clear();
    { // Output OBJ of the highest level refined -----------
        /// to debug
        Far::TopologyLevel const& refLastLevel = refiner->GetLevel(n);
        int nverts = refLastLevel.GetNumVertices();
        int nfaces = refLastLevel.GetNumFaces();

        // Print vertex positions
        int firstOfLastVerts = refiner->GetNumVerticesTotal() - nverts;

        for (int vert = 0; vert < nverts; ++vert)
        {
            float const* pos = verts[vert + firstOfLastVerts].GetPosition();
            _m.addVertex(pos[0], pos[1], pos[2]);
        }
        // Print faces
        for (int face = 0; face < nfaces; face++)
        {
            Far::ConstIndexArray fverts = refLastLevel.GetFaceVertices(face);
            auto curr = refLastLevel.GetFaceParentFace(face);
            int temp = n - 1;
            /*
            while (temp > 0)
            {
                curr = refLastLevel.GetFaceParentFace(curr);
                temp--;
            }
            */
            int idx = face;
            for (int l = n; l > 0; --l)
            {
                idx = refiner->GetLevel(l).GetFaceParentFace(idx);
            }
            // all refined Catmark faces should be quads
            assert(fverts.size() == 4);
            std::vector<Vertex*> vertices;
            for (int i = 0; i < 4; ++i)
            {
                vertices.push_back(_m.vertList.at(fverts[i]));
            }
            // int index = (face * faceList.size()) / nfaces;
            int index = idx;
            // floor(face / static_cast<int>(std::pow(4, n)));
            // int index = floor(face / floor(nfaces / faceList.size()));
            // int index = (face * faceList.size()) / nfaces;
            if (index >= faceList.size())
            {
                std::cout << "exceeded: " << index << "\n";
                index = faceList.size() - 1;
            }

            std::string surfaceName = faceList.at(index)->surfaceName;
            std::string backfaceName = faceList.at(index)->backfaceName;
            if (surfaceName.empty())
            {
                surfaceName = "";
            }
            if (backfaceName.empty() || (backfaceName.substr(0, 10)).compare("SubdivVert") == 0)
            {
                backfaceName = "";
            }

            _m.addFace(vertices, surfaceName, backfaceName);
            WireFrames.push_back(vertices);
        }
        if (isSharp)
{
    ReapplySharpnessToSubdividedEdges(_m, oldSharpSegments);
}

_m.computeNormals();
_m.buildBoundary();

for (int i = 0; i < (int)_m.vertList.size(); ++i)
    _m.vertList[i]->ID = i;
        for (int i = 0; i < (int)_m.faceList.size(); ++i)
            _m.faceList[i]->id = i;
        int maxID = -1;
        for (auto* v : _m.vertList)
            maxID = std::max(int(maxID), int(v->ID));
        std::cout << "verts=" << _m.vertList.size() << " maxID=" << maxID << "\n";
        std::cout << "vertList=" << _m.vertList.size() << " faceList=" << _m.faceList.size()
                  << " edgeList=" << _m.edgeList.size()
                  << " nameToVert=" << _m.nameToVert.size() // if exists
                  << " nameToFace=" << _m.nameToFace.size() // if exists
                  << "\n";
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

        // Horribly messy. Organize this better in the future so subdivision cleans up all data
        // structures accordingly
        for (int i = 0; i < newMesh.faceList.size(); i++)
        {
            Face* currFace = newMesh.faceList[i];
            std::string faceName = "subdivFace" + std::to_string(i);
            currFace->name = faceName;
            newMesh.nameToFace[faceName] = newMesh.faceList[i];
        }

        for (int i = 0; i < newMesh.vertList.size(); i++)
        {
            Vertex* currVert = newMesh.vertList[i];
            std::string vertName = "subdivVert" + std::to_string(i);
            currVert->name = vertName;
            newMesh.nameToVert[vertName] = newMesh.vertList[i];
        }

        currMesh.clear();
        currMesh = newMesh.newMakeCopy();
        newMesh.clear();
    }
    // return currMesh;
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
                    }
                    else
                    {
                        newEdgePoint->position =
                            (faceVert1->position + edgeVert1->position) / (float)2.0;
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