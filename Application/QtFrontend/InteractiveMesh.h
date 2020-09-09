#pragma once
#include "DebugDraw.h"
#include <Scene/RendererInterface.h>
#include <Scene/SceneGraph.h>

#include <Qt3DCore/QEntity>
#include <Qt3DCore/QTransform>
#include <Qt3DRender/QMaterial>

namespace Nome
{

class CInteractiveMesh : public Qt3DCore::QEntity
{
public:
    explicit CInteractiveMesh(Scene::CSceneTreeNode* node);

    [[nodiscard]] Scene::CSceneTreeNode* GetSceneTreeNode() const { return SceneTreeNode; }

    void UpdateTransform();
    void UpdateGeometry();
    void UpdateMaterial();
    void InitInteractions();
    void SetDebugDraw(const CDebugDraw* debugDraw);

private:
    Scene::CSceneTreeNode* SceneTreeNode = nullptr;

    Qt3DCore::QTransform* Transform = nullptr;
    Qt3DRender::QGeometry* Geometry = nullptr;
    Qt3DRender::QGeometryRenderer* GeometryRenderer = nullptr;
    Qt3DRender::QMaterial* Material = nullptr;
    Qt3DRender::QMaterial* LineMaterial = nullptr;

    Qt3DCore::QEntity* PointEntity;
    Qt3DRender::QMaterial* PointMaterial;
    Qt3DRender::QGeometry* PointGeometry;
    Qt3DRender::QGeometryRenderer* PointRenderer;
};

}
