#include "InteractiveCamera.h"
#include "Nome3DView.h"
#include <cmath>
#include "ResourceMgr.h"
#include <Scene/Camera.h>

namespace Nome
{

CInteractiveCamera::CInteractiveCamera(Scene::CSceneTreeNode* node)
    : SceneTreeNode(node)
{
    UpdateCamera();
}

void CInteractiveCamera::UpdateTransform() {
    const auto &tf = SceneTreeNode->L2WTransform.GetValue(tc::Matrix3x4::IDENTITY);
    QMatrix4x4 qtf{tf.ToMatrix4().Data()};

    Camera->setUpVector(QVector3D(0, 0, 1));
    Camera->setViewCenter(QVector3D(0, 0, 0));
    //Camera->setProjectionMatrix(qtf);

}
/* Brian add on Jun 7 2023 for Parallel, Perspective Projection with user defined frustum. */
void CInteractiveCamera::UpdateCamera()
{
    auto* entity = SceneTreeNode->GetInstanceEntity();
    if (!entity)
    {
        entity = SceneTreeNode->GetOwner()->GetEntity();
    }
    if (entity)
    {
        auto CameraInstance = dynamic_cast<Scene::CCamera*>(entity)->GetCamera();
        name = entity->GetNameWithoutPrefix();
        if (!Camera)
            Camera = new Qt3DRender::QCamera();
        if (CameraInstance.type == "PARALLEL") {
            this->Camera->lens()->setOrthographicProjection(CameraInstance.para[0],
                                                      CameraInstance.para[1],
                                                      CameraInstance.para[2],
                                                      CameraInstance.para[3],
                                                      CameraInstance.para[4],
                                                      CameraInstance.para[5]);

            type = Perspective;
        } else if (CameraInstance.type == "PERSPECTIVE") {
            float aspect_ratio = (float)(CameraInstance.para[1] - CameraInstance.para[0])/(float)(CameraInstance.para[3] - CameraInstance.para[2]);
            float fov = (float)(std::atan(CameraInstance.para[3]/CameraInstance.para[4]) * 2); 
            Camera->lens()->setPerspectiveProjection(fov, 
                                                        aspect_ratio,
                                                        CameraInstance.para[4],
                                                        CameraInstance.para[5]);
            type = Perspective;
        }
        else {
            Camera->lens()->setPerspectiveProjection(45.0f, 1280.f / 720.f, 0.1f, 1000.0f);
            std::cout << CameraInstance.type << " is not a Camera instance, Using default camera projection." << std::endl;
        }
    }
}
}