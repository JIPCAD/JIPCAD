#pragma once
#include "Entity.h"
#include <Flow/FlowNodeArray.h>

#include <map>
#include <set>
#include <vector>
#include <utility>
#include <QColor>

namespace Nome::Scene
{

struct CLightInfo
{
    QColor color;
    std::string type;
    float direction[3];
    virtual ~CLightInfo() = default;
};


class CLight : public CEntity
{
DEFINE_INPUT(float, R) { MarkDirty(); }
DEFINE_INPUT(float, G) { MarkDirty(); }
DEFINE_INPUT(float, B) { MarkDirty(); }
DEFINE_INPUT(float, X) { MarkDirty(); }
DEFINE_INPUT(float, Y) { MarkDirty(); }
DEFINE_INPUT(float, Z) { MarkDirty(); }

public:
    DECLARE_META_CLASS(CLight, CEntity);
    CLight() = default;
    explicit CLight(std::string name)
        : CEntity(std::move(name))
    {
        renderType = LIGHT;
    }


    void MarkDirty() override;
    void UpdateEntity() override;
    bool IsInstantiable() override;
    bool IsMesh() override;
    CEntity* Instantiate(CSceneTreeNode* treeNode) override;

    CLightInfo& GetLight(){return LI;};


private:
    CLightInfo LI;
    CSceneTreeNode* SceneTreeNode;
};
}