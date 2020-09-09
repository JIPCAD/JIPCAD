#include "Scene.h"
#include "InteractivePoint.h"
#include "Mesh.h"
#include <StringUtils.h>

namespace Nome::Scene
{

CScene::CScene()
{
    RootNode = new CSceneNode(this, "root", true);
    PickingMgr = new CPickingManager(this);
}

TAutoPtr<CSceneTreeNode> CScene::GetRootTreeNode() const
{
    const auto& rootTreeNodes = RootNode->GetTreeNodes();
    assert(rootTreeNodes.size() == 1);
    return *rootTreeNodes.begin();
}

void CScene::AddEntity(TAutoPtr<CEntity> entity)
{
    auto iter = EntityLibrary.find(entity->GetName());
    if (iter != EntityLibrary.end())
        EntityLibrary.erase(iter);
    EntityLibrary.insert(std::make_pair(entity->GetName(), std::move(entity)));
}

void CScene::RemoveEntity(const std::string& name, bool bAlsoRemoveChildren)
{
    auto iter = EntityLibrary.find(name);
    if (iter != EntityLibrary.end())
    {
        if (bAlsoRemoveChildren)
            while (tc::FStringUtils::StartsWith(iter->first, name))
                EntityLibrary.erase(iter++);
        else
            EntityLibrary.erase(iter);
    }
}

bool CScene::RenameEntity(const std::string& oldName, const std::string& newName)
{
    // New name already exists
    if (EntityLibrary.find(newName) != EntityLibrary.end())
        return false;

    // If entity with oldName not found
    auto iter = EntityLibrary.find(oldName);
    if (iter == EntityLibrary.end())
        return false;

    while (tc::FStringUtils::StartsWith(iter->first, oldName))
    {
        auto nh = EntityLibrary.extract(iter++);
        nh.key() = newName + nh.key().substr(oldName.length());
        nh.mapped()->SetName(nh.key());
        EntityLibrary.insert(std::move(nh));
    }
    return true;
}

TAutoPtr<CEntity> CScene::FindEntity(const std::string& name) const
{
    auto iter = EntityLibrary.find(name);
    if (iter != EntityLibrary.end())
        return iter->second;
    return nullptr;
}

TAutoPtr<CSceneNode> CScene::CreateGroup(const std::string& name)
{
    if (Groups.find(name) != Groups.end())
        return {};

    auto* node = new CSceneNode(this, name, false, true);
    Groups[name] = node;
    return node;
}

TAutoPtr<CSceneNode> CScene::FindGroup(const std::string& name) const
{
    auto iter = Groups.find(name);
    if (iter != Groups.end())
        return iter->second;
    return nullptr;
}

Flow::TOutput<CVertexInfo*>* CScene::FindPointOutput(const std::string& id) const
{
    auto iter = EntityLibrary.find(id);
    if (iter != EntityLibrary.end())
    {
        TAutoPtr<CEntity> ent = iter->second;
        TAutoPtr<CPoint> point = dynamic_cast<CPoint*>(ent.Get());
        if (point)
        {
            return &point->Point;
        }
    }

    size_t charsToIgnore = 0;
    if (id[0] == '.')
        charsToIgnore = 1;
    // Descend down the scene tree starting from the root
    CSceneTreeNode* currNode = *RootNode->GetTreeNodes().begin();
    while (true)
    {
        CSceneTreeNode* nextNode = nullptr;
        size_t nextDot = id.find('.', charsToIgnore);
        if (nextDot != std::string::npos)
        {
            std::string nextSeg = id.substr(charsToIgnore, nextDot - charsToIgnore);
            nextNode = currNode->FindChild(nextSeg);
        }

        if (!nextNode)
        {
            if (auto* meshInstance = dynamic_cast<CMeshInstance*>(currNode->GetInstanceEntity()))
            {
                std::string idTurnedVertName = id;
                std::replace(idTurnedVertName.begin(), idTurnedVertName.end(), '.', '_');
                auto* point =
                    meshInstance->CreateVertexSelector(id.substr(charsToIgnore), idTurnedVertName);
                if (point)
                    return &point->Point;
                else
                    return nullptr;
            }
            else
                return nullptr;
        }
        currNode = nextNode;
        // If the current node has only 1 child, it might be a group instance
        if (currNode->GetChildren().size() == 1)
        {
            CSceneTreeNode* onlyChild = *currNode->GetChildren().begin();
            if (onlyChild->GetOwner()->IsGroup())
                currNode = onlyChild;
        }
        charsToIgnore = nextDot + 1;
    }
    return nullptr;
}

std::pair<CSceneTreeNode*, std::string> CScene::WalkPath(const std::string& path) const
{
    auto pathComps = tc::FStringUtils::Split(path, ".");
    auto iter = pathComps.begin();
    if (iter->empty())
        ++iter;
    CSceneTreeNode* currNode = *RootNode->GetTreeNodes().begin();
    for (; iter != pathComps.end(); ++iter)
    {
        // If the current node has only 1 child, it might be a group instance
        if (currNode->GetChildren().size() == 1)
        {
            CSceneTreeNode* onlyChild = *currNode->GetChildren().begin();
            if (onlyChild->GetOwner()->IsGroup())
                currNode = onlyChild;
        }

        CSceneTreeNode* childNode = currNode->FindChild(*iter);
        if (!childNode)
            break;
        currNode = childNode;
    }
    return { currNode, tc::FStringUtils::Combine(iter, pathComps.end(), ".") };
}

void DFSTreeNodeUpdate(CSceneTreeNode* treeNode)
{
    treeNode->L2WTransform.Update();

    const auto& childNodes = treeNode->GetChildren();
    for (CSceneTreeNode* child : childNodes)
        DFSTreeNodeUpdate(child);

    if (auto* ent = treeNode->GetEntity())
    {
        // Update the instance entity
        if (ent->IsDirty())
            treeNode->SetEntityUpdated(true);
        ent->UpdateEntity();
    }
}

void CScene::Update()
{
    // Called every frame to make sure everything is up to date
    DFSTreeNodeUpdate(GetRootTreeNode());
}

std::vector<CSceneTreeNode*> CScene::GetSelectedNodes() const
{
    // TODO: selection is not currently implemented, so returns the whole scene
    std::vector<CSceneTreeNode*> r;
    ForEachSceneTreeNode([&r](CSceneTreeNode* node) { r.push_back(node); });
    return r;
}

CPickingManager* CScene::GetPickingMgr() const { return PickingMgr; }

}
