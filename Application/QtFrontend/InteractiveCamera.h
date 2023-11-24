#pragma once
#include <Scene/RendererInterface.h>
#include <Scene/SceneGraph.h>

#include <Qt3DCore/QEntity>
#include <Qt3DCore/QTransform>
#include <QCamera>
#include <QVector3D>
#include <QVector4D>


enum CameraType { Perspective, Orthogonal };


namespace Nome
{

class CInteractiveCamera : public Qt3DCore::QEntity
{
public:
    explicit CInteractiveCamera(Scene::CSceneTreeNode* node);

    [[nodiscard]] Scene::CSceneTreeNode* GetSceneTreeNode() const { return SceneTreeNode; }

    void UpdateTransform();
    void UpdateCamera();
    void CameraScroll(float objectz); 
    std::string GetCameraType();

public:
    Qt3DRender::QCamera* Camera = nullptr;
    Qt3DCore::QTransform* Transform = nullptr;
    CameraType type;
    std::string name;
    float left; 
    float right;
    float bottom; 
    float top; 
    float nearplane;
    float farplane; 
    


private:
    Scene::CSceneTreeNode* SceneTreeNode = nullptr;
};

}
