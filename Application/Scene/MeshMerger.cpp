#include "MeshMerger.h"
#include "OffsetRefiner.h"
#include "ShellRefiner.h"
#include "Subdivision.h"

#include <cmath>
#include <unordered_map>
#include <algorithm>
#include <vector>
#include <limits>
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

inline static const char* OpenSubdivRuleName(Sdc::Crease::Rule rule)
{
    switch (rule)
    {
    case Sdc::Crease::RULE_SMOOTH:
        return "SMOOTH";

    case Sdc::Crease::RULE_DART:
        return "DART";

    case Sdc::Crease::RULE_CREASE:
        return "CREASE";

    case Sdc::Crease::RULE_CORNER:
        return "CORNER";

    default:
        return "UNKNOWN";
    }
}

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

    if (needOffset)
    {
        // offset(otherMesh);
        offset(otherMesh, h, w, _outerRimSurface, _innerRimSurface, _outerRimHidden, _innerRimHidden);
        std::cout << "Apply offset, may take some time..." << std::endl;
        didOffset = true;
    }

    while (subdivisionLevel > 0)
    {
        // std::cout << "\nsubdivLevel again: " << subdivisionLevel << "\n";
        subdivide(otherMesh, 1); //, isSharp); // Randy commented this out for now. add back asap
        // 4/30/2025 - Robert made this 1 level at a time, see note at the end of the Catmull
        // function.
        std::cout << "Apply catmullclark subdivision, may take some time..." << std::endl;
        subdivisionLevel--;
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

// offset only added here
// Randy changed it to use DSMesh
// working with color
/*
bool CMeshMerger::offset(DSMesh& _m)
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
            newVert->name = "offsetVert" + std::to_string(i); // Randy this was the bug. Need to
name the Vert before adding it! Fix this logic. _m.addVertex(newVert); newVerts.push_back(newVert);
        }
        _m.addFace(newVerts,face->surfaceName, face->backfaceName);
    }

   _m.buildBoundary(); // Randy added this on 2/26
    _m.computeNormals();
    return true;
}
*/
// New Version that fixes subdivision chaining (ie offset and then subdiv)
#include <algorithm>
#include <map>
#include <set>
#include <string>
#include <vector>
/*
bool CMeshMerger::offset(DSMesh& _m, double height, double width)
{
    //double height = Height.GetValue(h);
    //double width = Width.GetValue(w);
    if (height <= 0 && width <= 0)
        return true;

    COffsetRefiner offsetRefiner(_m, offsetFlag);
    offsetRefiner.Refine(height, width);

    std::vector<Face*> rawFaces = offsetRefiner.GetFaces();
    std::sort(rawFaces.begin(), rawFaces.end(),
              [](Face* a, Face* b)
              {
                  return a->vertices.size() > b->vertices.size();
              });
    _m.clear();

    // Welding Cache
    std::map<std::string, Vertex*> uniqueVerts;

    // Face Deduplication
    std::set<std::vector<int>> existingFaces;

    // Ensures we never add a 3rd face to an edge, preventing Non-Manifold errors.
    std::map<std::pair<int, int>, int> edgeUsage;

    printf("============ output verts and faces with MANIFOLD GUARD ======\n");

    int addedFaceCount = 0;
    int skippedCount = 0;

    for (auto face : rawFaces)
    {
        std::vector<Vertex*> faceVerts;
        std::vector<int> faceIndices;

        // Merge vertices in the same position
        for (int i = 0; i < face->vertices.size(); i++)
        {
            auto sourceVert = face->vertices[i];

            char buffer[64];
            snprintf(buffer, sizeof(buffer), "%.4f_%.4f_%.4f", sourceVert->position.x,
                     sourceVert->position.y, sourceVert->position.z);
            std::string key = buffer;

            Vertex* finalVert = nullptr;
            if (uniqueVerts.find(key) != uniqueVerts.end())
            {
                finalVert = uniqueVerts[key];
            }
            else
            {
                finalVert = new Vertex(sourceVert->position.x, sourceVert->position.y,
                                       sourceVert->position.z, _m.vertList.size());
                finalVert->name = "offsetVert" + std::to_string(_m.vertList.size());
                _m.addVertex(finalVert);
                uniqueVerts[key] = finalVert;
            }
            faceVerts.push_back(finalVert);
            faceIndices.push_back(finalVert->ID);
        }


        bool isDegenerate = false;
        for (size_t i = 0; i < faceIndices.size(); ++i)
        {
            if (faceIndices[i] == faceIndices[(i + 1) % faceIndices.size()])
            {
                isDegenerate = true;
                break;
            }
        }
        if (isDegenerate)
            continue;
        WireFrames.push_back(faceVerts);
        std::vector<int> sortedIndices = faceIndices;
        std::sort(sortedIndices.begin(), sortedIndices.end());
        if (existingFaces.count(sortedIndices))
            continue;

        Vector3 v0 = faceVerts[0]->position;
        Vector3 v1 = faceVerts[1]->position;
        Vector3 v2 = faceVerts[2]->position;

        // We check squared length against a tiny epsilon
        float e0Sq = (v1 - v0).LengthSquared();
        float e1Sq = (v2 - v1).LengthSquared();
        float e2Sq = (v0 - v2).LengthSquared();

        if (e0Sq < 1e-8f || e1Sq < 1e-8f || e2Sq < 1e-8f)
        {
            skippedCount++;
            continue;
        }

        // Previous check was scale dependent. This one is consistent.
        // We reject anything below 0.005 (approx 1:10 aspect ratio).
        Vector3 edgeA = v1 - v0;
        Vector3 edgeB = v2 - v0;
        Vector3 cross = edgeA.CrossProduct(edgeB);
        float doubleArea = cross.Length();
        float perimeter = sqrt(e0Sq) + sqrt(e1Sq) + sqrt(e2Sq);

        if (perimeter < 1e-9f)
        {
            skippedCount++;
            continue;
        } // Prevent divide by zero

        // Ratio = Area / Perimeter^2
        // A sliver has a very small area relative to its perimeter squared.
        float aspect = doubleArea / (perimeter * perimeter);

        if (aspect < 0.005f)
        {
            skippedCount++;
            continue;
        }

        // Ensure strictly positive facing.
        if (face->normal.LengthSquared() > 0.001f)
        {
            if (cross.DotProduct(face->normal) < 1e-4f)
            {
                skippedCount++;
                continue;
            }
        }
        // Check if adding this face would overload any edge ( > 2 faces)
        bool isManifold = true;
        for (size_t i = 0; i < faceIndices.size(); ++i)
        {
            int u = faceIndices[i];
            int v = faceIndices[(i + 1) % faceIndices.size()];

            std::pair<int, int> edgeKey = std::minmax(u, v);

            if (edgeUsage[edgeKey] >= 2)
            {
                isManifold = false;
                break;
            }
        }

        if (!isManifold)
        {
            skippedCount++;
            continue;
        }

        for (size_t i = 0; i < faceIndices.size(); ++i)
        {
            int u = faceIndices[i];
            int v = faceIndices[(i + 1) % faceIndices.size()];
            edgeUsage[std::minmax(u, v)]++;
        }

        existingFaces.insert(sortedIndices);

        _m.addFace(faceVerts, face->surfaceName, face->backfaceName);
        addedFaceCount++;
    }
    std::vector<bool> vertIsUsed(_m.vertList.size(), false);
    for (auto face : _m.faceList)
    {
        for (auto v : face->vertices)
        {
            vertIsUsed[v->ID] = true;
        }
    }

    // 2. Create a compact list and delete unused vertices
    std::vector<Vertex*> packedVerts;
    packedVerts.reserve(_m.vertList.size()); // Reserve max potential size

    for (size_t i = 0; i < _m.vertList.size(); ++i)
    {
        Vertex* v = _m.vertList[i];
        if (vertIsUsed[i])
        {
            // Update ID to match the new contiguous index
            v->ID = (int)packedVerts.size();
            packedVerts.push_back(v);
        }
        else
        {
            // Vertex was created but its face was rejected. Delete it.
            delete v;
        }
    }

    // 3. Swap the clean list back into the mesh
    _m.vertList = packedVerts;
    printf("Rebuild Complete: Added %d faces, Skipped %d non-manifold faces.\n", addedFaceCount,
           skippedCount);

    _m.buildBoundary();

    if (addedFaceCount > 0)
    {
        _m.computeNormals();
    }

    return true;
}
*/

// New New version
tc::Vector3 Nome::Scene::CMeshMerger::calculate3PlaneIntersection(tc::Vector3 p, tc::Vector3 n1,
                                                                  tc::Vector3 n2, tc::Vector3 n3,
                                                                  double offsetDistance)
{
    // Original distances from origin
    double d1 = (n1.x * p.x) + (n1.y * p.y) + (n1.z * p.z);
    double d2 = (n2.x * p.x) + (n2.y * p.y) + (n2.z * p.z);
    double d3 = (n3.x * p.x) + (n3.y * p.y) + (n3.z * p.z);

    // Offset distances
    double d1_prime = d1 + offsetDistance;
    double d2_prime = d2 + offsetDistance;
    double d3_prime = d3 + offsetDistance;

    // Cross products
    tc::Vector3 c23(n2.y * n3.z - n2.z * n3.y, n2.z * n3.x - n2.x * n3.z,
                    n2.x * n3.y - n2.y * n3.x);
    tc::Vector3 c31(n3.y * n1.z - n3.z * n1.y, n3.z * n1.x - n3.x * n1.z,
                    n3.x * n1.y - n3.y * n1.x);
    tc::Vector3 c12(n1.y * n2.z - n1.z * n2.y, n1.z * n2.x - n1.x * n2.z,
                    n1.x * n2.y - n1.y * n2.x);

    // Scalar triple product (determinant)
    double determinant = (n1.x * c23.x) + (n1.y * c23.y) + (n1.z * c23.z);

    // If determinant is near 0, planes are parallel (degenerate). Return a zero-vector as a flag.
    if (std::abs(determinant) < 0.0001)
    {
        return tc::Vector3(0, 0, 0);
    }

    // Combine terms and divide by determinant
    double invDet = 1.0 / determinant;
    return tc::Vector3((c23.x * d1_prime + c31.x * d2_prime + c12.x * d3_prime) * invDet,
                       (c23.y * d1_prime + c31.y * d2_prime + c12.y * d3_prime) * invDet,
                       (c23.z * d1_prime + c31.z * d2_prime + c12.z * d3_prime) * invDet);
}
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
        //return true;
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
    DSMesh out;
    DSMesh _m_original = _m.newMakeCopy();
    _m_original.computeNormals();

    struct PendingHoleSharpEdge
{
    Vertex* a;
    Vertex* b;
    float sharpness;
};

std::vector<PendingHoleSharpEdge> pendingHoleSharpEdges;

// Avoid relying on edgeTable/findEdge for the source lookup.
// Avoid relying on edgeTable/findEdge for the source lookup.
auto getSourceEdgeSharpness =
    [&_m_original](Vertex* a, Vertex* b) -> float
{
    if (!a || !b)
        return 0.0f;

    for (Edge* edge : _m_original.edges())
    {
        if (!edge || !edge->v0() || !edge->v1())
            continue;

        const bool sameEndpoints =
            (edge->v0() == a && edge->v1() == b)
            || (edge->v0() == b && edge->v1() == a);

        if (sameEndpoints)
            return edge->sharpness;
    }

    return 0.0f;
};



    // Apply a crease to an output edge and its endpoint vertices.
    // Use this for copied source edges and through-thickness corner edges.
    auto applySharpness = [](DSMesh& mesh,
                             Vertex* a,
                             Vertex* b,
                             float sharpness)
    {
        if (!a || !b || sharpness <= 0.0f)
            return;

        Edge* edge = mesh.findEdge(a, b, false);

        if (!edge)
        {
            std::cout << "[offset] Could not find generated edge "
                      << a->name << " - " << b->name << std::endl;
            return;
        }

        edge->sharpness = std::max(edge->sharpness, sharpness);
        edge->isSharp = true;

    
    };

    // Apply a crease only to an edge. Hole edges use this so the ends of
    // partially sharp holes can still blend into neighboring smooth edges.
    auto applyEdgeSharpness = [](DSMesh& mesh,
                                 Vertex* a,
                                 Vertex* b,
                                 float sharpness)
    {
        if (!a || !b || sharpness <= 0.0f)
            return;

        Edge* edge = mesh.findEdge(a, b, false);

        if (!edge)
        {
            std::cout << "[offset] Could not find generated hole edge "
                      << a->name << " - " << b->name << std::endl;
            return;
        }

        edge->sharpness = std::max(edge->sharpness, sharpness);
        edge->isSharp = true;

        std::cout << "[offset] transferred sharpness "
                  << edge->sharpness
                  << " to hole edge "
                  << a->name << " - "
                  << b->name << std::endl;
    };

    auto applyHoleEdgeSharpness = [](DSMesh& mesh,
                                 Vertex* a,
                                 Vertex* b,
                                 float sharpness)
{
    if (!a || !b || sharpness <= 0.0f)
        return;

    Edge* edge = mesh.findEdge(a, b, false);

    if (!edge)
    {
        std::cout << "[offset] Could not find generated hole edge "
                  << a->name << " - "
                  << b->name << std::endl;
        return;
    }

    // Keep the hole perimeter edge sharp.
    edge->sharpness = std::max(edge->sharpness, sharpness);
    edge->isSharp = true;

    // Do not set a->sharpness or b->sharpness.
    // Explicitly sharp vertices caused the corner fragments.
};

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
    const bool flatOffset = std::abs(height) < 1e-8;
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
        outerVert = new Vertex(
    outerPos.x,
    outerPos.y,
    outerPos.z,
    out.vertList.size()
);

outerVert->name = v->name + "_offsetOuter";
outerVert->normal = v->normal;
outerVert->sharpness = v->sharpness;

outerVerts[v] = outerVert;
out.addVertex(outerVert);

if (flatOffset)
{
    // A flat offset has only one geometric shell.
    // Reuse the outer vertex instead of creating an unused duplicate.
    innerVert = outerVert;
    innerVerts[v] = outerVert;
}
else
{
    innerVert = new Vertex(
        innerPos.x,
        innerPos.y,
        innerPos.z,
        out.vertList.size()
    );

    innerVert->name = v->name + "_offsetInner";
    innerVert->normal = v->normal;
    innerVert->sharpness = v->sharpness;

    innerVerts[v] = innerVert;
    out.addVertex(innerVert);

    WireFrames.push_back({ outerVert, innerVert });
}
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
        //return false;
        //return f->name.find("_offsetRibbon") != std::string::npos;
        
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
                out.faceList.back()->hide = outerRimHidden; // for outer boundary ribbon
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

                tc::Vector3 H_out_pos = c_out + (O_curr->position - c_out) * width;
                tc::Vector3 H_in_pos = c_in + (I_curr->position - c_in) * width;

                Vertex* h_out =
    new Vertex(
        H_out_pos.x,
        H_out_pos.y,
        H_out_pos.z,
        out.vertList.size()
    );

h_out->name =
    f_out.name + "_holeOut_"
    + std::to_string(i) + "_"
    + std::to_string(j);

h_out->normal = O_curr->normal;

out.addVertex(h_out);
outerHoleVerts.push_back(h_out);
outerVertsHole[O_curr].push_back(h_out);

Vertex* h_in = nullptr;

if (flatOffset)
{
    // A flat offset has no separate inner hole rim.
    h_in = h_out;
}
else
{
    h_in =
        new Vertex(
            H_in_pos.x,
            H_in_pos.y,
            H_in_pos.z,
            out.vertList.size()
        );

    h_in->name =
        f_in.name + "_holeIn_"
        + std::to_string(i) + "_"
        + std::to_string(j);

    h_in->normal = I_curr->normal;

    out.addVertex(h_in);
}

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
           

            // Build the new geometry (the rings of trapezoids and the tube walls).
            for (int j = 0; j < numVerts; ++j)// 3. TUBE WALL (Straight down!)
            {
                const int nextIndex = (j + 1) % numVerts;
Vertex* sourceV0 = f_curr->vertices[j];
Vertex* sourceV1 = f_curr->vertices[nextIndex];

const float sourceSharpness =
    getSourceEdgeSharpness(sourceV0, sourceV1);

const int prevIndex =
    (j + numVerts - 1) % numVerts;

Vertex* sourcePrev =
    f_curr->vertices[prevIndex];

const float previousSharpness =
    getSourceEdgeSharpness(sourcePrev, sourceV0);

const float cornerSharpness =
    std::max(previousSharpness, sourceSharpness);

                bool isRibbon =
                    f_curr && f_curr->name.rfind("_offsetRibbon") != std::string::npos;
                bool isBoundaryRibbon =
                    false
                    && (f_curr
                        && f_curr->name.find("_offsetBoundaryRibbon")
                            != std::string::npos);

                // Because the inner array was not reversed, O_curr and I_curr
                // represent the same original corner.
                Vertex* O_curr = outVerts[j];
                Vertex* O_next = outVerts[nextIndex];
                Vertex* I_curr = inVerts[j];
                Vertex* I_next = inVerts[nextIndex];

                Vertex* H_out_curr = outerHoleVerts[j];
                Vertex* H_out_next = outerHoleVerts[nextIndex];
                Vertex* H_in_curr = innerHoleVerts[j];
                Vertex* H_in_next = innerHoleVerts[nextIndex];
                // The edges from each source corner to its generated hole corner
                // are artificial seams introduced by offset().  If the source corner
                // is sharp, leaving these seams smooth lets Catmull-Clark pull their
                // edge points toward the center of the ring, which rounds/folds the
                // surface across an otherwise sharp square hole.  Preserve the source
                // corner on these generated radial edges as well.
                if (cornerSharpness > 0.0f)
                {
                    pendingHoleSharpEdges.push_back({
                        O_curr,
                        H_out_curr,
                        cornerSharpness
                    });

                    if (!flatOffset)
                    {
                        pendingHoleSharpEdges.push_back({
                            I_curr,
                            H_in_curr,
                            cornerSharpness
                        });
                    }
                }

                if (flatOffset)
{
    out.addFace(
        { O_curr, O_next, H_out_next, H_out_curr },
        surfOut,
        ""
    );

    out.faceList.back()->name =
        f_curr->name + "_offsetOuterFace";

    WireFrames.push_back(
        { O_curr, O_next, H_out_next, H_out_curr, O_curr }
    );

   if (sourceSharpness > 0.0f)
{
    pendingHoleSharpEdges.push_back({
        H_out_curr,
        H_out_next,
        sourceSharpness
    });
}

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
                //out.addFace({ I_curr, I_next, H_in_next, H_in_curr }, surfIn, "");

                out.faceList.back()->name = f_curr->name + "_offsetInnerFace_" + std::to_string(i)
                    + "_" + std::to_string(j);
                // out.faceList.back()->name = out.faceList.back()->name + "_offsetRibbon";

                WireFrames.push_back({ I_curr, H_in_curr, H_in_next, I_next, I_curr });
                if (cornerSharpness > 0.0f)
    {
        pendingHoleSharpEdges.push_back({
            H_out_curr,
            H_in_curr,
            cornerSharpness
        });
    }
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
                    out.faceList.back()->hide = innerRimHidden; // for hole ribbon
                    WireFrames.push_back(
                        { H_out_curr, H_out_next, H_in_next, H_in_curr, H_out_curr });
                    WireFrames.push_back({ H_out_curr, H_in_curr });
                }
               if (sourceSharpness > 0.0f)
{
    // Outer hole perimeter.
    pendingHoleSharpEdges.push_back({
        H_out_curr,
        H_out_next,
        sourceSharpness
    });

    // Matching inner hole perimeter.
    pendingHoleSharpEdges.push_back({
        H_in_curr,
        H_in_next,
        sourceSharpness
    });
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

    auto findOutputEdge =
    [&out](Vertex* a, Vertex* b) -> Edge*
{
    if (!a || !b)
        return nullptr;

    for (Edge* edge : out.edges())
    {
        if (!edge || !edge->v0() || !edge->v1())
            continue;

        const bool sameEndpoints =
            (edge->v0() == a && edge->v1() == b)
            || (edge->v0() == b && edge->v1() == a);

        if (sameEndpoints)
            return edge;
    }

    return nullptr;
};

int appliedHoleSharpEdges = 0;

for (const PendingHoleSharpEdge& pending : pendingHoleSharpEdges)
{
    Edge* edge = findOutputEdge(pending.a, pending.b);

    if (!edge)
    {
        std::cout
            << "[offset] FINAL hole edge not found: "
            << pending.a->name << " - "
            << pending.b->name
            << std::endl;

        continue;
    }

    edge->sharpness =
        std::max(edge->sharpness, pending.sharpness);

    edge->isSharp = true;

    ++appliedHoleSharpEdges;

    std::cout
        << "[offset] FINAL sharp hole edge "
        << edge->v0()->name << " - "
        << edge->v1()->name
        << " sharpness = "
        << edge->sharpness
        << std::endl;
}

std::cout
    << "[offset] applied sharpness to "
    << appliedHoleSharpEdges
    << " generated hole edge(s)"
    << std::endl;
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
// All faces have already been added, so out.edgeList now contains
// the real edges created by out.addFace(...).
// Transfer source-edge sharpness to the corresponding offset geometry.
for (Edge* sourceEdge : _m_original.edges())
{
    if (!sourceEdge ||
        !sourceEdge->v0() ||
        !sourceEdge->v1() ||
        sourceEdge->sharpness <= 0.0f)
    {
        continue;
    }

    Vertex* sourceV0 = sourceEdge->v0();
    Vertex* sourceV1 = sourceEdge->v1();
    const float sharpness = sourceEdge->sharpness;

    auto outer0It = outerVerts.find(sourceV0);
    auto outer1It = outerVerts.find(sourceV1);
    auto inner0It = innerVerts.find(sourceV0);
    auto inner1It = innerVerts.find(sourceV1);

    Vertex* outerV0 =
        outer0It != outerVerts.end() ? outer0It->second : nullptr;
    Vertex* outerV1 =
        outer1It != outerVerts.end() ? outer1It->second : nullptr;
    Vertex* innerV0 =
        inner0It != innerVerts.end() ? inner0It->second : nullptr;
    Vertex* innerV1 =
        inner1It != innerVerts.end() ? inner1It->second : nullptr;

    // The two copies of the original sharp edge.
    applySharpness(out, outerV0, outerV1, sharpness);
    applySharpness(out, innerV0, innerV1, sharpness);

    // A boundary edge generates a side ribbon. Preserve the sharp
    // corners at both ends of that ribbon through the thickness.
    const bool isBoundaryEdge =
        sourceEdge->fa == nullptr || sourceEdge->fb == nullptr;

    if (isBoundaryEdge && std::abs(height) > 1e-8)
    {
        applySharpness(out, outerV0, innerV0, sharpness);
        applySharpness(out, outerV1, innerV1, sharpness);
    }
}
int sharpEdgeCount = 0;

for (Edge* edge : out.edges())
{
    if (edge && edge->sharpness > 0.0f)
    {
        ++sharpEdgeCount;

        std::cout << "[offset] sharp output edge "
                  << edge->v0()->name << " - "
                  << edge->v1()->name
                  << " sharpness = "
                  << edge->sharpness
                  << std::endl;
    }
}

std::cout << "[offset] total sharp output edges = "
          << sharpEdgeCount << std::endl;
out.buildBoundary();
out.computeNormals();

_m = out;
return true;
}

std::pair<Vertex*, float> FindClosestVert(const tc::Vector3& pos, std::vector<Vertex*> list)
{
    Vertex* result = NULL;
    float minDist = std::numeric_limits<float>::max();
    // TODO: linear search for the time being
    for (const auto& v : list) // Project AddOffset
    {
        Vector3 pp = v->position;
        float dist = pos.DistanceToPoint(pp);
        cout << "";
        if (dist < minDist)
        {
            minDist = dist;
            result = v;
        }
    }
    if (result == NULL)
    {
        return { NULL, -1 };
    }
    return { result, minDist };
}

void CMeshMerger::MergeClear()
{
    currMesh.clear();
    MergedMesh.clear();
}

// Copy the feature tags that OpenSubdiv itself produced on the refined level
// back into the rebuilt DSMesh.
//
// This is intentionally simpler than reconstructing parent/child ancestry ourselves:
// OpenSubdiv already knows exactly which child edges/vertices remain sharp after
// Catmull-Clark refinement.  Reading GetEdgeSharpness()/GetVertexSharpness() from
// the final level prevents us from inventing extra sharp edges and makes the next
// one-level Catmull() pass receive the exact refined crease data.
static void ApplyOpenSubdivSharpnessToRefinedMesh(
    DSMesh& mesh,
    Far::TopologyLevel const& refinedLevel)
{
    // Start with a clean feature state.
    for (Edge* edge : mesh.edges())
    {
        if (!edge)
            continue;

        edge->sharpness = 0.0f;
        edge->isSharp = false;
    }

    for (Vertex* vertex : mesh.vertList)
    {
        if (vertex)
            vertex->sharpness = 0.0f;
    }

    int sharpEdgeCount = 0;
    int sharpVertexCount = 0;

    // The rebuilt DSMesh vertices are created in the same order as the vertices
    // in refinedLevel, so topology vertex index == mesh.vertList index.
    const int topologyEdgeCount = refinedLevel.GetNumEdges();

    for (int edgeIndex = 0; edgeIndex < topologyEdgeCount; ++edgeIndex)
    {
        const float sharpness = refinedLevel.GetEdgeSharpness(edgeIndex);

        if (sharpness <= 0.0f)
            continue;

        Far::ConstIndexArray edgeVertices =
            refinedLevel.GetEdgeVertices(edgeIndex);

        if (edgeVertices.size() != 2)
            continue;

        const int v0Index = edgeVertices[0];
        const int v1Index = edgeVertices[1];

        if (v0Index < 0 || v1Index < 0
            || v0Index >= static_cast<int>(mesh.vertList.size())
            || v1Index >= static_cast<int>(mesh.vertList.size()))
        {
            std::cout << "[subdivide] refined sharp edge "
                      << edgeIndex
                      << " has invalid endpoint indices "
                      << v0Index << ", " << v1Index
                      << std::endl;
            continue;
        }

        Vertex* v0 = mesh.vertList[v0Index];
        Vertex* v1 = mesh.vertList[v1Index];

        if (!v0 || !v1)
            continue;

        Edge* meshEdge = nullptr;

        // Avoid TopologyLevel::FindEdge() and DSMesh::findEdge() here.  The former
        // caused the earlier final-level adjacency assertion, and the latter depends
        // on edge-table state.  Pointer endpoint matching is unambiguous here.
        for (Edge* candidate : mesh.edges())
        {
            if (!candidate || !candidate->v0() || !candidate->v1())
                continue;

            const bool sameEndpoints =
                (candidate->v0() == v0 && candidate->v1() == v1)
                || (candidate->v0() == v1 && candidate->v1() == v0);

            if (sameEndpoints)
            {
                meshEdge = candidate;
                break;
            }
        }

        if (!meshEdge)
        {
            std::cout << "[subdivide] could not map OpenSubdiv sharp edge "
                      << edgeIndex << " to DSMesh edge "
                      << v0Index << " - " << v1Index
                      << std::endl;
            continue;
        }

        meshEdge->sharpness = sharpness;
        meshEdge->isSharp = true;
        ++sharpEdgeCount;

        std::cout << "[subdivide] OpenSubdiv child crease "
                  << meshEdge->v0()->name << " - "
                  << meshEdge->v1()->name
                  << " sharpness = " << sharpness
                  << std::endl;
    }

    const int topologyVertexCount = refinedLevel.GetNumVertices();

    for (int vertexIndex = 0; vertexIndex < topologyVertexCount; ++vertexIndex)
    {
        const float sharpness =
            refinedLevel.GetVertexSharpness(vertexIndex);

        if (sharpness <= 0.0f)
            continue;

        if (vertexIndex >= static_cast<int>(mesh.vertList.size()))
            continue;

        Vertex* vertex = mesh.vertList[vertexIndex];

        if (!vertex)
            continue;

        vertex->sharpness = sharpness;
        ++sharpVertexCount;

        std::cout << "[subdivide] OpenSubdiv child corner "
                  << vertex->name
                  << " sharpness = " << sharpness
                  << std::endl;

    }

    std::cout << "[subdivide] copied "
              << sharpEdgeCount
              << " refined crease edge(s) and "
              << sharpVertexCount
              << " refined sharp vertex/vertices back to DSMesh"
              << std::endl;
}

bool CMeshMerger::subdivide(DSMesh& _m, unsigned int n)
{
    DSMesh myCopy = _m.newMakeCopy();
    std::vector<Face*> faceList = myCopy.faceList;

    // Instantiate a Far::TopologyRefiner from the descriptor.
    // GetRefiner reads the sharpness currently stored on _m's edges.
    Far::TopologyRefiner* refiner = GetRefiner(_m, isSharp);

    Far::TopologyRefiner::UniformOptions uniop(n);

    // We inspect edge topology on the final refined level in order to restore
    // sharpness to descendant edges. OpenSubdiv otherwise keeps only minimal
    // topology on the last level (face->vertex data), which leaves the final
    // edge->vertex tables empty and makes GetEdgeVertices()/FindEdge() unsafe.
    uniop.fullTopologyInLastLevel = true;

    // uniop.orderVerticesFromFacesFirst = true;
    refiner->RefineUniform(uniop);

    std::vector<Vertex> vbuffer(refiner->GetNumVerticesTotal());
    Vertex* verts = &vbuffer[0];

    for (int i = 0; i < (int)_m.vertList.size(); ++i)
    {
        auto* v = _m.vertList[i];
        verts[i].SetPosition(v->position.x, v->position.y, v->position.z);
    }

    // Interpolate vertex primvar data.
    Far::PrimvarRefiner primvarRefiner(*refiner);

    Vertex* src = verts;
    for (int level = 1; level <= static_cast<int>(n); ++level)
    {
        Vertex* dst = src + refiner->GetLevel(level - 1).GetNumVertices();
        primvarRefiner.Interpolate(level, src, dst);
        src = dst;
    }

    const int originalVertexCount =
        static_cast<int>(_m.vertList.size());

    // DEBUG: preserve the input vertex names and positions so we can verify
    // that OpenSubdiv's CORNER rule is actually keeping those vertex points
    // fixed during primvar interpolation.  Catmull() calls subdivide(..., 1),
    // so vertex i on the refined level is the child vertex point of input i.
    std::vector<std::string> originalVertexNames;
    std::vector<tc::Vector3> originalVertexPositions;
    originalVertexNames.reserve(originalVertexCount);
    originalVertexPositions.reserve(originalVertexCount);

    for (int i = 0; i < originalVertexCount; ++i)
    {
        Vertex* inputVertex = _m.vertList[i];

        if (inputVertex)
        {
            originalVertexNames.push_back(inputVertex->name);
            originalVertexPositions.push_back(inputVertex->position);
        }
        else
        {
            originalVertexNames.push_back("<null>");
            originalVertexPositions.push_back(tc::Vector3(0, 0, 0));
        }
    }

    // WireFrames stores raw Vertex* pointers into the current DSMesh.
    // The mesh is about to be deleted and rebuilt, so keeping the old
    // entries would leave dangling pointers and draw stale/floating lines.
    // Each subdivision pass rebuilds WireFrames from the newly created faces.
    WireFrames.clear();

    _m.clear();
    _m.clearAndDelete();
    _m.updateVertListAfterDeletion();
    _m.faceList.clear();
    _m.vertList.clear();
    _m.edgeList.clear();
    _m.boundaryEdgeList().clear();

    { // Output the highest refined level back into DSMesh.
        Far::TopologyLevel const& refLastLevel = refiner->GetLevel(n);

        // DEBUG: inspect OpenSubdiv's actual vertex rule for the vertices
        // inherited from the input level. Do not filter on explicit vertex
        // sharpness: a vertex can be classified as CORNER purely from the
        // incident crease topology even when GetVertexSharpness() is zero.
        const int debugVertexCount =
            std::min(
                refLastLevel.GetNumVertices(),
                originalVertexCount
            );

        int nverts = refLastLevel.GetNumVertices();
        int nfaces = refLastLevel.GetNumFaces();

        int firstOfLastVerts = refiner->GetNumVerticesTotal() - nverts;

        for (int vertexIndex = 0;
             vertexIndex < debugVertexCount;
             ++vertexIndex)
        {
            const float sharpness =
                refLastLevel.GetVertexSharpness(vertexIndex);

            const Sdc::Crease::Rule rule =
                refLastLevel.GetVertexRule(vertexIndex);

            float const* refinedPos =
                verts[vertexIndex + firstOfLastVerts].GetPosition();

            const tc::Vector3& inputPos =
                originalVertexPositions[vertexIndex];

            const double dx = refinedPos[0] - inputPos.x;
            const double dy = refinedPos[1] - inputPos.y;
            const double dz = refinedPos[2] - inputPos.z;
            const double delta = std::sqrt(dx * dx + dy * dy + dz * dz);

            std::cout
                << "[OpenSubdiv] position-check "
                << originalVertexNames[vertexIndex]
                << " index = " << vertexIndex
                << " rule = " << OpenSubdivRuleName(rule)
                << " vertexSharpness = " << sharpness
                << " input = ("
                << inputPos.x << ", "
                << inputPos.y << ", "
                << inputPos.z << ")"
                << " refined = ("
                << refinedPos[0] << ", "
                << refinedPos[1] << ", "
                << refinedPos[2] << ")"
                << " delta = " << delta
                << std::endl;
        }

        for (int vert = 0; vert < nverts; ++vert)
        {
            float const* pos = verts[vert + firstOfLastVerts].GetPosition();
            _m.addVertex(pos[0], pos[1], pos[2]);
        }

        for (int face = 0; face < nfaces; ++face)
        {
            Far::ConstIndexArray fverts = refLastLevel.GetFaceVertices(face);

            int idx = face;
            for (int l = static_cast<int>(n); l > 0; --l)
            {
                idx = refiner->GetLevel(l).GetFaceParentFace(idx);
            }

            // All refined Catmark faces should be quads.
            assert(fverts.size() == 4);

            std::vector<Vertex*> vertices;
            for (int i = 0; i < 4; ++i)
            {
                vertices.push_back(_m.vertList.at(fverts[i]));
            }

            int index = idx;
            if (index >= static_cast<int>(faceList.size()))
            {
                std::cout << "exceeded: " << index << "\n";
                index = static_cast<int>(faceList.size()) - 1;
            }

            std::string surfaceName = faceList.at(index)->surfaceName;
            std::string backfaceName = faceList.at(index)->backfaceName;

            if (surfaceName.empty())
            {
                surfaceName = "";
            }
            if (backfaceName.empty()
                || (backfaceName.substr(0, 10)).compare("SubdivVert") == 0)
            {
                backfaceName = "";
            }

            _m.addFace(vertices, surfaceName, backfaceName);
            WireFrames.push_back(vertices);
        }

        // addFace() has now rebuilt the real DSMesh edges. Copy the crease/corner
        // state that OpenSubdiv actually produced on this refined level. This is the
        // state that must feed the next recursive one-level subdivision pass.
        if (isSharp)
        {
            ApplyOpenSubdivSharpnessToRefinedMesh(
                _m,
                refLastLevel
            );
        }

        _m.computeNormals();
        _m.buildBoundary();

        for (int i = 0; i < (int)_m.vertList.size(); ++i)
        {
            _m.vertList[i]->ID = i;
        }
        for (int i = 0; i < (int)_m.faceList.size(); ++i)
        {
            _m.faceList[i]->id = i;
        }

        int maxID = -1;
        for (auto* v : _m.vertList)
        {
            maxID = std::max(maxID, static_cast<int>(v->ID));
        }

        std::cout << "verts=" << _m.vertList.size() << " maxID=" << maxID << "\n";
        std::cout << "vertList=" << _m.vertList.size()
                  << " faceList=" << _m.faceList.size()
                  << " edgeList=" << _m.edgeList.size()
                  << " nameToVert=" << _m.nameToVert.size()
                  << " nameToFace=" << _m.nameToFace.size()
                  << "\n";
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