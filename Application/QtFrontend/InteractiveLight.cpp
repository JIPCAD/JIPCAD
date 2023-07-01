#include "InteractiveLight.h"
#include "Nome3DView.h"
#include "ResourceMgr.h"
#include <Scene/Light.h>
#include <Qt3DExtras>
#include <QTransform>


namespace Nome
{

CInteractiveLight::CInteractiveLight(Scene::CSceneTreeNode* node)
    : SceneTreeNode(node)
{
    UpdateLight();
}

void CInteractiveLight::UpdateTransform() {
    if (type == DirectionalLight) {
        auto *dLight = dynamic_cast<Qt3DRender::QDirectionalLight *>(Light);
        dLight -> worldDirectionChanged(QVector3D(0, 1, 0));
        const auto &tf = SceneTreeNode->L2WTransform.GetValue(tc::Matrix3x4::IDENTITY);
        QMatrix4x4 qtf{tf.ToMatrix4().Data()};
        QVector4D mult = qtf * QVector4D(0, 0, 0, 1);
        QVector3D pointdir = QVector3D((float)0-mult.x(), (float)0-mult.y(), (float)0-mult.z());
        //std::cout << pointdir.x() << " " << pointdir.y() << " " << pointdir.z() << std::endl; 
        //dLight->position = pointdir;
        //dLight->setWorldDirection(pointdir);
    }
}

void CInteractiveLight::UpdateLight()
{
    auto* entity = SceneTreeNode->GetInstanceEntity();
    if (!entity)
    {
        entity = SceneTreeNode->GetOwner()->GetEntity();
    }
    if (entity)
    {
        // std::cout << "UPDATING" << std::endl;
        // std::cout << sphereTransform << std::endl;
        // Qt3DCore::QTransform* st = sphereTransform;
        // if (*st) {
        //     std::cout << st->matrix() << std::endl;
        // }
        //QMatrix4x4 transform = QMatrix4x4(&this->sphereTransform);
        auto LightInstance = dynamic_cast<Scene::CLight*>(entity)->GetLight();
        std::cout << LightInstance.direction[0] << ' ' << LightInstance.direction[1] << ' ' << LightInstance.direction[0] << std::endl;
        if (LightInstance.type == "DIRECTIONAL") {
            if (!Light)
                Light = new Qt3DRender::QDirectionalLight();
            auto *dLight = dynamic_cast<Qt3DRender::QDirectionalLight *>(Light);
            Light->setColor(LightInstance.color);
            Light->setIntensity((float)1.0);
            dLight->setWorldDirection(QVector3D(LightInstance.direction[0], LightInstance.direction[1], LightInstance.direction[2]));
            this->addComponent(Light);
            Light->setEnabled(true); 
            type = DirectionalLight;
        } else if (LightInstance.type == "AMBIENT") {
            if (!Light)
                Light = new Qt3DRender::QPointLight();
            Light->setColor(LightInstance.color);
            Color = LightInstance.color;
            Light->setEnabled(true); 
            type = AmbientLight;
        } else if (LightInstance.type == "SPOTLIGHT") {
            if (!Light)
                Light = new Qt3DRender::QSpotLight();
            auto *dLight = dynamic_cast<Qt3DRender::QSpotLight *>(Light);
            Light->setColor(LightInstance.color);
            dLight->setLocalDirection(QVector3D(LightInstance.direction[0], LightInstance.direction[1], LightInstance.direction[2])); 
            Light->setEnabled(true); 
            type = SpotLight;
        } 
        else
        {
            std::cout << "The entity is not a Light instance, we don't know how to handle it. For "
                         "example, if you try to instanciate a face, it'll generate this "
                         "placeholder sphere."
                      << std::endl;
        }
    }
}
}