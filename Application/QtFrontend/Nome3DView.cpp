#include "Nome3DView.h"
#include "FrontendContext.h"
#include "MainWindow.h"
#include "ResourceMgr.h"
#include "InteractiveCamera.h"
#include <Scene/Mesh.h>

#include <QBuffer>
#include <QDialog>

#include <iostream>
#include <unordered_map>
#include <QHBoxLayout>
#include <QInputDialog>
#include <QPushButton>
#include <QStatusBar>
#include <QTableWidget>
#include <Scene/Camera.h>
#include <Scene/Viewport.h>

namespace Nome
{

CNome3DView::CNome3DView()
    : mousePressEnabled(false)
    , animationEnabled(false)
    , rotationEnabled(true)
    , vertexSelectionEnabled(false)

{
    // Create a Base entity to host all entities
    Base = new Qt3DCore::QEntity;

    // Viewport initialization
    //this->defaultFrameGraph()->setClearColor(QColor(0,0,0,255));
    // addRenderer = new Qt3DExtras::QForwardRenderer();

    // addRenderer->setClearColor(QColor(QRgb(0xf0f000)));
    ss = new Qt3DRender::QRenderSurfaceSelector;

    Root = new Qt3DCore::QEntity(Base);

    this->setRootEntity(Base);

    // Setup camera
    zPos = 2.73;
    mainCamera = this->camera();
    //mainCamera->lens()->setPerspectiveProjection(45.0f, 1280.f / 720.f, 0.1f, 1000.0f);
    mainCamera->setPosition(QVector3D(0, 0, zPos));
    mainCamera->setViewCenter(QVector3D(0, 0, 0));


    // Xinyu add on Oct 8 for rotation
    projection.setToIdentity();
    objectX = objectY = objectZ = 0;
    // Set up the animated rotation and activate by space key
    sphereTransform = new Qt3DCore::QTransform;
    controller = new OrbitTransformController(rotation, sphereTransform);
    controller->setTarget(sphereTransform);
    controller->setRadius(0);
    sphereRotateTransformAnimation = new QPropertyAnimation(sphereTransform);
    sphereRotateTransformAnimation->setTargetObject(controller);
    sphereRotateTransformAnimation->setPropertyName("angle");
    sphereRotateTransformAnimation->setStartValue(QVariant::fromValue(0));
    sphereRotateTransformAnimation->setEndValue(QVariant::fromValue(360));
    sphereRotateTransformAnimation->setDuration(100000);
    sphereRotateTransformAnimation->setLoopCount(-1);

    Root->addComponent(sphereTransform);
    mainView = new Qt3DRender::QViewport(ss);
    clearBuffers = new Qt3DRender::QClearBuffers(mainView);
}

CNome3DView::~CNome3DView() { UnloadScene(); }

void CNome3DView::TakeScene(const tc::TAutoPtr<Scene::CScene>& scene)
{
    using namespace Scene;
    Scene = scene;
    this->defaultFrameGraph()->setClearColor(QColor(255*Scene->backgroundColor[0],255*Scene->backgroundColor[1],255*Scene->backgroundColor[2],255));
    Scene->Update();
    Scene->ForEachSceneTreeNode(
        [this](CSceneTreeNode* node)
        {
            auto* entity = node->GetInstanceEntity();
            if (!entity)
            {
                entity = node->GetOwner()->GetEntity();
            }
            if (entity)
            {
                if (!entity->IsMesh())
                {
                    if (entity->renderType == Scene::CEntity::LIGHT)
                    {
                        // Create an InteractiveLight from the scene node
                        auto* light = new CInteractiveLight(node);
                        light->setParent(this->Root);
                        light->sphereTransform = sphereTransform;
                        InteractiveLights.insert(light);
                        node->SetEntityUpdated(false);
                    }
                    else if (entity->renderType == Scene::CEntity::WINDOW)
                    {
                        auto* window = dynamic_cast<Scene::CWindow*>(entity);
                        auto& w = window->window;
                        int m_w = this->maximumWidth();
                        int m_h = this->maximumHeight();
                        QRect adapt_w =
                            QRect(w.x() * m_w, w.y() * m_h, w.width() * m_w, w.height() * m_h);
                        this->setGeometry(adapt_w);
                        this->defaultFrameGraph()->setClearColor(window->Background);
                        clearBuffers->setClearColor(window->Background);
                        node->SetEntityUpdated(false);
                    }
                    else if (entity->renderType == Scene::CEntity::VIEWPORT)
                    {
                        auto* viewport = dynamic_cast<Scene::CViewport*>(entity);
                        if (this->activeFrameGraph() == this->defaultFrameGraph())
                        {
                            this->setActiveFrameGraph(ss);
                            this->renderSettings()->setActiveFrameGraph(ss);
                            mainView->setNormalizedRect(QRectF(0, 0, 1, 1));
                            clearBuffers->setBuffers(Qt3DRender::QClearBuffers::ColorDepthBuffer);
                            clearBuffers->setClearColor(QColor(QRgb(0x4d4d4f)));
                            auto* noDraw = new Qt3DRender::QNoDraw(clearBuffers);
                            // auto* mainCamSelector = new Qt3DRender::QCameraSelector(mainView);
                            // mainCamSelector->setCamera(mainCamera);
                        }
                        auto* vp = new Qt3DRender::QViewport(mainView);
                        vp->setNormalizedRect(QRectF(viewport->viewport));
                        auto* camSelector = new Qt3DRender::QCameraSelector(vp);
                        camViewMap.emplace(viewport->cameraId, camSelector);
                    }
                    else if (entity->renderType == Scene::CEntity::CAMERA)
                    {
                        // Create an InteractiveLight from the scene node
                        auto* camera = new CInteractiveCamera(node);
                        camera->setParent(this->Root);
                        camera->Camera = this->camera();
                        InteractiveCameras.insert(camera);
                        //IF a camera is rotated, the OrbitTransformController needs to be reconfigured
                        this->camerarotation = node->L2WTransform.GetValue(tc::Matrix3x4::IDENTITY);

                        //node->SetEntityUpdated(false);
                    }
                }
                else
                {
                    // Create an InteractiveMesh from the scene node
                    auto* mesh = new CInteractiveMesh(node, Scene->frontColor, Scene->backColor);
                    mesh->setParent(this->Root);
                    InteractiveMeshes.insert(mesh);
                }
            }
        });
    PostSceneUpdate();
}

void CNome3DView::UnloadScene()
{
    for (auto* m : InteractiveMeshes)
        delete m;
    InteractiveMeshes.clear();
    for (auto* l : InteractiveLights)
        delete l;
    for (auto* c : InteractiveCameras)
        delete c;
    InteractiveLights.clear();
    InteractiveCameras.clear();
    this->renderSettings()->setActiveFrameGraph(this->defaultFrameGraph());
    Scene = nullptr;
}

void CNome3DView::PostSceneUpdate()
{
    using namespace Scene;
    bool bUpdateScene = false;
    std::unordered_map<CSceneTreeNode*, CInteractiveMesh*> sceneMeshAssoc;
    std::unordered_map<CSceneTreeNode*, CInteractiveLight*> sceneLightAssoc;
    std::unordered_map<CSceneTreeNode*, CInteractiveCamera*> sceneCameraAssoc;

    std::unordered_set<CInteractiveMesh*> aliveSetMesh;
    std::unordered_set<CInteractiveLight*> aliveSetLight;
    std::unordered_set<CInteractiveCamera*> aliveSetCamera;
    std::unordered_map<Scene::CEntity*, CDebugDraw*> aliveEntityDrawData;
    for (auto* m : InteractiveMeshes)
        sceneMeshAssoc.emplace(m->GetSceneTreeNode(), m);
    for (auto* l : InteractiveLights)
        sceneLightAssoc.emplace(l->GetSceneTreeNode(), l);
    for (auto* c : InteractiveCameras)
        sceneCameraAssoc.emplace(c->GetSceneTreeNode(), c);

    Scene->ForEachSceneTreeNode(
        [&](CSceneTreeNode* node)
        {
            // Obtain either an instance entity or a shared entity from the scene node
            auto* entity = node->GetInstanceEntity();
            if (!entity)
            {
                entity = node->GetOwner()->GetEntity();
            }
            if (entity)
            {
                if (!entity->IsMesh())
                {
                    if (entity->renderType == Scene::CEntity::LIGHT)
                    {
                        /// add and update light
                        CInteractiveLight* light;
                        light->Camera = this->camera();
                        light->sphereTransform = sphereTransform; 
                        // Check for existing InteractiveMesh
                        auto iter = sceneLightAssoc.find(node);
                        if (iter != sceneLightAssoc.end())
                        {
                            // Found existing InteractiveMesh, mark as alive
                            light = iter->second;
                            aliveSetLight.insert(light);
                            light->UpdateTransform();
                            if (node->WasEntityUpdated())
                            {
                                light->UpdateLight();
                                bUpdateScene = true;
                                printf("Delivering the rendering light of the scene %s\n",
                                       node->GetPath().c_str());
                                node->SetEntityUpdated(false);
                            }
                        }
                        else
                        {
                            light = new CInteractiveLight(node);
                            light->setParent(this->Root);
                            aliveSetLight.insert(light);
                            InteractiveLights.insert(light);
                        }
                    }
                    else if (entity->renderType == Scene::CEntity::WINDOW)
                    {
                        if (node->WasEntityUpdated())
                        {
                            auto* window = dynamic_cast<Scene::CWindow*>(entity);
                            /// auto*
                            this->defaultFrameGraph()->setClearColor(window->Background);
                            clearBuffers->setClearColor(window->Background);
                            node->SetEntityUpdated(false);
                        }
                    }
                    else if (entity->renderType == Scene::CEntity::CAMERA)
                    {
                        /// add and update camera
                        CInteractiveCamera* camera;
                        // Check for existing InteractiveMesh
                        auto iter = sceneCameraAssoc.find(node);
                        if (iter != sceneCameraAssoc.end())
                        {
                            // Found existing InteractiveMesh, mark as alive
                            camera = iter->second;
                            aliveSetCamera.insert(camera);
                            camera->UpdateTransform();
                            if (node->WasEntityUpdated())
                            {
                                camera->UpdateCamera();
                                bUpdateScene = true;
                                printf("Delivering the rendering camera of the scene %s\n",
                                       node->GetPath().c_str());
                                node->SetEntityUpdated(false);
                                for (auto camMap : camViewMap)
                                {
                                    if (camMap.first == camera->name)
                                    {
                                        camera->Camera = new Qt3DRender::QCamera;
                                        camMap.second->setCamera(camera->Camera);
                                        cameraSet.emplace(camMap.first, camera->Camera);
                                    }
                                }
                            }
                        }
                        else
                        {
                            camera = new CInteractiveCamera(node);
                            camera->setParent(this->Root);
                            aliveSetCamera.insert(camera);
                            InteractiveCameras.insert(camera);
                        }
                    }
                    else if (entity->renderType == Scene::CEntity::VIEWPORT)
                    {
                        //TODO: may add the viewport change capability
                    }
                }
            }
        });

    Scene->ForEachSceneTreeNode(
        [&](CSceneTreeNode* node)
        {
            // Obtain either an instance entity or a shared entity from the scene node
            auto* entity = node->GetInstanceEntity();
            if (!entity)
            {
                entity = node->GetOwner()->GetEntity();
            }
            if (entity)
            {
                if (entity->IsMesh())
                {
                    CInteractiveMesh* mesh = nullptr;
                    // Check for existing InteractiveMesh
                    auto iter = sceneMeshAssoc.find(node);
                    if (iter != sceneMeshAssoc.end())
                    {
                        // Found existing InteractiveMesh, mark as alive
                        mesh = iter->second;
                        if (entity->isMerged)
                        {
                            auto iterr = aliveSetMesh.find(mesh);
                            if (iterr != aliveSetMesh.end())
                            {
                                aliveSetMesh.erase(iterr);
                            }
                        }
                        else
                        {
                            aliveSetMesh.insert(mesh);
                            mesh->UpdateTransform();
                            if (node->WasEntityUpdated() || bUpdateScene)
                            {
                                // WireFrameMode
                                mesh->UpdateMaterial();
                                mesh->UpdateGeometry(PickVertexBool, BackFaceBool, FrontFaceBool);
                                node->SetEntityUpdated(false);
                            }
                        }
                    }
                    else if (!entity->isMerged)
                    {
                        mesh = new CInteractiveMesh(node, Scene->frontColor, Scene->backColor);
                        mesh->setParent(this->Root);
                        aliveSetMesh.insert(mesh);
                        InteractiveMeshes.insert(mesh);
                    }
                    if (!entity->isMerged)
                    {
                        // Create a DebugDraw for the CEntity if not already
                        auto eIter = EntityDrawData.find(entity);
                        if (eIter == EntityDrawData.end())
                        {
                            auto* debugDraw = new CDebugDraw(Root);
                            aliveEntityDrawData[entity] = debugDraw;
                            // TODO: somehow uncommenting this line leads to a crash in Qt3D
                            // mesh->SetDebugDraw(debugDraw);
                        }
                        else
                        {
                            aliveEntityDrawData[entity] = eIter->second;
                            mesh->SetDebugDraw(eIter->second);
                        }
                    }
                }
            }
        });

    // Now kill all the dead objects, i.e., not longer in the scene graph. If it wasn't added to
    // aliveSetMesh above, then it is dead.
    for (auto* m : InteractiveMeshes)
    {
        auto iter = aliveSetMesh.find(m);
        if (iter == aliveSetMesh.end())
        {
            // Not in aliveSetMesh
            delete m;
        }
    }
    InteractiveMeshes = std::move(aliveSetMesh);

    // Take the same method as the mesh
    for (auto* l : InteractiveLights)
    {
        auto iter = aliveSetLight.find(l);
        if (iter == aliveSetLight.end())
        {
            // Not in aliveSetMesh
            delete l;
        }
    }
    InteractiveLights = std::move(aliveSetLight);

    for (auto* c : InteractiveCameras)
    {
        auto iter = aliveSetCamera.find(c);
        if (iter == aliveSetCamera.end())
        {
            // Not in aliveSetMesh
            delete c;
        }
    }
    InteractiveCameras = std::move(aliveSetCamera);

    // Kill all entity debug draws that are not alive
    for (auto& iter : EntityDrawData)
    {
        auto iter2 = aliveEntityDrawData.find(iter.first);
        if (iter2 == aliveEntityDrawData.end())
        {
            delete iter.second;
        }
    }

    EntityDrawData = std::move(aliveEntityDrawData);
    for (const auto& pair : EntityDrawData)
    {
        pair.second->Reset();
        pair.first->Draw(pair.second, WireFrameMode);
        pair.second->Commit();
    }
}

// Randy added 9/27
void CNome3DView::ClearSelectedVertices()
{
    SelectedVertices.clear();
    Scene->ForEachSceneTreeNode(
        [&](Scene::CSceneTreeNode* node)
        {
            // Obtain either an instance entity or a shared entity from the scene node
            auto* entity = node->GetInstanceEntity();
            if (!entity)
                entity = node->GetOwner()->GetEntity();
            if (entity)
            {
                auto* meshInst = dynamic_cast<Scene::CMeshInstance*>(entity);
                meshInst->DeselectAll();
            }
        });
}

// Randy added on 10/14 to clear face selection. Only used with "Remove face", which isn't
// implemented yet.
void CNome3DView::ClearSelectedFaces()
{
    SelectedFaces.clear();
    Scene->ForEachSceneTreeNode(
        [&](Scene::CSceneTreeNode* node)
        {
            // Obtain either an instance entity or a shared entity from the scene node
            auto* entity = node->GetInstanceEntity();
            if (!entity)
                entity = node->GetOwner()->GetEntity();
            if (entity)
            {
                auto* meshInst = dynamic_cast<Scene::CMeshInstance*>(entity);
                meshInst->DeselectAll();
            }
        });
}

// Randy added on 11/5 to clear edge selection
void CNome3DView::ClearSelectedEdges()
{
    SelectedEdgeVertices.clear();
    Scene->ForEachSceneTreeNode(
        [&](Scene::CSceneTreeNode* node)
        {
            // Obtain either an instance entity or a shared entity from the scene node
            auto* entity = node->GetInstanceEntity();
            if (!entity)
                entity = node->GetOwner()->GetEntity();
            if (entity)
            {
                auto* meshInst = dynamic_cast<Scene::CMeshInstance*>(entity);
                meshInst->DeselectAll();
            }
        });
}

// Randy added on 2/26 to clear rendered ray
void CNome3DView::ClearRenderedRay()
{
    RayVertPositions.clear();
    Scene->ForEachSceneTreeNode(
        [&](Scene::CSceneTreeNode* node)
        {
            // Obtain either an instance entity or a shared entity from the scene node
            auto* entity = node->GetInstanceEntity();
            if (!entity)
                entity = node->GetOwner()->GetEntity();
            if (entity)
            {
                auto* meshInst = dynamic_cast<Scene::CMeshInstance*>(entity);
                meshInst->DeselectAll();
            }
        });
}

// Randy added on 2/26 to clear rendered ray
void CNome3DView::ClearInteractivePoint()
{
    RayInteractivePoint.clear();
    Scene->ForEachSceneTreeNode(
        [&](Scene::CSceneTreeNode* node)
        {
            // Obtain either an instance entity or a shared entity from the scene node
            auto* entity = node->GetInstanceEntity();
            if (!entity)
                entity = node->GetOwner()->GetEntity();
            if (entity)
            {
                auto* meshInst = dynamic_cast<Scene::CMeshInstance*>(entity);
                meshInst->DeselectAll();
            }
        });
}
void CNome3DView::PickFaceWorldRay(tc::Ray& ray)
{
    rotateRay(ray);
    std::vector<std::tuple<float, Scene::CMeshInstance*, std::string>> hits;
    Scene->ForEachSceneTreeNode(
        [&](Scene::CSceneTreeNode* node)
        {
            // Obtain either an instance entity or a shared entity from the scene node
            auto* entity = node->GetInstanceEntity();
            if (!entity)
                entity = node->GetOwner()->GetEntity();
            if (entity)
            {
                if (!entity->isMerged && entity->IsMesh())
                {
                    const auto& l2w = node->L2WTransform.GetValue(tc::Matrix3x4::IDENTITY);
                    auto localRay = ray.Transformed(l2w.Inverse());
                    localRay.Direction =
                        localRay.Direction
                            .Normalized(); // Normalize to fix "scale" error caused by l2w.Inverse()
                    auto* meshInst = dynamic_cast<Scene::CMeshInstance*>(entity);
                    auto pickResults = meshInst->PickFaces(localRay);
                    for (const auto& [dist, name] : pickResults)
                        hits.emplace_back(dist, meshInst, name);
                }
            }
        });

    std::sort(hits.begin(), hits.end());
    // if (!hits.empty()) {
    //    hits.resize(1); // Force there to be only one face selected. This is more user-friendly.
    //}
    if (hits.size() == 1)
    {
        const auto& [dist, meshInst, faceName] = hits[0];
        auto position = std::find(SelectedFaces.begin(), SelectedFaces.end(), faceName);
        if (position == SelectedFaces.end())
        { // if this face has not been selected before
            SelectedFaces.push_back(faceName); // add face to selected faces
            SelectedFacesMap[faceName] = &meshInst->GetDSMesh() ;
            //SelectedFacesMap.emplace_back(std::make_pair(faceName, meshInst));
            GFrtCtx->MainWindow->statusBar()->showMessage(
                QString::fromStdString("Selected " + faceName));
        }
        else // else, this face has been selected previously
        {
            const auto& newposition = position;
            SelectedFaces.erase(position);
            //SelectedFacesMap.erase(faceName);
            GFrtCtx->MainWindow->statusBar()->showMessage(
                QString::fromStdString("Deselected " + faceName));
        }
        meshInst->MarkFaceAsSelected({ faceName }, true, InputSharpness());
    }
    else if (!hits.empty())
    {
        // Show a dialog for the user to choose one face
        auto* dialog = new QDialog(GFrtCtx->MainWindow);
        dialog->setModal(true);
        auto* layout1 = new QHBoxLayout(dialog);
        auto* table = new QTableWidget();
        table->setRowCount(hits.size());
        table->setColumnCount(2);
        QStringList titles;
        titles.append(QString::fromStdString("Closeness Rank"));
        titles.append(QString::fromStdString("Face Name"));
        table->setHorizontalHeaderLabels(titles);
        int closenessRank = 1;
        for (size_t i = 0; i < hits.size(); i++)
        {
            const auto& [dist, meshInst, faceName] = hits[i];
            if (i != 0)
            {
                const auto& [prevDist, prevMeshInst, prevFaceName] = hits[i - 1];
                if (round(dist * 100) != round(prevDist * 100))
                {
                    closenessRank += 1;
                }
                // else, closenessRank stay the same as prev as the distance is the same (faces
                // in same location)
            }

            auto* distWidget = new QTableWidgetItem(QString::number(closenessRank));
            auto* item = new QTableWidgetItem(QString::fromStdString(faceName));
            table->setItem(i, 0, distWidget); // i is row num, and 0 is col num
            table->setItem(i, 1, item);
        }
        layout1->addWidget(table);
        auto* layout2 = new QVBoxLayout();
        auto* btnOk = new QPushButton();
        btnOk->setText("OK");
        connect(btnOk, &QPushButton::clicked,
                [this, dialog, table, hits]()
                {
                    auto sel = table->selectedItems();
                    if (!sel.empty())
                    {
                        int row = sel[0]->row();
                        const auto& [dist, meshInst, faceName] = hits[row];
                        auto position =
                            std::find(SelectedFaces.begin(), SelectedFaces.end(), faceName);
                        if (position == SelectedFaces.end())
                        { // if this face has not been selected before
                            SelectedFaces.push_back(faceName); // add face to selected face
                            SelectedFacesMap[faceName] = &meshInst->GetDSMesh();
                            GFrtCtx->MainWindow->statusBar()->showMessage(
                                QString::fromStdString("Selected " + faceName));
                        }
                        else // else, this face has been selected previously
                        {
                            SelectedFaces.erase(position);
                            GFrtCtx->MainWindow->statusBar()->showMessage(
                                QString::fromStdString("Deselected " + faceName));
                        }
                        float selected_dist = round(dist * 100);

                        // mark all those that share the same location
                        //Aaron's fix, we only mark one face at a time

                        meshInst->MarkFaceAsSelected({ faceName }, true, InputSharpness());
                        /* for (int i = 0; i < hits.size(); i++)
                        {
                            const auto& [dist, meshInst, overlapfaceName] = hits[i];
                            if (round(dist * 100) == selected_dist)
                            {
                                meshInst->MarkFaceAsSelected({ overlapfaceName }, true,
                                                             InputSharpness());
                            }
                        } */

                    }
                    dialog->close();
                });
        auto* btnCancel = new QPushButton();
        btnCancel->setText("Cancel");
        connect(btnCancel, &QPushButton::clicked, dialog, &QWidget::close);
        layout2->addWidget(btnOk);
        layout2->addWidget(btnCancel);
        layout1->addLayout(layout2);
        dialog->show();
    }
    else
    {
        GFrtCtx->MainWindow->statusBar()->showMessage("No face hit.");
    }
}

// Used for picking edges
void CNome3DView::PickEdgeWorldRay(tc::Ray& ray)
{
    rotateRay(ray);
    std::vector<std::tuple<float, Scene::CMeshInstance*, std::vector<std::string>>> hits;
    Scene->ForEachSceneTreeNode(
        [&](Scene::CSceneTreeNode* node)
        {
            // Obtain either an instance entity or a shared entity from the scene node
            auto* entity = node->GetInstanceEntity();
            if (!entity)
                entity = node->GetOwner()->GetEntity();
            if (entity)
            {
                if (!entity->isMerged && entity->IsMesh())
                {
                    const auto& l2w = node->L2WTransform.GetValue(tc::Matrix3x4::IDENTITY);
                    auto localRay = ray.Transformed(l2w.Inverse());
                    localRay.Direction =
                        localRay.Direction
                            .Normalized(); // Normalize to fix "scale" error caused by l2w.Inverse()

                    auto* meshInst = dynamic_cast<Scene::CMeshInstance*>(entity);
                    auto pickResults = meshInst->PickEdges(localRay);

                    for (const auto& [dist, names] : pickResults)
                        hits.emplace_back(dist, meshInst, names);
                }
            }
        });
    std::sort(hits.begin(), hits.end());

    // TODO 11/21, If it contains a temp SELECT EDGE polyline, then immediately return that one
    std::vector<std::tuple<float, Scene::CMeshInstance*, std::vector<std::string>>> temp;
    for (const auto& hit : hits)
    {
        auto [dist, meshInst, edgeVertNames] = hit;
        if (edgeVertNames[0].find("SELECTED") != std::string::npos)
        {
            temp.push_back(hit);
        }
    }

    if (!temp.empty())
        hits = temp;
    if (hits.size() == 1)
    {
        const auto& [dist, meshInst, edgeVertNames] =
            hits[0]; // where the edgeVertNames is defined to a vector of two vertex names
        auto position1 =
            std::find(SelectedEdgeVertices.begin(), SelectedEdgeVertices.end(), edgeVertNames[0]);
        auto position2 =
            std::find(SelectedEdgeVertices.begin(), SelectedEdgeVertices.end(), edgeVertNames[1]);
        SelectedEdgeVertices.push_back(edgeVertNames[0]);
        SelectedEdgeVertices.push_back(edgeVertNames[1]);

        // if the selected edge is not a SELECTED (temp) edge
        if (edgeVertNames[0].find("SELECTED") == std::string::npos)
            GFrtCtx->MainWindow->statusBar()->showMessage(QString::fromStdString(
                "Selected " + edgeVertNames[0] + edgeVertNames[1] + " edge"));
        else
        {
            GFrtCtx->MainWindow->statusBar()->showMessage("Deselected edge");
        }
        std::set<std::string> edgeVertNamesSet(edgeVertNames.begin(), edgeVertNames.end());
        meshInst->MarkEdgeAsSelected(edgeVertNamesSet, true, InputSharpness()); // here
    }
    // If you need to implement multiple edge intersection, please see the below line at:
    // https://github.com/randyfan/NOME3/commit/55ab6d81140d09f1725e261ed810c1a15646ab5c
    else
        GFrtCtx->MainWindow->statusBar()->showMessage(
            "No edge hit or more than one edge hit. Please select again");
}

// Used for picking edges
void CNome3DView::PickPolylineWorldRay(tc::Ray& ray)
{
    rotateRay(ray);
    std::vector<std::tuple<float, Scene::CMeshInstance*, std::vector<std::string>>>
        hits; // note the string is a vector of strings
    Scene->ForEachSceneTreeNode(
        [&](Scene::CSceneTreeNode* node)
        {
            std::cout << "Currently in NOME3DView's PickPolylineWorldRay. At node: "
                    + node->GetPath()
                      << std::endl;
            // Obtain either an instance entity or a shared entity from the scene node
            auto* entity = node->GetInstanceEntity();
            if (!entity)
                entity = node->GetOwner()->GetEntity();
            if (entity)
            {
                const auto& l2w = node->L2WTransform.GetValue(tc::Matrix3x4::IDENTITY);
                auto localRay = ray.Transformed(l2w.Inverse());
                localRay.Direction =
                    localRay.Direction
                        .Normalized(); // Normalize to fix "scale" error caused by l2w.Inverse()

                auto* meshInst = dynamic_cast<Scene::CMeshInstance*>(entity);
                auto pickResults = meshInst->PickPolylines(localRay);

                for (const auto& [dist, names] : pickResults)
                    hits.emplace_back(dist, meshInst, names);
            }
        });
    std::sort(hits.begin(), hits.end());

    // TODO 11/21, If it contains a temp SELECT EDGE polyline, then immediately return that one
    std::vector<std::tuple<float, Scene::CMeshInstance*, std::vector<std::string>>> temp;
    for (auto hit : hits)
    {
        auto [dist, meshInst, edgeVertNames] = hit;
        if (edgeVertNames[0].find("SELECTED") != std::string::npos)
        {
            temp.push_back(hit);
        }
    }

    if (!temp.empty())
        hits = temp;
    if (hits.size() == 1)
    {
        const auto& [dist, meshInst, edgeVertNames] =
            hits[0]; // where the edgeVertNames is defined to a vector of two vertex names
        auto position1 =
            std::find(SelectedEdgeVertices.begin(), SelectedEdgeVertices.end(), edgeVertNames[0]);
        auto position2 =
            std::find(SelectedEdgeVertices.begin(), SelectedEdgeVertices.end(), edgeVertNames[1]);
        SelectedEdgeVertices.push_back(edgeVertNames[0]);
        SelectedEdgeVertices.push_back(edgeVertNames[1]);

        // if the selected edge is not a SELECTED (temp) edge
        if (edgeVertNames[0].find("SELECTED") == std::string::npos)
            GFrtCtx->MainWindow->statusBar()->showMessage(QString::fromStdString(
                "Selected " + edgeVertNames[0] + edgeVertNames[1] + " edge"));
        else
        {
            GFrtCtx->MainWindow->statusBar()->showMessage("Deselected edge");
        }
        std::set<std::string> edgeVertNamesSet(edgeVertNames.begin(), edgeVertNames.end());
        meshInst->MarkEdgeAsSelected(edgeVertNamesSet, true, InputSharpness()); // here
    }
    // If you need to implement multiple edge intersection, please see the below line at:
    // https://github.com/randyfan/NOME3/commit/55ab6d81140d09f1725e261ed810c1a15646ab5c
    else
        GFrtCtx->MainWindow->statusBar()->showMessage(
            "No edge hit or more than one edge hit. Please select again");
}

void CNome3DView::PickVertexWorldRay(tc::Ray& ray, bool sharpSelection)
{
    rotateRay(ray);
    std::vector<std::tuple<float, Scene::CMeshInstance*, std::string>> hits;
    Scene->ForEachSceneTreeNode(
        [&](Scene::CSceneTreeNode* node)
        {
            // Obtain either an instance entity or a shared entity from the scene node
            auto* entity = node->GetInstanceEntity();
            if (!entity)
                entity = node->GetOwner()->GetEntity();
            if (entity)
            {
                if (!entity->isMerged && entity->IsMesh())
                {
                    const auto& l2w = node->L2WTransform.GetValue(tc::Matrix3x4::IDENTITY);
                    auto localRay = ray.Transformed(l2w.Inverse());
                    localRay.Direction =
                        localRay.Direction
                            .Normalized(); // Normalize to fix "scale" error caused by l2w.Inverse()
                    auto* meshInst = dynamic_cast<Scene::CMeshInstance*>(entity);
                    auto pickResults = meshInst->PickVertices(localRay);
                    for (const auto& [dist, name] : pickResults)
                        hits.emplace_back(dist, meshInst, name);
                }
            }
        });

    std::sort(hits.begin(), hits.end());

    if (hits.size() == 1)
    {
        mousePressEnabled =
            false; // Randy added this to fix annoying Windows bug where mousePressEvent is not
                   // triggered when clicking on a pop-up NOME window (e.g., sharpness)
        const auto& [dist, meshInst, vertName] = hits[0];
        auto position = std::find(SelectedVertices.begin(), SelectedVertices.end(), vertName);
        if (position == SelectedVertices.end()) // if this vertex has not been selected before
        {
            SelectedVertices.push_back(vertName); // add vertex to selected vertices
            GFrtCtx->MainWindow->statusBar()->showMessage(
                QString::fromStdString("Selected " + vertName));
        }
        else // else, this vertex has been selected previously
        {
            SelectedVertices.erase(position);
            GFrtCtx->MainWindow->statusBar()->showMessage(
                QString::fromStdString("Deselected " + vertName));

        }
        if (sharpSelection)
        {
            meshInst->MarkVertAsSelected({ vertName }, InputSharpness());
        }
        else
        {
            meshInst->MarkVertAsSelected({ vertName }, 0);
        }
    }
    else if (hits.size() > 1)
    {
        mousePressEnabled =
            false; // Randy added this to fix annoying Windows bug where mousePressEvent is not
                   // triggered when clicking on a pop-up NOME window (e.g., sharpness)
        // Show a dialog for the user to choose one vertex
        auto* dialog = new QDialog(GFrtCtx->MainWindow);
        dialog->setModal(true);
        auto* layout1 = new QHBoxLayout(dialog);
        auto* table = new QTableWidget();
        table->setRowCount(hits.size());
        table->setColumnCount(2);
        QStringList titles;
        titles.append(QString::fromStdString("Closeness Rank"));
        titles.append(QString::fromStdString("Vertex Name"));
        table->setHorizontalHeaderLabels(titles);
        int closenessRank = 1;
        for (size_t i = 0; i < hits.size(); i++)
        {
            const auto& [dist, meshInst, vertName] = hits[i];
            if (i != 0)
            {
                const auto& [prevDist, prevMeshInst, prevVertName] = hits[i - 1];
                if (round(dist * 100) != round(prevDist * 100))
                {
                    closenessRank += 1;
                }
                // else, closenessRank stay the same as prev as the distance is the same (vertices
                // in same location)
            }

            auto* distWidget = new QTableWidgetItem(QString::number(closenessRank));
            auto* item = new QTableWidgetItem(QString::fromStdString(vertName));
            table->setItem(i, 0, distWidget); // i is row num, and 0 is col num
            table->setItem(i, 1, item);
        }
        layout1->addWidget(table);
        auto* layout2 = new QVBoxLayout();
        auto* btnOk = new QPushButton();
        btnOk->setText("OK");
        connect(btnOk, &QPushButton::clicked,
                [this, dialog, table, hits, sharpSelection]()
                {
                    auto sel = table->selectedItems();
                    if (!sel.empty())
                    {
                        int rowNum = sel[0]->row();
                        const auto& [dist, meshInst, vertName] = hits[rowNum];
                        std::vector<std::string>::iterator position =
                            std::find(SelectedVertices.begin(), SelectedVertices.end(), vertName);
                        if (position == SelectedVertices.end())
                        { // if this vertex has not been selected before
                            SelectedVertices.push_back(vertName); // add vertex to selected vertices
                            if (sharpSelection)
                            {
                                meshInst->MarkVertAsSelected({ vertName }, InputSharpness());
                            }
                            else
                            {
                                meshInst->MarkVertAsSelected({ vertName }, 0);
                            }
                            GFrtCtx->MainWindow->statusBar()->showMessage(
                                QString::fromStdString("Selected " + vertName));
                        }
                        else // else, this vertex has been selected previously, so we deselect it
                        {
                            SelectedVertices.erase(position);
                            GFrtCtx->MainWindow->statusBar()->showMessage(
                                QString::fromStdString("Deselected " + vertName));
                        }

                        float selected_dist = round(dist * 100);

                        // mark all those that share the same location
                        for (int i = 0; i < hits.size(); i++)
                        {
                            if (i != rowNum)
                            {
                                const auto& [dist, meshInst, overlapvertName] = hits[i];
                                if (round(dist * 100) == selected_dist)
                                {
                                    std::cout << "mark overlap vert: " << overlapvertName
                                              << std::endl;
                                    meshInst->MarkVertAsSelected(
                                        { overlapvertName },
                                        1); // TODO: Fix this -1 sharpness logic. Should
                                             // overlapping vertices have the same sharpness?
                                }
                            }
                        }
                    }
                    dialog->close();
                });
        auto* btnCancel = new QPushButton();
        btnCancel->setText("Cancel");
        connect(btnCancel, &QPushButton::clicked, dialog, &QWidget::close);
        layout2->addWidget(btnOk);
        layout2->addWidget(btnCancel);
        layout1->addLayout(layout2);
        dialog->show();
    }
    else
    {
        GFrtCtx->MainWindow->statusBar()->showMessage("No point hit.");
    }
}
// typedef struct
//{
//    double x, y, z;
//} XYZ;

/*
   Calculate the line segment PaPb that is the shortest route between
   two lines P1P2 and P3P4. Calculate also the values of mua and mub where
      Pa = P1 + mua (P2 - P1)
      Pb = P3 + mub (P4 - P3)
   Return FALSE if no solution exists.
*/
int LineLineIntersect(tc::Vector3 p1, tc::Vector3 p2, tc::Vector3 p3, tc::Vector3 p4,
                      tc::Vector3* pa, tc::Vector3* pb, double* mua, double* mub)
{
    tc::Vector3 p13, p43, p21;
    double d1343, d4321, d1321, d4343, d2121;
    double numer, denom;
    float EPS = 0.000001f;
    p13.x = p1.x - p3.x;
    p13.y = p1.y - p3.y;
    p13.z = p1.z - p3.z;
    p43.x = p4.x - p3.x;
    p43.y = p4.y - p3.y;
    p43.z = p4.z - p3.z;
    if (abs(p43.x) < EPS && abs(p43.y) < EPS && abs(p43.z) < EPS)
    {
        std::cout << "LineLine does not intersect 43: " << abs(p43.x) << " " << abs(p43.x) << " "
                  << abs(p43.z) << std::endl;
        return (false);
    }
    p21.x = p2.x - p1.x;
    p21.y = p2.y - p1.y;
    p21.z = p2.z - p1.z;
    if (abs(p21.x) < EPS && abs(p21.y) < EPS && abs(p21.z) < EPS)
    {
        std::cout << "LineLine does not intersect 21: " << abs(p21.x) << " " << abs(p21.x) << " "
                  << abs(p21.z) << std::endl;
        return (false);
    }

    d1343 = p13.x * p43.x + p13.y * p43.y + p13.z * p43.z;
    d4321 = p43.x * p21.x + p43.y * p21.y + p43.z * p21.z;
    d1321 = p13.x * p21.x + p13.y * p21.y + p13.z * p21.z;
    d4343 = p43.x * p43.x + p43.y * p43.y + p43.z * p43.z;
    d2121 = p21.x * p21.x + p21.y * p21.y + p21.z * p21.z;

    denom = d2121 * d4343 - d4321 * d4321;
    if (abs(denom) < EPS)
    {

        std::cout << "LineLine does not intersect denom: " << abs(denom) << " " << d2121 * d4343
                  << "-" << d4321 * d4321 << "  " << d2121 << " " << d4343 << " " << d4321 << " "
                  << d4321 << std::endl;
        return (false);
    }
    numer = d1343 * d4321 - d1321 * d4343;

    *mua = numer / denom;
    *mub = (d1343 + d4321 * (*mua)) / d4343;

    pa->x = p1.x + *mua * p21.x;
    pa->y = p1.y + *mua * p21.y;
    pa->z = p1.z + *mua * p21.z;
    pb->x = p3.x + *mub * p43.x;
    pb->y = p3.y + *mub * p43.y;
    pb->z = p3.z + *mub * p43.z;

    return (true);
}

// Creates ray OR adds hit point on ray
void CNome3DView::RenderRay(tc::Ray& ray, QVector3D intersection)
{
    rotateRay(ray);

    if (RayVertPositions.size() == 0) // Create Ray Mode
    {
        std::vector<std::tuple<float, Scene::CMeshInstance*, tc::Vector3>> hits;
        Scene->ForEachSceneTreeNode(
            [&](Scene::CSceneTreeNode* node)
            {
                // Obtain either an instance entity or a shared entity from the scene node
                auto* entity = node->GetInstanceEntity();
                if (!entity)
                    entity = node->GetOwner()->GetEntity();
                if (entity)
                {
                    if (!entity->isMerged && entity->IsMesh())
                    {
                        const auto& l2w = node->L2WTransform.GetValue(tc::Matrix3x4::IDENTITY);
                        auto localRay = ray.Transformed(l2w.Inverse());
                        localRay.Direction =
                            localRay.Direction.Normalized(); // Normalize to fix "scale" error
                                                             // caused by l2w.Inverse()
                        auto* meshInst = dynamic_cast<Scene::CMeshInstance*>(entity);
                        auto pickResults = meshInst->GetHitPoint(localRay);
                        for (const auto& [dist, hitPoint] : pickResults)
                        {
                            auto hitPointRotated = l2w * hitPoint; // transform(hitPoint, l2w);
                            std::cout
                                << hitPointRotated.x << " " << hitPointRotated.y << " "
                                << hitPointRotated.z
                                << "RenderRay should be like current Ray but with the current "
                                   "intersection point"
                                << std::endl;
                            hits.emplace_back(dist, meshInst, hitPointRotated);
                        }
                    }
                }
            });
        std::cout << "hitPoint size: " << hits.size() << std::endl;
        std::sort(hits.begin(), hits.end());

        tc::Vector3 closestHitPoint;
        if (hits.size() > 0)
            closestHitPoint = std::get<2>(hits[0]); // get the closest hitPointRotated
        else
            return;

        RayVertPositions.push_back(ray.Origin);
        QVector3D test = { closestHitPoint.x, closestHitPoint.y, closestHitPoint.z };
        RayVertPositions.push_back(closestHitPoint);
        RayCasted = true;
    }
    else // Selection Mode
    {
        std::cout << "initial castRay startPoint " << RayVertPositions[0].x << " "
                  << RayVertPositions[0].y << " " << RayVertPositions[0].z
                  << "   initial castRay endpoint " << RayVertPositions[1].x << " "
                  << RayVertPositions[1].y << " " << RayVertPositions[1].z << std::endl;

        std::cout << "newly cast ray origin " << ray.Origin.x << " " << ray.Origin.y << " "
                  << ray.Origin.z << "   newly cast ray Dir" << ray.Direction.x << " "
                  << ray.Direction.y << " " << ray.Direction.z << std::endl;

        // tc::Vector3 closestPoint = castRay.ClosestPoint(ray);  // ray.ClosestPoint(castRay);  ///
        // Return closest point to another ray.
        tc::Vector3 closestPointOnInitialRay;
        tc::Vector3 closestPointOnNewRay;
        double mua;
        double mub;

        auto interBool = LineLineIntersect(
            RayVertPositions[0], RayVertPositions[1], ray.Origin, ray.Origin + 5 * ray.Direction,
            &closestPointOnInitialRay, &closestPointOnNewRay, &mua, &mub);

        std::cout << "interBool " << interBool << std::endl;
        if (interBool == 0)
        {
            std::cout << "Warning did not find intersection" << std::endl;
        }
        else
        {
            std::cout << "Found intersection" << std::endl;
        }
        auto closestPoint = closestPointOnInitialRay;
        std::cout << "closestPoint on initial castRay: " << closestPoint.x << " " << closestPoint.y
                  << " " << closestPoint.z << std::endl;
        RayInteractivePoint.push_back(closestPoint); // single interactive point

        RayVertPositions.clear();
        RayVertPositions.push_back(ray.Origin);
        RayVertPositions.push_back(ray.Origin + 5 * ray.Direction); // closestPoint);
        RayCasted = true; // false;

        // Remove the casted ray TODO: Remove the entire node so we can save changes
        Scene->ForEachSceneTreeNode(
            [&](Scene::CSceneTreeNode* node)
            {
                // Obtain either an instance entity or a shared entity from the scene node
                auto* entity = node->GetInstanceEntity();
                if (!entity)
                    entity = node->GetOwner()->GetEntity();
                if (entity)
                {
                    if (!entity->isMerged && entity->IsMesh())
                    {
                        // std::cout << "looking for ray" << std::endl;
                        if (entity->GetName().find("TempInteractivePoly") != std::string::npos)
                        {
                            // std::cout << "found ray, remove" << std::endl;
                            // node->GetOwner()->SetEntity(nullptr);
                        }
                    }
                }
            });
    }
}

// Xinyu add on Oct 8 for rotation
void CNome3DView::mousePressEvent(QMouseEvent* e)
{
    // material->setAlpha(0.7f);

    // Clicking with left button enables rotation
    rotationEnabled = e->button() == Qt::RightButton ? false : true;
    // Save mouse press position
    firstPosition = QVector2D(e->localPos());
    mousePressEnabled =
        true; // TODO: this is the Windowsbug. It's not being turned off when we click on a vertex.
}

void CNome3DView::mouseMoveEvent(QMouseEvent* e)
{
    if (mousePressEnabled)
    {
        // Mouse release position - mouse press position
        secondPosition = QVector2D(e->localPos());
        QVector2D diff = secondPosition - firstPosition;
        if (!rotationEnabled)
        {
            objectX = diff.x() / 100 + objectX;
            objectY = -diff.y() / 100 + objectY;
            sphereTransform->setTranslation(QVector3D(objectX, objectY, objectZ));
        }
        else
        {
            QVector2D firstPoint = GetProjectionPoint(firstPosition);
            QVector2D secondPoint = GetProjectionPoint(secondPosition);
            double projectedRadius = sqrt(qPow(zPos, 2) - 1) / zPos;

            if (firstPoint.length() > projectedRadius || secondPoint.length() > projectedRadius)
            {
                float angle = qRadiansToDegrees(
                    qAsin(QVector3D::crossProduct(QVector3D(firstPoint, 0).normalized(),
                                                  QVector3D(secondPoint, 0).normalized())
                              .z()));
                rotation = QQuaternion::fromAxisAndAngle(0, 0, 1, angle) * rotation;
            }
            else
            {
                QVector3D firstCrystalPoint = GetCrystalPoint(firstPoint);
                QVector3D secondCrystalPoint = GetCrystalPoint(secondPoint);
                QVector3D axis =
                    QVector3D::crossProduct(firstCrystalPoint, secondCrystalPoint).normalized();
                /* Added by Brian, 06/15/2023. Change the viewing axis if the camera is rotated.*/
                QMatrix4x4 qtf{this->camerarotation.ToMatrix4().Data()};
                QVector4D trans = qtf * QVector4D(axis.x(), axis.y(), axis.z(), 0);
                QVector3D objectinput = trans.toVector3D(); 
                float distance = firstCrystalPoint.distanceToPoint(secondCrystalPoint);
                rotation =
                    QQuaternion::fromAxisAndAngle(objectinput, qRadiansToDegrees(2 * asin(distance / 2)))
                    * rotation;
            }
        }
        sphereTransform->setRotation(rotation);

        firstPosition = secondPosition;
    }
}

void CNome3DView::mouseReleaseEvent(QMouseEvent* e)
{
    // material->setAlpha(0.0f);
    mousePressEnabled = false;
    rotationEnabled = true;
}

void CNome3DView::wheelEvent(QWheelEvent* ev)
{
    if (rotationEnabled)
    {
        QPoint numPixels = ev->pixelDelta();
        QPoint numDegrees = ev->angleDelta() / 13.0f;

        if (!numPixels.isNull())
        {
            objectZ += numPixels.y() * 0.15;
        }
        else if (!numDegrees.isNull())
        {
            QPoint numSteps = numDegrees / 15.0;
            objectZ += numSteps.y() * 0.15;
        }
        if (objectZ > 30)
            objectZ = 30;
        /* Added by Brian, 11/24/2023, Wheelevent should adjust left, right, bottom, top cutoff fields in order to zoom in with Parallel Projection */ 
        if (InteractiveCameras.size() >= 1) {
            CInteractiveCamera* camera = *(InteractiveCameras.begin()); 
            if (camera->GetCameraType() == "PARALLEL") {
                camera->CameraScroll(numDegrees.y()*0.15);
                return; 
            }
        }
        /* Added by Brian, 06/15/2023, wheelEvent axis should be translated via camera's transformation matrix. */
        QMatrix4x4 qtf{this->camerarotation.ToMatrix4().Data()};
        QVector4D trans = qtf * QVector4D(objectX, objectY, objectZ, 0);
        QVector3D objectinput = trans.toVector3D(); 
        sphereTransform->setTranslation(objectinput);
        //sphereTransform->setTranslation(QVector3D(objectX, objectY, objectZ)); Old Code (Does not rotate camera)
        ev->accept();
    }
}

void CNome3DView::keyPressEvent(QKeyEvent* ev)
{
    switch (ev->key())
    {
    case Qt::Key_Tab:
        // material->setAlpha(rotationEnabled * 0.1);

        break;
    case Qt::Key_Shift:
        vertexSelectionEnabled = true;
        break;
    case Qt::Key_Space:
        if (animationEnabled)
        {
            sphereRotateTransformAnimation->pause();
        }
        else
        {
            sphereRotateTransformAnimation->start();
        }
        animationEnabled = !animationEnabled;
        break;
    }
}

bool CNome3DView::eventFilter(QObject* obj, QEvent* event)
{
    if (event->type() == QEvent::KeyRelease)
    {
        QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
        if (keyEvent->key() == Qt::Key_Shift)
        {
            return true;
        }
        else
            return false;
    }
    return false;
}

QVector2D CNome3DView::GetProjectionPoint(QVector2D originalPosition)
{
    double xRatio = (originalPosition.x() - this->width() / 2.0) / (this->width() / 2.0);
    double yRatio = (this->height() / 2.0 - originalPosition.y()) / (this->height() / 2.0);
    // Calculate the x ratio according to the screen ratio
    double tempX = xRatio * this->width() / this->height();
    // Calculate the equivalent y by the radius
    double tempY = sqrt(qPow(tempX, 2) + qPow(yRatio, 2));
    // Calculate the camera view angle according to the picked point
    double theta = qAtan(tempY * qTan(qDegreesToRadians(45.0 / 2.0)));

    double temp = 1 + qPow(qTan(theta), 2);
    double judge = (1 - qPow(zPos, 2)) / temp + qPow(zPos / temp, 2);

    double projectedHeight = (judge > 0 ? (zPos / temp - qSqrt(judge)) : (zPos - 1 / zPos))
        * qTan(qDegreesToRadians(45.0 / 2.0));

    double projectedWidth = projectedHeight * this->width() / this->height();

    return QVector2D(xRatio * projectedWidth, yRatio * projectedHeight);
}

QVector3D CNome3DView::GetCrystalPoint(QVector2D originalPoint)
{
    double z = sqrt(1 - qPow(originalPoint.x(), 2) - qPow(originalPoint.y(), 2));
    return QVector3D(originalPoint, z);
}

void CNome3DView::rotateRay(tc::Ray& ray)
{
    QVector3D origin =
        QVector3D(ray.Origin.x, ray.Origin.y, ray.Origin.z) - QVector3D(objectX, objectY, objectZ);
    ;
    origin = rotation.inverted().rotatedVector(origin);
    QVector3D direction = rotation.inverted().rotatedVector(
        QVector3D(ray.Direction.x, ray.Direction.y, ray.Direction.z));

    ray.Direction = tc::Vector3(direction.x(), direction.y(), direction.z());
    ray.Origin = tc::Vector3(origin.x(), origin.y(), origin.z());
}
void CNome3DView::FreeVertexSelection() { vertexSelectionEnabled = false; }
float CNome3DView::InputSharpness()
{
    bool ok;

    float sharpness =
        QInputDialog::getDouble(GFrtCtx->MainWindow, tr("Set the sharpness"), tr("Sharpness:"), 0,
                                0, 10, 1, &ok, Qt::WindowFlags(), 1);
    if (ok)
        return sharpness;
    else
        return -1;
}
}