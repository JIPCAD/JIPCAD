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