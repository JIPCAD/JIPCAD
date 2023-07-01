#pragma once
#include <Scene/RendererInterface.h>
#include <Scene/SceneGraph.h>

#include <Qt3DCore/QEntity>
#include <Qt3DCore/QTransform>
#include <QDirectionalLight>
#include <QCamera>
#include <QVector3D>
#include <QVector4D>

enum LightType { SpotLight, AmbientLight, DirectionalLight, PointLight };

namespace Nome
{

class CInteractiveLight : public Qt3DCore::QEntity
{
public:
    explicit CInteractiveLight(Scene::CSceneTreeNode* node);

    [[nodiscard]] Scene::CSceneTreeNode* GetSceneTreeNode() const { return SceneTreeNode; }

    void UpdateTransform();
    void UpdateLight();

public:
    Qt3DRender::QAbstractLight* Light = nullptr;
    Qt3DRender::QCamera* Camera = nullptr;
    Qt3DCore::QTransform* sphereTransform;
    LightType type;
    QColor Color;

private:
    Scene::CSceneTreeNode* SceneTreeNode = nullptr;
};

}
