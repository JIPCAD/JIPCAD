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
    std::cout << "\nNum Faces:" << otherMesh.faceList.size() << "\n";
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
        /*
        std::cout << "DSMesh: v=" << currMesh.n_vertices() << " f=" << currMesh.n_faces() << "\n"
                  << "OpenMesh: v=" << Mesh.n_vertices() << " f=" << Mesh.n_faces() << "\n";
        std::cout << "\n Curr Mesh Properties (the one that's rendered): ";
        std::cout << "vertList=" << currMesh.vertList.size()
                  << " faceList=" << currMesh.faceList.size()
                  << " edgeList=" << currMesh.edgeList.size()
                  << " nameToVert=" << currMesh.nameToVert.size() // if exists
                  << " nameToFace=" << currMesh.nameToFace.size() // if exists
                  << "\n";
        */
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
            /*
            std::cout << "[mergeCurr] transferred sharpness "
                      << mergedEdge->sharpness
                      << " to edge "
                      << mergedEdge->v0()->name << " - "
                      << mergedEdge->v1()->name
                      << std::endl;
                      */
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
            //printf("set sharpness: %f\n", closestVert->sharpness);
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
        /*
        std::cout << "[merge] transferred sharpness "
                  << mergedEdge->sharpness << " to edge "
                  << mergedEdge->v0()->name << " - "
                  << mergedEdge->v1()->name << std::endl;*/
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

#include <omp.h>
#include <cmath>

#include <memory>



#include <omp.h>

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <new>
#include <string>
#include <vector>

// ============================================================================
// Tunables
// ============================================================================

#ifndef MESH_EMIT_DEBUG_NAMES
#define MESH_EMIT_DEBUG_NAMES 0
#endif

#ifndef MESH_USE_VERTEX_POOL
#define MESH_USE_VERTEX_POOL 0
#endif

#ifndef MESH_FREE_SOURCE_FACES_EARLY
#define MESH_FREE_SOURCE_FACES_EARLY 0
#endif

#ifndef MESH_CLEAR_SHARPNESS
#define MESH_CLEAR_SHARPNESS 1
#endif

namespace
{
#if MESH_USE_VERTEX_POOL
    class VertexArena
    {
    public:
        VertexArena() = default;
        ~VertexArena() { }

        Vertex* make(double x, double y, double z, int id)
        {
            if (used_ >= kBlock)
            {
                cur_ = static_cast<Vertex*>(::operator new(sizeof(Vertex) * kBlock));
                used_ = 0;
            }
            return new (cur_ + used_++) Vertex(x, y, z, id);
        }

    private:
        static constexpr size_t kBlock = 1u << 16;
        Vertex* cur_ = nullptr;
        size_t used_ = kBlock;
    };
#endif

    // Bit-packing 64-bit Edge Key:
    // Bits 43..63 (21 bits) -> v_min (up to 2,097,151)
    // Bits 22..42 (21 bits) -> v_max (up to 2,097,151)
    // Bit  21     (1 bit)  -> dir   (0: v1=v_min, 1: v1=v_max)
    // Bits 0..20  (21 bits) -> face  (up to 2,097,151)
    inline uint64_t packEdge(uint32_t v1, uint32_t v2, uint32_t fi)
    {
        uint64_t v_min = std::min(v1, v2);
        uint64_t v_max = std::max(v1, v2);
        uint64_t dir = (v1 > v2) ? 1ull : 0ull;
        return (v_min << 43) | (v_max << 22) | (dir << 21)
            | (static_cast<uint64_t>(fi) & 0x1FFFFFull);
    }

    inline uint64_t getEdgePairKey(uint64_t packed)
    {
        return packed >> 22; // Extracts (v_min << 21) | v_max
    }

    inline void unpackEdge(uint64_t packed, uint32_t& v1, uint32_t& v2, uint32_t& fi)
    {
        uint32_t v_min = static_cast<uint32_t>((packed >> 43) & 0x1FFFFFull);
        uint32_t v_max = static_cast<uint32_t>((packed >> 22) & 0x1FFFFFull);
        uint32_t dir = static_cast<uint32_t>((packed >> 21) & 0x1ull);
        fi = static_cast<uint32_t>(packed & 0x1FFFFFull);
        v1 = dir ? v_max : v_min;
        v2 = dir ? v_min : v_max;
    }

} // namespace

