#pragma once
#include "Face.h"
#include "Sharp.h"
#include "InteractivePoint.h"

#include <Plane.h> // Randy added on 10/10 for pick face
#include <Ray.h>
// We use OpenMesh for now. Can easily replace with in-house library when needed.
#define _USE_MATH_DEFINES
#undef min
#undef max
#include <OpenMesh/Core/Mesh/PolyMesh_ArrayKernelT.hh>
#include <OpenMesh/Core/Mesh/Traits.hh>

#include <map>
#include <set>
#include <utility>

// Randy note, we need to change this to winged edge asap. Project AddOfset
struct CNomeTraits : public OpenMesh::DefaultTraits
{
    // store barycenter of neighbors in this member
    VertexTraits
    {
    private:
        float sharpness_;

    public:
        VertexT()
            : sharpness_(0.0f)
        {
        }
        [[nodiscard]] const float& sharpness() const { return sharpness_; }
        void set_sharpness(const float& _s) { sharpness_ = _s; }
    };
    EdgeTraits
    {
    private:
        float sharpness_;

    public:
        EdgeT()
            : sharpness_(0.0f)
        {
        }
        [[nodiscard]] const float& sharpness() const { return sharpness_; }
        void set_sharpness(const float& _s) { sharpness_ = _s; }
    };
};

#include "./CustomMeshDataStructure/DataStructureMesh.h" // Project SwitchDS


typedef OpenMesh::PolyMesh_ArrayKernelT<> CMeshImpl;
typedef Mesh DSMesh; // Project SwitchDS


namespace Nome::Scene
{

class CMeshInstance;
class CVertexSelector;

// Common base class for all mesh objects
class CMesh : public CEntity
{
    DEFINE_INPUT_ARRAY(CFace*, Faces) { MarkDirty(); }

    DEFINE_INPUT_ARRAY(CSharp*, SharpPoints) { MarkDirty(); }

    DEFINE_INPUT_ARRAY(CVertexInfo*, Points)
    {
        MarkDirty();
    } // Randy added this on 12/5. CVertexInfo completely specifies a vertex.


public:
    DECLARE_META_CLASS(CMesh, CEntity);
    CMesh();
    explicit CMesh(std::string name);

    void MarkDirty() override;
    void UpdateEntity() override;
    void Draw(IDebugDraw* draw) override;

    Vertex* AddVertex(const std::string& name, Vector3 pos, float sharpness = 0.0f);

    bool HasVertex(const std::string& name) const
    {
        if (currMesh.nameToVert.count(name) != 0)
            return true;
        else
            return false;   
    }


    Vector3 GetVertexPos(const std::string& name) const;

    Vertex* FindVertex(const std::string& name) const
    {
        return currMesh.nameToVert.find(name)->second;
    }

    static void AddPointSharpness(Vertex* p, float sharpness);
    void AddEdgeSharpness(Vertex* e1, Vertex* e2, float sharpness);
    void AddFace(const std::string& name, const std::vector<std::string>& facePointNames, std::string faceSurfaceIdent = "", std::string faceBackfaceIdent = ""); // Randy added faceSurfaceIdent on 12/12
    void AddFace(const std::string& name, const std::vector<Vertex*>& faceDSVerts, std::string faceSurfaceIdent = "", std::string faceBackfaceIdent = ""); // Randy added faceSurfaceIdent on 12/12
    void AddLineStrip(const std::string& name, const std::vector<Vertex*>& points);
    void ClearMesh();

    void SetFromData(CMeshImpl mesh, std::map<std::string, Vertex*> vnames,
                     std::map<std::string, Face* > fnames);

    bool IsInstantiable() override;
    CEntity* Instantiate(CSceneTreeNode* treeNode) override;
    AST::ACommand* SyncToAST(AST::CASTContext& ctx, bool createNewNode) override;

private:
    friend class CMeshInstance;
    friend class CMeshMerger;
    std::set<CMeshInstance*> InstanceSet;

    CMeshImpl Mesh; // Deprecated. Remove soon
    DSMesh currMesh; // Project SwitchDS

    // Randy removed all the NameToVert, VertToName, etc. data structures on 1/31/2021

    std::vector<std::vector<Vertex*>> LineStrips; // Randy changed this to LineStrips on 12/25
    std::map<Face* , std::string> DSFaceWithColor; // Randy added on 12/12 for face entity coloring
};

class CMeshInstancePoint : public CInteractivePoint
{
public:
    explicit CMeshInstancePoint(CMeshInstance* owner)
        : Owner(owner)
    {
    }

    [[nodiscard]] std::string GetPointPath() const;

private:
    CMeshInstance* Owner;
};

// This is the entity class for a mesh instance
// Why is this needed?
//  The same mesh might get instanciated multiple times and get separately edited by the user.
class CMeshInstance : public CEntity
{
    // This connects to all the vertex selectors naming a vertex from this mesh instance
    DEFINE_OUTPUT_WITH_UPDATE(CMeshInstance*, SelectorSignal)
    {
        UpdateEntity();
        SelectorSignal.UpdateValue(this);
    }

    friend class CVertexSelector;

public:
    using Super = CEntity;

