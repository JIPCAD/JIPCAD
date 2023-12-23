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
    if (tf.ToMatrix4().Data()) {
        QMatrix4x4 qtf{tf.ToMatrix4().Data()};
        if (!qtf.isIdentity()) {
            Camera->setViewCenter(QVector3D(0, 0, 0));
            QVector3D vec = QVector3D(0, 0, 2.73);
            QVector4D prevupvec = QVector4D(0, 1, 0, 1);
            QVector4D proj_vec = vec.toVector4D();
            QVector4D newupvec = qtf * prevupvec; 
            proj_vec.setW(1); //For Translation!
            QVector4D mult_vec = qtf * proj_vec;
            QVector3D eye = QVector3D(mult_vec.x(), mult_vec.y(), mult_vec.z());
            QVector3D line_of_sight = QVector3D(0-(float)mult_vec.x(), 0-(float)mult_vec.y(), 0-(float)mult_vec.z()).normalized();
            Camera->setPosition(eye);
            QVector3D u = QVector3D::crossProduct(line_of_sight, QVector3D(0, 1, 0)).normalized();
            QVector3D v = QVector3D::crossProduct(u, line_of_sight).normalized();
            QVector3D updatednewupvec = newupvec.toVector3D().normalized(); 
            //std::cout << mult_vec.x() << " " << mult_vec.y() << " " << mult_vec.z() << std::endl;
            //std::cout << updatednewupvec.x() << " " << updatednewupvec.y() << " " << updatednewupvec.z() << std::endl;
            Camera->setUpVector(updatednewupvec);
            //Camera->setUpVector(qtf.column(3).toVector3D().normalized());
            // Camera->setProjectionMatrix(qtf);
            Camera->transform(); 
        } else {
            Camera->setViewCenter(QVector3D(0, 0, 0));
            Camera->setPosition(QVector3D(0, 0, 2.73));
            Camera->setUpVector(QVector3D(0, 1, 0));
        }
    }

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
            left = CameraInstance.para[0];
            right = CameraInstance.para[1];
            bottom = CameraInstance.para[2]; 
            top = CameraInstance.para[3]; 
            nearplane = CameraInstance.para[4]; 
            farplane = CameraInstance.para[5];  
            type = Orthogonal;
        } else if (CameraInstance.type == "PERSPECTIVE") {
            float aspect_ratio = (float)(CameraInstance.para[3] - CameraInstance.para[2])/(float)(CameraInstance.para[1] - CameraInstance.para[0]);
            float fov = (float)(std::atan(CameraInstance.para[3]/CameraInstance.para[4]) * 2); 
            std::cout << aspect_ratio << std::endl;
            Camera->lens()->setPerspectiveProjection(45.0f, 
                                                        (1280.f / 720.f)* aspect_ratio,
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

std::string CInteractiveCamera::GetCameraType() {
    auto* entity = SceneTreeNode->GetInstanceEntity();
    if (!entity)
    {
        entity = SceneTreeNode->GetOwner()->GetEntity();
    }
    if (entity)
    {
        auto CameraInstance = dynamic_cast<Scene::CCamera*>(entity)->GetCamera();
        return CameraInstance.type; 
    }
    return NULL; 
}
void CInteractiveCamera::CameraScroll(float objectz) {
    if (left + objectz > right - objectz) return; 
    if (bottom + objectz > top - objectz) return;
    left += objectz;
    right -= objectz;
    bottom += objectz; 
    top -= objectz; 
    this->Camera->lens()->setOrthographicProjection(left,
                            right,
                            bottom,
                            top,
                            nearplane,
                            farplane);
}
}

