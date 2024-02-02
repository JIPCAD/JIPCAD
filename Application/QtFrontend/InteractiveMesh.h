#pragma once
#include "DebugDraw.h"
#include <Scene/RendererInterface.h>
#include <Scene/SceneGraph.h>
#include <array>
#include <Qt3DCore/QEntity>
#include <Qt3DCore/QTransform>
#include <Qt3DRender/QMaterial>

namespace Nome
{

class CInteractiveMesh : public Qt3DCore::QEntity
{
public:
    explicit CInteractiveMesh(Scene::CSceneTreeNode* node, std::array<float, 3> frontColor, std::array<float, 3> backColor);

    [[nodiscard]] Scene::CSceneTreeNode* GetSceneTreeNode() const { return SceneTreeNode; }

    void UpdateTransform();
    void UpdateGeometry(bool showVertBox, bool showBackFace = true, bool showFrontFace = true);
    void UpdateMaterial();
    void InitInteractions();
    void SetDebugDraw(const CDebugDraw* debugDraw);

private:
    Scene::CSceneTreeNode* SceneTreeNode = nullptr;

    Qt3DCore::QTransform* Transform = nullptr;
    #ifdef __ARM_ARCH
    Qt3DCore::QGeometry* Geometry = nullptr;
    #else 
    Qt3DRender::QGeometry* Geometry = nullptr;
    #endif
    Qt3DRender::QGeometryRenderer* GeometryRenderer = nullptr;
    Qt3DRender::QMaterial* Material = nullptr;
    Qt3DRender::QMaterial* LineMaterial = nullptr;

    Qt3DCore::QEntity* PointEntity;
    Qt3DRender::QMaterial* PointMaterial;
    #ifdef __ARM_ARCH
    Qt3DCore::QGeometry* PointGeometry;
    #else
    Qt3DRender::QGeometry* PointGeometry;
    #endif
    Qt3DRender::QGeometryRenderer* PointRenderer;

    std::array<float, 3> InstanceColor;
    std::array<float, 3> InstanceBackColor;
};

}