    CMeshInstance(CMesh* generator, CSceneTreeNode* stn);
    ~CMeshInstance() override;

    // Called when the mesh entity is updated
    void MarkDirty() override;
    // Nothing changed about this mesh except its transformations and such
    void MarkOnlyDownstreamDirty();
    // Copy the actual mesh from the mesh entity and notify selectors
    void UpdateEntity() override;

    std::set<std::string>& GetFacesToDelete() { return FacesToDelete; }
    const std::set<std::string>& GetFacesToDelete() const { return FacesToDelete; }

    void Draw(IDebugDraw* draw) override;

    // Create a vertex selector with a vertex name, and a name for the resulting vertex
    CVertexSelector* CreateVertexSelector(const std::string& name, const std::string& outputName);

    // Get the scene tree node associated with this mesh instance
    CSceneTreeNode* GetSceneTreeNode() const { return SceneTreeNode; }

    void CopyFromGenerator();

    std::vector<std::string> RemoveFace(const std::vector<std::string>& faceNames);

    // I am really not sure whether this is a good interface or not
    const CMeshImpl& GetMeshImpl() const { return Mesh; }

    // Project SwitchDS
    DSMesh& GetDSMesh() { return currMesh; }
   


    std::vector<std::pair<float, std::string>> PickVertices(const tc::Ray& localRay);
    std::vector<std::pair<float, std::string>> PickFaces(const tc::Ray& localRay); // Randy added on 10/10 for face selection
    std::vector<std::pair<float, Vector3>> GetHitPoint(const tc::Ray& localRay); // Randy added on 3/10 for intersection point
    std::vector<std::pair<float, std::vector<std::string>>> PickPolylines(const tc::Ray& localRay); // Randy added on 12/22 for polyline selection
    std::vector<std::pair<float, std::vector<std::string>>>
    PickEdges(const tc::Ray& localRay); // Randy added on 10/29 for edge selection
    void MarkVertAsSelected(const std::set<std::string>& vertNames, float sharpness = -1);
    void MarkFaceAsSelected(const std::set<std::string>& faceNames,
                            bool bSel, float sharpness = -1); // Randy added on 10/10 for face selection
    void MarkEdgeAsSelected(const std::set<std::string>& vertNames,
                            bool bSel, float sharpness = -1); // Randy added on 10/29 for edge selection. TODO: rn it
                                        // takes in the edge's verts as input

    std::vector<Face* >
    GetSelectedFaceHandles(); // Get selected face handles. Used in InteractiveMesh.cpp.

    std::map<Face* , std::array<float,3> > GetDSFaceWithColorVector() { return DSFaceWithColorVector;}; // Randy added on 12/12 

    std::vector<std::string> GetFaceVertexNames(
        std::vector<std::string> faceNames); // Randy added on 10/19 to return face vert names

    void DeselectAll();

private:
    // Instance specific data
    TAutoPtr<CMesh> MeshGenerator;
    /// A weak pointer to the owning scene tree node
    CSceneTreeNode* SceneTreeNode;

    unsigned int TransformChangeConnection;

    CMeshImpl Mesh;
    DSMesh currMesh; // Project SwitchDS. changed to pointer on 1/29


    std::map<Face* , std::array<float, 3>> DSFaceWithColorVector; // Randy added on 12/12 for face entity coloring
    std::set<std::string> FacesToDelete;

    // std::map<std::string, std::pair<CMeshInstancePoint*, uint32_t>> PickingVerts; Randy commented
    // this out on 10/10 . I dont think it does anything???

    // Store selected vertex handles and names
    std::vector<std::string> CurrSelectedVertNames;
    std::vector<std::string> CurrSelectedVertNamesWithPrefix;
    std::vector<Vertex*> CurrSelectedDSVerts; // Randy added on 11/4

    // Store selected edge handles TODO: Create edge names
    //std::vector<CMeshImpl::HalfedgeHandle> CurrSelectedHalfEdgeHandles; // Randy added on 11/4. TODO: Should I use HalfedgeHandle or
                                     // EdgeHandle? Store selected vertex handles and names
    std::vector<std::string> CurrSelectedEdgeVertNames;
    std::vector<std::string> CurrSelectedEdgeVertNamesWithPrefix;
    std::vector<Vertex*> CurrSelectedEdgeVertHandles; // Randy added on 11/4
    // Store selected face handles and names
    std::vector<std::string> CurrSelectedfaceNames; // face name only
    std::vector<std::string> CurrSelectedfaceNamesWithPrefix; // includes prefix (path) and face name
    std::vector<Face* > CurrSelectedDSFace; // Randy added on 10/10 for face selection
};

class CVertexSelector : public Flow::CFlowNode
{
    DEFINE_INPUT(CMeshInstance*, MeshInstance) { Point.MarkDirty(); }

    DEFINE_OUTPUT_WITH_UPDATE(CVertexInfo*, Point);

public:
    CVertexSelector(std::string targetName, const std::string& resultName)
        : TargetName(std::move(targetName))
    {
        VI.Name = resultName;
    }

    std::string GetPath() const;

private:
    std::string TargetName;

    CVertexInfo VI;
};

}