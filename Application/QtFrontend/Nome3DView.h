#pragma once
#include "DebugDraw.h"
#include "InteractiveCamera.h"
#include "InteractiveLight.h"
#include "InteractiveMesh.h"
#include "OrbitTransformController.h"
#include "Scene/Window.h"
#include <Ray.h>
#include <Scene/Scene.h>

#include <Qt3DExtras>

#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <Scene/Mesh.h>
// TODO: enable multiple view ports
#include <QRenderSurfaceSelector>
#include <QViewport>

namespace Nome
{

class CNome3DView : public Qt3DExtras::Qt3DWindow
{
public:
    CNome3DView();

    ~CNome3DView() override;

    [[nodiscard]] const std::vector<std::string>& GetSelectedVertices() const
    {
        return SelectedVertices;
    }

    // Randy added on 10/14 for face selection
    [[nodiscard]] const std::vector<std::string>& GetSelectedFaces() const { return SelectedFaces; }

    [[nodiscard]] const std::unordered_map<std::string, DSMesh*> &GetSelectedMeshInstances() const
    {
        return SelectedFacesMap;
    }
    // Randy added on 11/5 for edge selection
    [[nodiscard]] const std::vector<std::string>& GetSelectedEdgeVertices() const
    {
        return SelectedEdgeVertices;
    }

    // Randy added on 2/26 for adding vertices via a ray
    [[nodiscard]] const std::vector<tc::Vector3>& GetRayVertPositions() const
    {
        return RayVertPositions;
    }

    [[nodiscard]] const std::vector<tc::Vector3>& GetInteractivePoint() const
    {
        return RayInteractivePoint;
    }

    void ClearSelectedVertices(); // Randy added on 9/27
    void ClearSelectedFaces(); // Randy added on 10/14 for deselecting faces
    void ClearSelectedEdges(); // Randy added on 11/5 for deselecting edges
    void ClearRenderedRay(); // Randy added on 2/26 for deselecting ray
    void ClearInteractivePoint();
    void TakeScene(const tc::TAutoPtr<Scene::CScene>& scene);
    void UnloadScene();
    void PostSceneUpdate();
    void PickVertexWorldRay(tc::Ray& ray, bool sharpSelection);
    void PickFaceWorldRay(tc::Ray& ray); // Randy added on 10/10
    void PickEdgeWorldRay(tc::Ray& ray); // Randy added on 10/29
    void PickPolylineWorldRay(tc::Ray& ray); // Randy added on 12/22
    void RenderRay(tc::Ray& ray, QVector3D intersection); // Randy added on 2/26
    bool WireFrameMode = false; // Randy added on 10/16 for choose wireframe mode or default mode.
                                // Changed to false on 11/21

    void FreeVertexSelection();

    static Qt3DCore::QEntity* MakeGridEntity(Qt3DCore::QEntity* parent);

    bool PickVertexBool = false; // Randy added on 11/5/20
    bool BackFaceBool = true;
    bool FrontFaceBool = true;
    bool VertexSharpnessBool = false; // Randy added on 6/20/21
    bool PickFaceBool = false; // Randy added on 11/5/20
    bool PickEdgeBool = false; // Randy added on 11/5/20
    bool PickPolylineBool = false; // Randy added on 12/22/20
    bool RenderRayBool = false; // Randy added on 2/26/20
    bool RayCasted = false;
    std::unordered_set<CInteractiveLight*> InteractiveLights;
    std::unordered_set<CInteractiveCamera*> InteractiveCameras;
    // For the animation
    Qt3DCore::QTransform* sphereTransform;
protected:
    // Xinyu added on Oct 8 for rotation
    void mouseMoveEvent(QMouseEvent* e) override;
    void mousePressEvent(QMouseEvent* e) override;
    void mouseReleaseEvent(QMouseEvent* e) override;
    void wheelEvent(QWheelEvent* ev) override;
    void keyPressEvent(QKeyEvent* ev) override;
    bool eventFilter(QObject* obj, QEvent* event) override;

private:
    QVector2D GetProjectionPoint(QVector2D originalPosition);
    static QVector3D GetCrystalPoint(QVector2D originalPoint);
    void rotateRay(tc::Ray& ray);
    float InputSharpness();

private:
    Qt3DCore::QEntity* Root;
    Qt3DCore::QEntity* Base;
    tc::TAutoPtr<Scene::CScene> Scene;
    std::unordered_set<CInteractiveMesh*> InteractiveMeshes;
    std::unordered_map<Scene::CEntity*, CDebugDraw*> EntityDrawData;
    std::vector<std::string> SelectedVertices;
    std::vector<tc::Vector3> RayVertPositions; // Randy added on 2/26 for adding vertices via a Ray
    std::vector<tc::Vector3>
        RayInteractivePoint; // Randy added on 2/26 for adding vertices via a Ray
    std::vector<std::string> SelectedFaces; // Randy added on 10/10
    std::unordered_map<std::string, DSMesh*> SelectedFacesMap;
    std::vector<std::string> SelectedEdgeVertices; // Randy added on 11/5
    // std::vector<const & std::vector<std::string>> SelectedEdgeVertPositions; // There are no edge
    // "names" right now TODO: Introduce Edge names and handles    bool vertexSelectionEnabled;
    bool vertexSelectionEnabled;


    // Xinyu added on Oct 8 for rotation
    QMatrix4x4 projection;
    QVector2D firstPosition;
    QVector2D secondPosition;

    /* Brian Added Jun 15 2023 for dynamic rendering */
    tc::Matrix3x4 camerarotation = tc::Matrix3x4::IDENTITY;
    
     
    QQuaternion rotation;
    Qt3DRender::QCamera* mainCamera;
    // Qt3DRender::QMaterial *material;
    bool mousePressEnabled;

    bool rotationEnabled;

    bool animationEnabled;
    float zPos;

    float objectX;
    float objectY;
    float objectZ;


    OrbitTransformController* controller;
    QPropertyAnimation* sphereRotateTransformAnimation;
    Qt3DCore::QEntity* torus;

    Qt3DExtras::QPhongAlphaMaterial* material;

    // TODO: configure viewport
    std::map<std::string, Qt3DRender::QCameraSelector*> camViewMap;
    std::map<std::string, Qt3DRender::QCamera*> cameraSet;

    Qt3DRender::QViewport* mainView;
    Qt3DRender::QClearBuffers* clearBuffers;
    Qt3DRender::QRenderSurfaceSelector* ss;
};
}