#pragma once
#include "BankAndSet.h"
#include "Entity.h"
#include "Point.h"
#include "SceneGraph.h"
#include <Color.h>
#include <queue>
#include <utility>

namespace Nome
{
class CPickingManager;
}

namespace Nome::Scene
{

class CScene : public tc::FRefCounted
{
public:
    CScene();

    CBankAndSet& GetBankAndSet() { return BankAndSet; }

    TAutoPtr<CSceneNode> GetRootNode() const { return RootNode; }
    TAutoPtr<CSceneTreeNode> GetRootTreeNode() const;

    // Adds an entity into the lookup map
    void AddEntity(TAutoPtr<CEntity> entity);
    void RemoveEntity(const std::string& name, bool bAlsoRemoveChildren = false);
    bool RenameEntity(const std::string& oldName, const std::string& newName);

    // Finds an entity by its name
    TAutoPtr<CEntity> FindEntity(const std::string& name) const;

    // Creates a group that is represented by a scene node with the specified name
    TAutoPtr<CSceneNode> CreateGroup(const std::string& name);
    // Finds a group by its name
    TAutoPtr<CSceneNode> FindGroup(const std::string& name) const;

    // Locate in the scene a point output (could be a point or a mesh vertex) by its path
    Flow::TOutput<CVertexInfo*>* FindPointOutput(const std::string& id) const;

    /// Walks the scene tree along a path, return the last matching node and the rest of path
    std::pair<CSceneTreeNode*, std::string> WalkPath(const std::string& path) const;

    void Update();

    template <typename TFunc> void ForEachSceneTreeNode(const TFunc& func) const
    {
        std::queue<CSceneTreeNode*> q;
        q.push(GetRootTreeNode());
        while (!q.empty())
        {
            func(q.front());
            const auto& childNodes = q.front()->GetChildren();
            for (CSceneTreeNode* child : childNodes)
                q.push(child);
            q.pop();
        }
    }

    // Return the selected scene tree nodes in order, if none, returns the whole scene
    std::vector<CSceneTreeNode*> GetSelectedNodes() const;

    CPickingManager* GetPickingMgr() const;

private:
    CBankAndSet BankAndSet;

    // This is the root node of the scene tree
    TAutoPtr<CSceneNode> RootNode;

    TAutoPtr<CPickingManager> PickingMgr;

    // Every generator or group in the nom file is declared with a name
    // The following two maps enable looking up objects by their names
    std::map<std::string, TAutoPtr<CEntity>> EntityLibrary;
    std::map<std::string, TAutoPtr<CSceneNode>> Groups;
};

}
