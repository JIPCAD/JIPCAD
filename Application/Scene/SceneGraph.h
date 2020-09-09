#pragma once
#include "Surface.h"
#include "Transforms.h"
#include <Parsing/ASTContext.h>
#include <SignalSlot.h>
#include <map>
#include <set>
#include <string>
#include <utility>

namespace Nome::Scene
{

using tc::FSignal;
using tc::TAutoPtr;

// Forward declaration
class CSceneNode;
class CEntity;
class CScene;

class CSceneTreeNode : public Flow::CFlowNode
{
    // The matrix that takes you from local coord to world
    DEFINE_OUTPUT_WITH_UPDATE(Matrix3x4, L2WTransform);

public:
    // Public APIs
    CSceneNode* GetOwner() const { return Owner; }
    bool IsValid() const { return Owner; }
    CEntity* GetInstanceEntity() const { return InstanceEntity; }
    CEntity* GetEntity() const;
    /// Returns whether the associated entity was changed or updated in the last frame
    bool WasEntityUpdated() const { return bEntityUpdated; }
    void SetEntityUpdated(bool value) { bEntityUpdated = value; }

    // Note: linear time is prob too slow
    CSceneTreeNode* FindChildOfOwner(CSceneNode* owner) const;
    CSceneTreeNode* FindChild(const std::string& name) const;

    const std::set<CSceneTreeNode*>& GetChildren() const { return Children; }
    std::string GetPath() const;

    FSignal<void()> OnTransformChange;

private:
    // Only CSceneNode manages the tree nodes
    friend class CSceneNode;

    explicit CSceneTreeNode(CSceneNode* owner);

    static CSceneTreeNode* CreateTree(CSceneNode* dagNode);
    void RemoveTree();
    void MarkTreeL2WDirty();

    // Private fields, accessible to CSceneNode though
    CSceneNode* Owner;
    CSceneTreeNode* Parent = nullptr;
    std::set<CSceneTreeNode*> Children;

    // This is non-null if the entity is instantiable
    TAutoPtr<CEntity> InstanceEntity;
    bool bEntityUpdated = false;
};

// A scene node, which represents either a group or an instance call in Nom file
class CSceneNode : public Flow::CFlowNode
{
    DEFINE_INPUT(Matrix3x4, Transform);

public:
    explicit CSceneNode(CScene* owningScene, std::string name, bool isRoot = false,
                        bool isGroup = false);
    ~CSceneNode() override;

    const std::string& GetName() const { return Name; }
    bool SetName(std::string newName);
    bool IsGroup() const { return bIsGroup; }

    // Hierarchy management
    void AddParent(CSceneNode* newParent);
    void RemoveParent(CSceneNode* parent);
    CSceneNode* CreateChildNode(const std::string& name);
    CSceneNode* FindChildNode(const std::string& name);
    CSceneNode* FindOrCreateChildNode(const std::string& name);

    // Returns the number of associated tree nodes, i.e. the number of ways from the root to this
    // graph node
    size_t CountTreeNodes() const;
    const std::set<TAutoPtr<CSceneTreeNode>>& GetTreeNodes() const;

    // Instance/Entity related
    CEntity* GetEntity() const;
    void SetEntity(CEntity* ent);

    CScene* GetScene() const { return Scene; }

    // A signal/input that manages the possibly unassigned surface (color)
    // TODO: generalize this and tie this to the AST
    void SetSurface(const TAutoPtr<CSurface>& surface) { Surface = surface; }
    TAutoPtr<CSurface> GetSurface() const { return Surface; }
    void NotifySurfaceDirty() const;

    void SyncFromAST(AST::ACommand* cmd, CScene& scene);
    AST::ACommand* BuildASTCommand(Nome::AST::CASTContext& ctx) const;
    void SyncToAST(AST::CASTContext& ctx);

private:
    std::string Name;
    /// Denotes whether this node is a group. Group names can be skipped in a path
    bool bIsGroup = false;
    CScene* Scene;

    friend class CSceneTreeNode;
    // Parents and associated tree nodes organized by parent
    std::set<CSceneNode*> Parents;
    std::set<TAutoPtr<CSceneNode>> Children;
    std::set<TAutoPtr<CSceneTreeNode>> TreeNodes;

    // Associated entity, aka the generator instantiated
    TAutoPtr<CEntity> Entity;
    TAutoPtr<CSurface> Surface;

    AST::ACommand* ASTSource {};
};

}
