#include "InteractiveLight.h"
#include "Nome3DView.h"
#include "ResourceMgr.h"
#include <Scene/Light.h>

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
        auto LightInstance = dynamic_cast<Scene::CLight*>(entity)->GetLight();
        if (LightInstance.type == "DIRECTIONAL") {
            if (!Light)
                Light = new Qt3DRender::QDirectionalLight();
            auto *dLight = dynamic_cast<Qt3DRender::QDirectionalLight *>(Light);
            dLight->setColor(LightInstance.color);
            dLight->setWorldDirection(QVector3D(LightInstance.direction[0], LightInstance.direction[1], LightInstance.direction[2])); 
            this->addComponent(Light);
            // Qt3DCore::QTransform *lightTransform = new Qt3DCore::QTransform(this);
            // lightTransform->setTranslation(QVector3D(3, 3, 2.73));
            // this->addComponent(lightTransform); 
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