#if MESH_EMIT_DEBUG_NAMES
char nameBuf[256];
#define MESH_FMT_NAME(target, ...)                                                                 \
    do                                                                                             \
    {                                                                                              \
        snprintf(nameBuf, sizeof(nameBuf), __VA_ARGS__);                                           \
        (target) = nameBuf;                                                                        \
    } while (0)
#define MESH_SET_NAME(target, expr)                                                                \
    do                                                                                             \
    {                                                                                              \
        (target) = (expr);                                                                         \
    } while (0)
#else
#define MESH_FMT_NAME(target, ...)                                                                 \
    do                                                                                             \
    {                                                                                              \
    } while (0)
#define MESH_SET_NAME(target, expr)                                                                \
    do                                                                                             \
    {                                                                                              \
    } while (0)
#endif

bool CMeshMerger::offset(DSMesh& _m, double height, double width, std::string outerRimSurface,
                         std::string innerRimSurface, bool outerRimHidden, bool innerRimHidden)
{
    if (height < 0 && width < 0)
        return true;

    width = 1.0 - width;
    if (width >= 1.0)
        width = 0.0;

    const bool flatOffset = std::abs(height) < 1e-8;
    const bool hasHole = (width != 0.0);
    const double d = height * 0.5;

    _m.computeNormals();

    // ------------------------------------------------------------------
    // 1. Compact source
    // ------------------------------------------------------------------
    std::vector<Vertex*> sv;
    sv.reserve(_m.vertList.size());
    for (Vertex* v : _m.vertList)
        if (v)
            sv.push_back(v);
    const int nV = static_cast<int>(sv.size());
    for (int i = 0; i < nV; ++i)
        sv[i]->ID = i;

    std::vector<Face*> sf;
    sf.reserve(_m.faceList.size());
    for (Face* f : _m.faceList)
        if (f && !f->vertices.empty())
            sf.push_back(f);
    const int nF = static_cast<int>(sf.size());

    std::vector<uint32_t> fCornerStart(nF + 1, 0);
    for (int i = 0; i < nF; ++i)
        fCornerStart[i + 1] = fCornerStart[i] + static_cast<uint32_t>(sf[i]->vertices.size());
    const uint32_t nCorners = fCornerStart[nF];

    std::vector<uint32_t> cornerVid(nCorners);
    for (int i = 0; i < nF; ++i)
    {
        uint32_t o = fCornerStart[i];
        for (Vertex* v : sf[i]->vertices)
            cornerVid[o++] = static_cast<uint32_t>(v->ID);
    }

    // ------------------------------------------------------------------
    // 2. True SoA Data Layout (SIMD Friendly)
    // ------------------------------------------------------------------
    std::vector<double> srcX(nV), srcY(nV), srcZ(nV);
    for (int i = 0; i < nV; ++i)
    {
        srcX[i] = sv[i]->position.x;
        srcY[i] = sv[i]->position.y;
        srcZ[i] = sv[i]->position.z;
    }

    std::vector<double> fNrmX(nF), fNrmY(nF), fNrmZ(nF);
    for (int i = 0; i < nF; ++i)
    {
        fNrmX[i] = sf[i]->normal.x;
        fNrmY[i] = sf[i]->normal.y;
        fNrmZ[i] = sf[i]->normal.z;
    }

    std::vector<double> vNrmX(nV, 0.0), vNrmY(nV, 0.0), vNrmZ(nV, 0.0);
    std::vector<double> miter(nV, d);

    // ------------------------------------------------------------------
    // 3. Fused Gather Pass with RAII Scoped CSR Arrays
    // ------------------------------------------------------------------
    {
        std::vector<uint32_t> vFaceStart(nV + 1, 0);
        for (uint32_t c = 0; c < nCorners; ++c)
            ++vFaceStart[cornerVid[c] + 1];
        for (int i = 0; i < nV; ++i)
            vFaceStart[i + 1] += vFaceStart[i];

        std::vector<uint32_t> vFace(nCorners);
        {
            std::vector<uint32_t> cursor(vFaceStart.begin(), vFaceStart.end() - 1);
            for (int fi = 0; fi < nF; ++fi)
                for (uint32_t c = fCornerStart[fi]; c < fCornerStart[fi + 1]; ++c)
                    vFace[cursor[cornerVid[c]]++] = static_cast<uint32_t>(fi);
        }

#pragma omp parallel for schedule(static) if (nV > 8192)
        for (int i = 0; i < nV; ++i)
        {
            const uint32_t b = vFaceStart[i];
            const uint32_t e = vFaceStart[i + 1];

            double nx = 0.0, ny = 0.0, nz = 0.0;
#pragma omp simd reduction(+ : nx, ny, nz)
            for (uint32_t k = b; k < e; ++k)
            {
                const uint32_t fIdx = vFace[k];
                nx += fNrmX[fIdx];
                ny += fNrmY[fIdx];
                nz += fNrmZ[fIdx];
            }

            const double len2 = nx * nx + ny * ny + nz * nz;
            if (len2 > 1e-24)
            {
                const double inv = 1.0 / std::sqrt(len2);
                nx *= inv;
                ny *= inv;
                nz *= inv;
            }
            vNrmX[i] = nx;
            vNrmY[i] = ny;
            vNrmZ[i] = nz;

            if (e > b)
            {
                double minDot = 1.0;
                for (uint32_t k = b; k < e; ++k)
                {
                    const uint32_t fIdx = vFace[k];
                    const double dot = nx * fNrmX[fIdx] + ny * fNrmY[fIdx] + nz * fNrmZ[fIdx];
                    if (dot < minDot)
                        minDot = dot;
                }
                miter[i] = d / std::max(0.2, minDot);
            }
        }
    } // vFace and vFaceStart memory freed HERE automatically

    // ------------------------------------------------------------------
    // 4. Bit-Packed Fast Boundary Scan (Scoped)
    // ------------------------------------------------------------------
    std::vector<uint64_t> boundaryEdges;
    if (!flatOffset)
    {
        std::vector<uint64_t> packedEdges(nCorners);
        for (int fi = 0; fi < nF; ++fi)
        {
            const uint32_t b = fCornerStart[fi];
            const uint32_t n = fCornerStart[fi + 1] - b;
            for (uint32_t j = 0; j < n; ++j)
            {
                uint32_t v1 = cornerVid[b + j];
                uint32_t v2 = cornerVid[b + (j + 1) % n];
                packedEdges[b + j] = packEdge(v1, v2, static_cast<uint32_t>(fi));
            }
        }

        std::sort(packedEdges.begin(), packedEdges.end());

        boundaryEdges.reserve(nCorners / 16);
        for (size_t i = 0; i < nCorners; ++i)
        {
            const uint64_t key = getEdgePairKey(packedEdges[i]);
            const bool hasPrev = (i > 0) && (getEdgePairKey(packedEdges[i - 1]) == key);
            const bool hasNext = (i + 1 < nCorners) && (getEdgePairKey(packedEdges[i + 1]) == key);
            if (!hasPrev && !hasNext)
            {
                boundaryEdges.push_back(packedEdges[i]);
            }
        }
    } // packedEdges memory freed HERE automatically

    // ------------------------------------------------------------------
    // 5. Output Mesh Setup
    // ------------------------------------------------------------------
    const size_t outVertCount = static_cast<size_t>(2) * nV + (hasHole ? 2ull * nCorners : 0ull);
    size_t outFaceCount =
        hasHole ? (flatOffset ? nCorners : 3ull * nCorners) : static_cast<size_t>(2) * nF;
    outFaceCount += boundaryEdges.size();

    DSMesh out;
    out.vertList.reserve(outVertCount);
    out.faceList.reserve(outFaceCount);

#if MESH_USE_VERTEX_POOL
    VertexArena arena;
    auto makeVertex = [&](double x, double y, double z)
    { return arena.make(x, y, z, static_cast<int>(out.vertList.size())); };
#else
    auto makeVertex = [&](double x, double y, double z)
    { return new Vertex(x, y, z, static_cast<int>(out.vertList.size())); };
#endif

    // ------------------------------------------------------------------
    // 6. Shell Vertex Generation
    // ------------------------------------------------------------------
    std::vector<Vertex*> outerVerts(nV, nullptr);
    std::vector<Vertex*> innerVerts(nV, nullptr);

    for (int i = 0; i < nV; ++i)
    {
        const double px = srcX[i], py = srcY[i], pz = srcZ[i];
        const double nx = vNrmX[i], ny = vNrmY[i], nz = vNrmZ[i];
        const double m = miter[i];

        Vertex* ov = makeVertex(px + m * nx, py + m * ny, pz + m * nz);
        ov->normal = tc::Vector3(nx, ny, nz);
        MESH_SET_NAME(ov->name, sv[i]->name + "_offsetOuter");
        outerVerts[i] = ov;
        out.addVertex(ov);

        Vertex* iv = makeVertex(px - m * nx, py - m * ny, pz - m * nz);
        iv->normal = tc::Vector3(-nx, -ny, -nz);
        MESH_SET_NAME(iv->name, sv[i]->name + "_offsetInner");
        innerVerts[i] = iv;
        out.addVertex(iv);
    }

    std::vector<double>().swap(miter);

    // ------------------------------------------------------------------
    // 7. Boundary Ribbons
    // ------------------------------------------------------------------
    if (!flatOffset)
    {
        std::vector<Vertex*> ribbon(4);
        for (uint64_t packed : boundaryEdges)
        {
            uint32_t v1, v2, fi;
            unpackEdge(packed, v1, v2, fi);

            ribbon[0] = outerVerts[v2];
            ribbon[1] = outerVerts[v1];
            ribbon[2] = innerVerts[v1];
            ribbon[3] = innerVerts[v2];

            const std::string& rim = sf[fi]->surfaceName;
            out.addFace(ribbon, outerRimSurface.empty() ? rim : outerRimSurface, "");
            out.faceList.back()->hide = outerRimHidden;
            MESH_SET_NAME(out.faceList.back()->name,
                          out.faceList.back()->name + "_offsetBoundaryRibbon");
        }
    }
    std::vector<uint64_t>().swap(boundaryEdges);

    // ------------------------------------------------------------------
    // 8. Main Face Loop
    // ------------------------------------------------------------------
    std::vector<Vertex*> outerRing, innerRing, holeOut, holeIn, scratch;
    outerRing.reserve(16);
    innerRing.reserve(16);
    holeOut.reserve(16);
    holeIn.reserve(16);
    scratch.reserve(16);
    std::vector<Vertex*> quad(4);

    for (int fi = 0; fi < nF; ++fi)
    {
        Face* f = sf[fi];
        const uint32_t base = fCornerStart[fi];
        const int n = static_cast<int>(fCornerStart[fi + 1] - base);

        outerRing.clear();
        innerRing.clear();
        for (int j = 0; j < n; ++j)
        {
            const uint32_t vid = cornerVid[base + j];
            outerRing.push_back(outerVerts[vid]);
            innerRing.push_back(innerVerts[vid]);
        }

        if (!hasHole)
        {
            out.addFace(outerRing, f->surfaceName, f->backfaceName);
            scratch.assign(innerRing.rbegin(), innerRing.rend());
            out.addFace(scratch, f->surfaceName, f->backfaceName);
#if MESH_FREE_SOURCE_FACES_EARLY
            delete f;
            sf[fi] = nullptr;
#endif
            continue;
        }

        const double fnx = fNrmX[fi], fny = fNrmY[fi], fnz = fNrmZ[fi];
        const double invN = 1.0 / static_cast<double>(n);

        double cx = 0.0, cy = 0.0, cz = 0.0;
        for (int j = 0; j < n; ++j)
        {
            const uint32_t vid = cornerVid[base + j];
            cx += srcX[vid];
            cy += srcY[vid];
            cz += srcZ[vid];
        }
        cx *= invN;
        cy *= invN;
        cz *= invN;

        holeOut.clear();
        holeIn.clear();
        for (int j = 0; j < n; ++j)
        {
            const uint32_t vid = cornerVid[base + j];
            const double px = srcX[vid], py = srcY[vid], pz = srcZ[vid];
            const double vnx = vNrmX[vid], vny = vNrmY[vid], vnz = vNrmZ[vid];

            const double hx = cx + (px - cx) * width;
            const double hy = cy + (py - cy) * width;
            const double hz = cz + (pz - cz) * width;

            Vertex* ho = makeVertex(hx + d * fnx, hy + d * fny, hz + d * fnz);
            ho->normal = tc::Vector3(vnx, vny, vnz);
            MESH_FMT_NAME(ho->name, "%s_holeOut_%d_%d", f->name.c_str(), fi, j);
            out.addVertex(ho);
            holeOut.push_back(ho);

            Vertex* hi = makeVertex(hx - d * fnx, hy - d * fny, hz - d * fnz);
            hi->normal = tc::Vector3(-vnx, -vny, -vnz);
            MESH_FMT_NAME(hi->name, "%s_holeIn_%d_%d", f->name.c_str(), fi, j);
            out.addVertex(hi);
            holeIn.push_back(hi);
        }

        const std::string& surf = f->surfaceName;
        const std::string& holeRimSurf = innerRimSurface.empty() ? surf : innerRimSurface;

        for (int j = 0; j < n; ++j)
        {
            const int k = (j + 1 == n) ? 0 : j + 1;

            quad[0] = outerRing[j];
            quad[1] = outerRing[k];
            quad[2] = holeOut[k];
            quad[3] = holeOut[j];
            out.addFace(quad, surf, "");

            if (flatOffset)
            {
                MESH_SET_NAME(out.faceList.back()->name, f->name + "_offsetOuterFace");
                continue;
            }
            MESH_FMT_NAME(out.faceList.back()->name, "%s_offsetOuterFace_%d_%d", f->name.c_str(),
                          fi, j);

            quad[0] = innerRing[j];
            quad[1] = holeIn[j];
            quad[2] = holeIn[k];
            quad[3] = innerRing[k];
            out.addFace(quad, surf, "");
            MESH_FMT_NAME(out.faceList.back()->name, "%s_offsetInnerFace_%d_%d", f->name.c_str(),
                          fi, j);

            quad[0] = holeOut[j];
            quad[1] = holeOut[k];
            quad[2] = holeIn[k];
            quad[3] = holeIn[j];
            out.addFace(quad, holeRimSurf, "");
            out.faceList.back()->hide = innerRimHidden;
            MESH_FMT_NAME(out.faceList.back()->name, "%s_offsetHoleRibbon_%d_%d", f->name.c_str(),
                          fi, j);
        }

#if MESH_FREE_SOURCE_FACES_EARLY
        delete f;
        sf[fi] = nullptr;
#endif
    }

    // ------------------------------------------------------------------
    // 9. Cleanup & Mesh Assignment
    // ------------------------------------------------------------------
    std::vector<uint32_t>().swap(cornerVid);
    std::vector<uint32_t>().swap(fCornerStart);
    std::vector<double>().swap(srcX);
    std::vector<double>().swap(srcY);
    std::vector<double>().swap(srcZ);
    std::vector<double>().swap(vNrmX);
    std::vector<double>().swap(vNrmY);
    std::vector<double>().swap(vNrmZ);
    std::vector<double>().swap(fNrmX);
    std::vector<double>().swap(fNrmY);
    std::vector<double>().swap(fNrmZ);
    std::vector<Vertex*>().swap(outerVerts);
    std::vector<Vertex*>().swap(innerVerts);

    out.buildBoundary();

#if MESH_CLEAR_SHARPNESS
    for (Vertex* v : out.vertList)
        if (v)
            v->sharpness = 0.0f;

    for (auto edge : out.edges())
    {
        if (edge)
        {
            edge->isSharp = false;
            edge->sharpness = 0.0f;
        }
    }
#endif

#if MESH_FREE_SOURCE_FACES_EARLY
    _m.faceList.clear();
#endif
    _m.clearAndDelete();
    _m = std::move(out);

    return true;
}

