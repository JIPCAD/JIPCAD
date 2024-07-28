#include "ASTSceneAdapter.h"
#include "BSpline.h"
#include "Window.h"
#include "BezierSpline.h"
#include "Camera.h"
#include "Circle.h"
#include "Cylinder.h"
#include "Dupin.h"
#include "Ellipsoid.h"
#include "Environment.h"
#include "Face.h"
#include "Funnel.h"
#include "GenCartesianSurf.h"
#include "GenImplicitSurf.h"
#include "GenParametricSurf.h"
#include "Helix.h"
#include "Hyperboloid.h"
#include "Light.h"
#include "MeshMerger.h"
#include "MobiusStrip.h"
#include "Point.h"
#include "Polyline.h"
#include "Sphere.h"
#include "Spiral.h"
#include "Surface.h"
#include "Backface.h"
#include "Sweep.h"
#include "SweepMorph.h"
#include "SweepMorphVisualizer.h"
#include "SweepControlPoint.h"
#include "Torus.h"
#include "TorusKnot.h"
#include "Tunnel.h"
#include "Viewport.h"
#include <StringPrintf.h>
#include <unordered_map>

namespace Nome::Scene
{

/*
 * Some notes for when I was looking through Nom.g4 trying to generalize things as much as possible.
 * The only primitive binding types I could find:
 * ident <-> std::string name
 * expression <-> float
 * ident <-> scene object reference
 * sub-command <-> ???
 *
 * Aside from that ident and expressions can also appear inside vectors and have variable length
 * bindings.
 */

static const std::unordered_map<std::string, ECommandKind> CommandInfoMap = {
    { "point", ECommandKind::Entity },
    { "polyline", ECommandKind::Entity },
    { "sweep", ECommandKind::Entity },
    { "sweepmorph", ECommandKind::Entity },
    { "morphvisualizer", ECommandKind::Entity },
    { "controlpoint", ECommandKind::Entity },
    { "face", ECommandKind::Entity },
    { "object", ECommandKind::Entity },
    { "mesh", ECommandKind::Entity },
    { "group", ECommandKind::Instance },
    { "merge", ECommandKind::Instance },
    { "subdivide", ECommandKind::Instance },
    { "circle", ECommandKind::Entity },
    { "sphere", ECommandKind::Entity },
    { "cylinder", ECommandKind::Entity },
    { "funnel", ECommandKind::Entity },
    { "hyperboloid", ECommandKind::Entity },
    { "dupin", ECommandKind::Entity },
    { "tunnel", ECommandKind::Entity },
    { "beziercurve", ECommandKind::Entity },
    { "torusknot", ECommandKind::Entity },
    { "torus", ECommandKind::Entity },
    { "bspline", ECommandKind::Entity },
    { "instance", ECommandKind::Instance },
    { "surface", ECommandKind::Entity },
    { "backface", ECommandKind::Entity },
    { "background", ECommandKind::Entity },
    { "foreground", ECommandKind::Dummy },
    { "insidefaces", ECommandKind::Dummy },
    { "outsidefaces", ECommandKind::Dummy },
    { "offsetfaces", ECommandKind::Dummy },
    { "frontfaces", ECommandKind::Dummy },
    { "backfaces", ECommandKind::Dummy },
    { "rimfaces", ECommandKind::Dummy },
    { "bank", ECommandKind::BankSet },
    { "set", ECommandKind::BankSet },
    { "list", ECommandKind::BankSet},
    { "delete", ECommandKind::Instance },
    { "subdivision", ECommandKind::Instance },
    {"frontcolor", ECommandKind::Instance},
    {"backcolor", ECommandKind::Instance},
    {"backgroundcolor", ECommandKind::Instance},
    { "offset", ECommandKind::Instance },
    { "mobiusstrip", ECommandKind::Entity },
    { "helix", ECommandKind::Entity },
    { "ellipsoid", ECommandKind::Entity },
    { "include", ECommandKind::DocEdit },
    { "spiral", ECommandKind::Entity },
    { "sharp", ECommandKind::Entity },
    { "camera", ECommandKind::Entity },
    { "light", ECommandKind::Entity },
    { "viewport", ECommandKind::Entity },
    { "gencartesiansurf", ECommandKind::Entity },
    { "genparametricsurf", ECommandKind::Entity },
    { "genimplicitsurf", ECommandKind::Entity }

};

ECommandKind CASTSceneAdapter::ClassifyCommand(const std::string& cmd)
{
    return CommandInfoMap.at(cmd);
}

CEntity* CASTSceneAdapter::MakeEntity(const std::string& cmd, const std::string& name)
{
    if (cmd == "beziercurve")
        return new CBezierSpline(name);
    else if (cmd == "bspline")
        return new CBSpline(name);
    else if (cmd == "circle")
        return new CCircle(name);
    else if (cmd == "cylinder")
        return new CCylinder(name);
    else if (cmd == "ellipsoid")
        return new CEllipsoid(name);
    else if (cmd == "face")
        return new CFace(name);
    else if (cmd == "funnel")
        return new CFunnel(name);
    else if (cmd == "mesh" || cmd == "object")
        return new CMesh(name);
    else if (cmd == "point")
        return new CPoint(name);
    else if (cmd == "polyline")
        return new CPolyline(name);
    else if (cmd == "helix")
        return new CHelix(name);
    else if (cmd == "sphere")
        return new CSphere(name);
    else if (cmd == "sweep")
        return new CSweep(name);
    else if (cmd == "sweepmorph")
        return new CSweepMorph(name);
    else if (cmd == "morphvisualizer")
        return new CSweepMorphVisualizer(name);
    else if (cmd == "controlpoint")
        return new CSweepControlPoint(name);
    else if (cmd == "surface")
        return new CSurface(name);
    else if (cmd == "backface")
        return new CBackface(name);
    else if (cmd == "tunnel")
        return new CTunnel(name);
    else if (cmd == "torusknot")
        return new CTorusKnot(name);
    else if (cmd == "torus")
        return new CTorus(name);
    else if (cmd == "mobiusstrip")
        return new CMobiusStrip(name);
    else if (cmd == "hyperboloid")
        return new CHyperboloid(name);
    else if (cmd == "dupin")
        return new CDupin(name);
    else if (cmd == "spiral")
        return new SSpiral(name);
    else if (cmd == "light")
        return new CLight(name);
    else if (cmd == "window")
        return new CWindow(name);
    else if (cmd == "camera")
        return new CCamera(name);
    else if (cmd == "viewport")
        return new CViewport(name);

    return nullptr;
}

// Randy changed on 11/30. TraverseFile returns list of additional file names that need to be parsed
std::vector<std::string> CASTSceneAdapter::GetIncludes(AST::AFile* astRoot, CScene& scene)
{

    assert(CmdTraverseStack.empty());

    std::vector<std::string> includeFileNames;
    for (auto* cmd : astRoot->GetCommands())
    {
        auto fileName = VisitInclude(cmd, scene);
        if (fileName != "")
            includeFileNames.push_back(fileName);
    }
    return includeFileNames;
}
// Randy changed on 11/30. TraverseFile returns list of additional file names that need to be parsed
void CASTSceneAdapter::TraverseFile(AST::AFile* astRoot, CScene& scene)
{
    assert(CmdTraverseStack.empty());

    for (auto* cmd : astRoot->GetCommands())
    {
        VisitCommandBankSet(cmd, scene);
    }
    InstanciateUnder = GEnv.Scene->GetRootNode();
    for (auto* cmd : astRoot->GetCommands())
        VisitCommandSyncScene(cmd, scene, false);
}

std::string CASTSceneAdapter::VisitInclude(AST::ACommand* cmd, CScene& scene)
{
    std::string includeFileName;
    CmdTraverseStack.push_back(cmd);
    auto kind = ClassifyCommand(cmd->GetCommand());
    if (kind == ECommandKind::DocEdit && cmd->GetCommand() == "include")
    {
        auto name = cmd->GetName();
        includeFileName = name;
    }
    CmdTraverseStack.pop_back();
    return includeFileName;
}

void CASTSceneAdapter::VisitCommandBankSet(AST::ACommand* cmd, CScene& scene)
{
    CmdTraverseStack.push_back(cmd);
    auto kind = ClassifyCommand(cmd->GetCommand());
    if (kind == ECommandKind::BankSet && cmd->GetCommand() == "set")
    {
        if (CmdTraverseStack.rbegin()[1]->GetCommand() != "bank")
            throw AST::CSemanticError("Set command is not under bank", cmd);
        auto bank = CmdTraverseStack.rbegin()[1]->GetName();
        // You could generalize this somehow
        auto evalArg = [cmd](size_t index) {
            auto* expr = cmd->GetPositionalArgument(index);
            CExprEvalDirect eval;
            auto result = expr->Accept(&eval);
            return std::any_cast<float>(result);
        };
        auto name = bank + "." + cmd->GetName();
        scene.GetBankAndSet().AddSlider(name, cmd, evalArg(1), evalArg(2), evalArg(3), evalArg(4));
    }
    if (kind == ECommandKind::BankSet && cmd->GetCommand() == "list")
    {
        if (CmdTraverseStack.rbegin()[1]->GetCommand() != "bank")
            throw AST::CSemanticError("List command is not under bank", cmd);
        auto bank = CmdTraverseStack.rbegin()[1]->GetName();
        auto bankList = (static_cast<AST::AVector*>(cmd->GetPositionalArgument(0)))->GetItems();
        for (auto expr : bankList)
        {
            auto name = static_cast<AST::AIdent*>(expr)->ToString();
            scene.GetBankAndSet().AddToSliderList(bank + "." + name);
        }
    }

    for (auto* sub : cmd->GetSubCommands())
        VisitCommandBankSet(sub, scene);
    CmdTraverseStack.pop_back();
}

void CASTSceneAdapter::IterateSharpness(AST::ACommand* cmd, CScene& scene) const
{
    TAutoPtr<CEntity> entity = new CSharp();
    entity->GetMetaObject().DeserializeFromAST(*cmd, *entity);
    GEnv.Scene->AddEntity(entity);

    if (auto* mesh = dynamic_cast<CMesh*>(ParentEntity))
        if (auto* points = dynamic_cast<CSharp*>(entity.Get()))
            mesh->SharpPoints.Connect(points->SharpPoints);
}

void CASTSceneAdapter::VisitCommandSyncScene(AST::ACommand* cmd, CScene& scene, bool insubMesh)
{
    CmdTraverseStack.push_back(cmd);
    auto kind = ClassifyCommand(cmd->GetCommand());
    //printf("%s: %d\n", cmd->GetCommand().c_str(), kind);
    if (kind == ECommandKind::Dummy)
    {
        printf("Warning: %s command unrecognized.\n", cmd->GetCommand().c_str());
    }
    else if (kind == ECommandKind::Entity)
    {
        if (cmd->GetCommand() == "sharp")
        {
            for (auto* sub : cmd->GetSubCommands())
            {
                sub->PushPositionalArgument(cmd->GetLevel());
                IterateSharpness(sub, scene);
            }
        }
        else
        {
            TAutoPtr<CEntity> entity;
            if (cmd->GetCommand() == "gencartesiansurf")
            {
                auto func = cmd->GetNamedArgument("func")->GetArgument(
                    0)[0]; // Returns a casted AExpr that was an AIdent before casting
                std::string funcIdentifier =
                    static_cast<AST::AIdent*>(&func)->ToString(); // Downcast it back to an AIdent
                entity = new CGenCartesianSurf(EntityNamePrefix + cmd->GetName(), funcIdentifier);
            }
            else if (cmd->GetCommand() == "genparametricsurf")
            {
                auto funcX = cmd->GetNamedArgument("funcX")->GetArgument(
                    0)[0]; // Returns a casted AExpr that was an AIdent before casting
                std::string funcIdentifierX =
                    static_cast<AST::AIdent*>(&funcX)->ToString(); // Downcast it back to an AIdent
                auto funcY = cmd->GetNamedArgument("funcY")->GetArgument(
                    0)[0]; // Returns a casted AExpr that was an AIdent before casting
                std::string funcIdentifierY =
                    static_cast<AST::AIdent*>(&funcY)->ToString(); // Downcast it back to an AIdent
                auto funcZ = cmd->GetNamedArgument("funcZ")->GetArgument(
                    0)[0]; // Returns a casted AExpr that was an AIdent before casting
                std::string funcIdentifierZ =
                    static_cast<AST::AIdent*>(&funcZ)->ToString(); // Downcast it back to an AIdent
                entity = new CGenParametricSurf(EntityNamePrefix + cmd->GetName(), funcIdentifierX, funcIdentifierY, funcIdentifierZ);
            }
            else if (cmd->GetCommand() == "genimplicitsurf")
            {
                auto func = cmd->GetNamedArgument("func")->GetArgument(0)[0]; // Returns a casted AExpr that was an AIdent before casting
                std::string funcIdentifier = static_cast<AST::AIdent*>(&func)->ToString(); // Downcast it back to an AIdent
                entity = new CGenImplicitSurf(EntityNamePrefix + cmd->GetName(), funcIdentifier);
            }
            else
            {
                entity = MakeEntity(cmd->GetCommand(), EntityNamePrefix + cmd->GetName());
            }

            entity->GetMetaObject().DeserializeFromAST(*cmd, *entity);
            if (auto* light = dynamic_cast<CLight*>(entity.Get())) {
                auto* typeinfo = cmd->GetNamedArgument("type");
                auto* vectorinfo = cmd->GetNamedArgument("color");
                auto* expr = typeinfo->GetArgument(0);
                // Just return if the corresponding element is not found in the AST
                if (!expr)
                    std::cout << "Haven't detected the light type, use ambient light as default" << std::endl;
                else
                    light->GetLight().type = static_cast<const AST::AIdent*>(expr)->ToString();
            }
            if (auto* camera = dynamic_cast<CCamera*>(entity.Get())) {
                auto* typeinfo = cmd->GetNamedArgument("projection");
                auto* expr = typeinfo->GetArgument(0);
                // Just return if the corresponding element is not found in the AST
                if (!expr) {
                    std::cout << "Haven't detected the camera projection type" << std::endl;
                    camera->GetCamera().type = "DEFAULT"; 
                }
                else {
                    camera->GetCamera().type = static_cast<const AST::AIdent*>(expr)->ToString();
                }
            } 
            if (auto* viewport = dynamic_cast<CViewport*>(entity.Get())) {
                auto* cameraId = cmd->GetNamedArgument("cameraID");
                auto* expr = cameraId->GetArgument(0);
                // Just return if the corresponding element is not found in the AST
                if (!expr)
                    std::cout << "Haven't detected the camera input to the viewport" << std::endl;
                else
                    viewport->cameraId = static_cast<const AST::AIdent*>(expr)->ToString();
            }

            // All entities are added to the EntityLibrary dictionary
            GEnv.Scene->AddEntity(entity);
            if (auto* mesh = dynamic_cast<CMesh*>(ParentEntity))
                if (auto* face = dynamic_cast<CFace*>(entity.Get()))
                {
                    mesh->Faces.Connect(face->Face);
                }
                else if (auto* point = dynamic_cast<CPoint*>(entity.Get()))
                {
                    mesh->Points.Connect(point->Point); // Randy added on 12/5 to allow points to be of a mesh
                }

            // Added insubMesh bool to allow Meshes to process multiple subcommands (more than one
            // face) recursively via VisitCommandSyncScene.
            if (insubMesh == false)
            {
                ParentEntity = entity;
                EntityNamePrefix = cmd->GetName() + ".";
            }

            auto subCommands = cmd->GetSubCommands();
            for (size_t i = 0; i < subCommands.size(); i++)
            {
                auto* sub = subCommands[i];
                VisitCommandSyncScene(sub, scene, true);

                // if done visiting mesh, mark it as visited. Randy added this on 12/9
                if (i == subCommands.size() - 1)
                {
                    auto meshNameNoPeriod = EntityNamePrefix.substr(0, EntityNamePrefix.size() - 1);
                    GEnv.Scene->DoneVisitingMesh(meshNameNoPeriod);
                }
            }

            // Added insubMesh bool to allow Meshes to process multiple faces.
            if (insubMesh == false)
            {
                EntityNamePrefix = "";
                ParentEntity = nullptr;
            }
        }
    }
    else if (cmd->GetCommand() == "instance")
    {
        // CreateChildNode() adds a node to the scene graph IF it hasn't been added already, and
        // always adds a node to the scene tree This means ONE sceneNode could correspond to
        // multiple scene tree nodes, which is how we want to represent the scene
        auto* sceneNode = InstanciateUnder->CreateChildNode(cmd->GetName());
        // To perform rotation
        sceneNode->SyncFromAST(cmd, scene);
        // TODO: move the following logic into SyncFromAST

        // Check to see if there is a surface color associated with this instance or group scene
        // node. If the surface argument exists, then set it to be the scene node's surface. Surface
        // color for group vs mesh instance logic is handled in InteractiveMesh.cpp (at the
        // rendering stage).
        auto surface = cmd->GetNamedArgument("surface");
        if (surface)
        {
            auto surfaceEntityNameExpr = surface->GetArgument(
                0)[0]; // Returns a casted AExpr that was an AIdent before casting
            auto surfaceIdentifier = static_cast<AST::AIdent*>(&surfaceEntityNameExpr)
                                         ->ToString(); // Downcast it back to an AIdent
            auto surfaceEntity = GEnv.Scene->FindEntity(surfaceIdentifier);
            if (surfaceEntity)
                sceneNode->SetSurface(dynamic_cast<CSurface*>(surfaceEntity.Get()));
        }
        auto backface = cmd->GetNamedArgument("backface");
        if (backface)
        {
            auto backfaceEntityNameExpr = backface->GetArgument(
                0)[0]; // Returns a casted AExpr that was an AIdent before casting
                auto backfaceIdentifier = static_cast<AST::AIdent*>(&backfaceEntityNameExpr)
                    ->ToString(); // Downcast it back to an AIdent
                    auto backfaceEntity = GEnv.Scene->FindEntity(backfaceIdentifier);
                    if (backfaceEntity)
                        sceneNode->SetBackface(dynamic_cast<CBackface*>(backfaceEntity.Get()));
        }
        auto entityName = cmd->GetPositionalIdentAsString(1);
        auto entity = GEnv.Scene->FindEntity(entityName);

        // <CMeshInstance> entity . check to see if this cast works
        // use the casted object, which is successfully casted as a CMesh instance
        // so now just do dsMesh = entity.GetDSMesh()
        // dsMesh.faces, dsMesh.edgeList,

        if (entity){
            sceneNode->SetEntity(entity); // This line is very important. It attaches an entity
                                          // (e.g. mesh) to the scene node
        }
        else if (auto group = GEnv.Scene->FindGroup(entityName)) { // If the entityName is a group identifier
            group->AddParent(sceneNode);
        }
        else if (GEnv.Scene->ExistMerge(entityName)) { //Merge Section.
            std::pair<TAutoPtr<CSceneNode>, int> merge_obj = GEnv.Scene->FindMerge(entityName);
            auto merge = merge_obj.first;
            if (!merge->GetEntity()) {
                tc::TAutoPtr<Scene::CMeshMerger> merger = new Scene::CMeshMerger(entityName + cmd->GetName());
                merge->SetEntity(merger.Get());
            }
            //std::cout << "num of tree nodes in merge: " << merge->CountTreeNodes() << std::endl;
            merge->AddParent(sceneNode);
            //sceneNode->AddEntity(tc::static_pointer_cast<Scene::CEntity>(merger)); // Merger now has all the vertices set, so we can add it into the scene as a new
        }
        else {
            throw AST::CSemanticError(
                tc::StringPrintf("Instantiation failed, unknown generator: %s", entityName.c_str()),
                cmd);
        }
    }
    else if (cmd->GetCommand() == "group")
    {
        InstanciateUnder = GEnv.Scene->CreateGroup(cmd->GetName());
        InstanciateUnder->SyncFromAST(cmd, scene);
        for (auto* sub : cmd->GetSubCommands())
            VisitCommandSyncScene(sub, scene, false);
        InstanciateUnder = GEnv.Scene->GetRootNode();
    }
    else if (cmd->GetCommand() == "merge") {
        MergeCommandMap[cmd->GetName()] = cmd;
        InstanciateUnder = GEnv.Scene->CreateMerge(cmd->GetName());
        InstanciateUnder->SyncFromAST(cmd, scene);
        for (auto* sub : cmd->GetSubCommands())
            VisitCommandSyncScene(sub, scene, false);
        InstanciateUnder = GEnv.Scene->GetRootNode();
    } else if (cmd->GetCommand() == "subdivide") {
        std::string id = cmd->GetPositionalIdentAsString(0);
        auto* typeinfo = cmd->GetNamedArgument("mergeinstance");
        auto* args = typeinfo->GetArgument(0);
        std::string entityName; 
        if (!args) {
            std::cout << "[Subdivision Error] Subdivision Parsing Error" << std::endl;
        }
        else {
            entityName = static_cast<const AST::AIdent*>(args)->ToString();
        }
        if (GEnv.Scene->ExistMerge(entityName)) {   
            AST::ACommand* savedmergecmd = MergeCommandMap[entityName]; 
            InstanciateUnder = GEnv.Scene->CreateMerge(id);
            InstanciateUnder->SyncFromAST(cmd, scene);
            for (auto* sub : savedmergecmd->GetSubCommands())
                VisitCommandSyncScene(sub, scene, false);
            InstanciateUnder = GEnv.Scene->GetRootNode();
            auto* expr = cmd->GetPositionalArgument(1);
            CExprEvalDirect eval;
            auto items = static_cast<AST::AVector*>(expr)->GetItems();
            auto level = std::any_cast<float>(items.at(0)->Accept(&eval));
            GEnv.Scene->AdjustSubdivisionLevel(id, level); 
        } else {
            std::cout << "[Subdivision Error] objects not merged" << std::endl;

        }  
    } else if (cmd->GetCommand() == "frontcolor") {
        auto* expr = cmd->GetPositionalArgument(0);

        CExprEvalDirect eval;
        // auto result = expr->Accept(&eval);
        // return std::any_cast<float>(result);
        auto items = static_cast<AST::AVector*>(expr)->GetItems();
        auto R = std::any_cast<float>(items.at(0)->Accept(&eval));
        auto G = std::any_cast<float>(items.at(1)->Accept(&eval));
        auto B = std::any_cast<float>(items.at(2)->Accept(&eval));
        GEnv.Scene->frontColor = {R, G, B};
    }
    else if (cmd->GetCommand() == "backcolor") {
        auto* expr = cmd->GetPositionalArgument(0);

        CExprEvalDirect eval;
        // auto result = expr->Accept(&eval);
        // return std::any_cast<float>(result);
        auto items = static_cast<AST::AVector*>(expr)->GetItems();
        auto R = std::any_cast<float>(items.at(0)->Accept(&eval));
        auto G = std::any_cast<float>(items.at(1)->Accept(&eval));
        auto B = std::any_cast<float>(items.at(2)->Accept(&eval));
        GEnv.Scene->backColor = {R, G, B};
    } else if (cmd->GetCommand() == "backgroundcolor") {
        auto* expr = cmd->GetPositionalArgument(0);
        CExprEvalDirect eval;
        // auto result = expr->Accept(&eval);
        // return std::any_cast<float>(result);
        auto items = static_cast<AST::AVector*>(expr)->GetItems();
        auto R = std::any_cast<float>(items.at(0)->Accept(&eval));
        auto G = std::any_cast<float>(items.at(1)->Accept(&eval));
        auto B = std::any_cast<float>(items.at(2)->Accept(&eval));
        GEnv.Scene->backgroundColor = {R, G, B};

    }
    CmdTraverseStack.pop_back();
}

CTransform* CASTSceneAdapter::ConvertASTTransform(AST::ANamedArgument* namedArg)
{
    auto items = static_cast<AST::AVector*>(namedArg->GetArgument(0))->GetItems();
    if (namedArg->GetName() == "translate")
    {
        auto* transform = new CTranslate();
        CExprToNodeGraph c1 { items.at(0), GEnv.Scene->GetBankAndSet(), &transform->X };
        CExprToNodeGraph c2 { items.at(1), GEnv.Scene->GetBankAndSet(), &transform->Y };
        CExprToNodeGraph c3 { items.at(2), GEnv.Scene->GetBankAndSet(), &transform->Z };
        return transform;
    }
    else if (namedArg->GetName() == "rotate")
    {
        auto* transform = new CRotate();
        CExprToNodeGraph c1 { items.at(0), GEnv.Scene->GetBankAndSet(), &transform->AxisX };
        CExprToNodeGraph c2 { items.at(1), GEnv.Scene->GetBankAndSet(), &transform->AxisY };
        CExprToNodeGraph c3 { items.at(2), GEnv.Scene->GetBankAndSet(), &transform->AxisZ };
        auto v2 = static_cast<AST::AVector*>(namedArg->GetArgument(1))->GetItems();
        CExprToNodeGraph c4 { v2.at(0), GEnv.Scene->GetBankAndSet(), &transform->Angle };
        return transform;
    }
    else if (namedArg->GetName() == "scale")
    {
        auto* transform = new CScale();
        CExprToNodeGraph c1 { items.at(0), GEnv.Scene->GetBankAndSet(), &transform->X };
        CExprToNodeGraph c2 { items.at(1), GEnv.Scene->GetBankAndSet(), &transform->Y };
        CExprToNodeGraph c3 { items.at(2), GEnv.Scene->GetBankAndSet(), &transform->Z };
        return transform;
    }
    return nullptr;
}
}