#undef MESH_FMT_NAME
#undef MESH_SET_NAME


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
            /*
            std::cout << "[subdivide] re-applied sharpness "
                      << edge->sharpness
                      << " to child edge "
                      << edge->v0()->name << " - "
                      << edge->v1()->name
                      << std::endl;
                      */
            break;
        }
    }
    /*
    std::cout << "[subdivide] re-applied sharpness to "
              << reappliedCount
              << " child edge(s)"
              << std::endl;
              */
}

#include <memory>
#include <numeric>
#include <omp.h>
#include <string>
#include <vector>

bool CMeshMerger::subdivide(DSMesh& _m, unsigned int n)
{
    if (_m.vertList.empty() || _m.faceList.empty())
        return false;

    // Force global scope resolution to prevent namespace collisions inside Nome::Scene
    namespace Far = ::OpenSubdiv::Far;

    const int oldFaceCount = static_cast<int>(_m.faceList.size());
    const int oldVertCount = static_cast<int>(_m.vertList.size());

    // 1. Parallel Face Metadata Extraction (Owning std::string copies)
    struct FaceMeta
    {
        std::string surfaceName;
        std::string backfaceName;
    };
    std::vector<FaceMeta> faceMeta(oldFaceCount);

#pragma omp parallel for schedule(static)
    for (int i = 0; i < oldFaceCount; ++i)
    {
        const auto* f = _m.faceList[i];
        if (f)
        {
            std::string bName = f->backfaceName;
            if (bName.size() >= 10 && bName.compare(0, 10, "SubdivVert") == 0)
            {
                bName.clear();
            }
            faceMeta[i] = { f->surfaceName, std::move(bName) };
        }
    }

    // 2. Preserve Sharp Edges
    std::vector<SharpSegment> oldSharpSegments;
    if (isSharp)
    {
        for (auto* edge : _m.edges())
        {
            if (edge && edge->v0() && edge->v1() && edge->sharpness > 0.0f)
            {
                oldSharpSegments.push_back(
                    { edge->v0()->position, edge->v1()->position, edge->sharpness });
            }
        }
    }

    // 3. OpenSubdiv Setup
    Far::TopologyRefiner* refiner = GetRefiner(_m, isSharp);
    Far::TopologyRefiner::UniformOptions uniop(n);
    refiner->RefineUniform(uniop);

    const int totalVerts = refiner->GetNumVerticesTotal();
    std::vector<Vertex> vbuffer(totalVerts);

    // 4. Parallel Copy of Initial Positions
#pragma omp parallel for schedule(static)
    for (int i = 0; i < oldVertCount; ++i)
    {
        const auto* v = _m.vertList[i];
        vbuffer[i].SetPosition(v->position.x, v->position.y, v->position.z);
    }

    // 5. Primvar Interpolation via PrimvarRefiner
    Far::PrimvarRefiner primvarRefiner(*refiner);
    Vertex* src = vbuffer.data();
    for (unsigned int level = 1; level <= n; ++level)
    {
        Vertex* dst = src + refiner->GetLevel(level - 1).GetNumVertices();
        primvarRefiner.Interpolate(level, src, dst);
        src = dst;
    }

    // 6. Precompute Base Parent Face Map Iteratively in O(Total Faces)
    std::vector<int> faceMap(refiner->GetLevel(0).GetNumFaces());
    std::iota(faceMap.begin(), faceMap.end(), 0);

    for (unsigned int l = 1; l <= n; ++l)
    {
        const auto& level = refiner->GetLevel(l);
        const int numLFaces = level.GetNumFaces();
        std::vector<int> nextMap(numLFaces);
        for (int f = 0; f < numLFaces; ++f)
        {
            nextMap[f] = faceMap[level.GetFaceParentFace(f)];
        }
        faceMap = std::move(nextMap);
    }

    // Clear old mesh (Destroys original face pointers)
    _m.clear();
    _m.clearAndDelete();
    _m.updateVertListAfterDeletion();
    _m.faceList.clear();
    _m.vertList.clear();
    _m.edgeList.clear();
    _m.boundaryEdgeList().clear();

    // 7. Mesh Reconstruction
    Far::TopologyLevel const& refLastLevel = refiner->GetLevel(n);
    const int nverts = refLastLevel.GetNumVertices();
    const int nfaces = refLastLevel.GetNumFaces();
    const int firstOfLastVerts = totalVerts - nverts;

    _m.vertList.reserve(nverts);
    _m.faceList.reserve(nfaces);

    for (int vert = 0; vert < nverts; ++vert)
    {
        float const* pos = vbuffer[vert + firstOfLastVerts].GetPosition();
        _m.addVertex(pos[0], pos[1], pos[2]);
    }

    std::vector<Vertex*> quadVertices(4);
    for (int face = 0; face < nfaces; ++face)
    {
        Far::ConstIndexArray fverts = refLastLevel.GetFaceVertices(face);
        int idx = faceMap[face];
        if (idx < 0 || idx >= oldFaceCount)
            idx = std::clamp(idx, 0, oldFaceCount - 1);

        quadVertices[0] = _m.vertList[fverts[0]];
        quadVertices[1] = _m.vertList[fverts[1]];
        quadVertices[2] = _m.vertList[fverts[2]];
        quadVertices[3] = _m.vertList[fverts[3]];

        _m.addFace(quadVertices, faceMeta[idx].surfaceName, faceMeta[idx].backfaceName);
    }

    // 8. Post-Processing
    if (isSharp)
    {
        ReapplySharpnessToSubdividedEdges(_m, oldSharpSegments);
    }

    _m.computeNormals();
    _m.buildBoundary();

    const int finalVertCount = static_cast<int>(_m.vertList.size());
#pragma omp parallel for schedule(static)
    for (int i = 0; i < finalVertCount; ++i)
    {
        _m.vertList[i]->ID = i;
    }

    const int finalFaceCount = static_cast<int>(_m.faceList.size());
#pragma omp parallel for schedule(static)
    for (int i = 0; i < finalFaceCount; ++i)
    {
        _m.faceList[i]->id = i;
    }

    delete refiner;
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