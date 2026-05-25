#include "ASTSceneAdapter.h"
#include "Arc.h"
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
#include "Icosphere.h"
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
#include "../QtFrontend/DebugDraw.h"
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
    { "facenormal", ECommandKind::Instance },
    { "vertexnormal", ECommandKind::Instance },
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
    { "arc", ECommandKind::Entity },
    { "icosphere", ECommandKind::Entity },
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
struct commandInput
{
    Nome::AST::ACommand* provided_command = nullptr;
    std::string type = "";
    std::string name = "";
    std::string calledFrom = "offset";


    commandInput(Nome::AST::ACommand* inputted_command, std::string inputted_type,
                 std::string inputted_name, std::string inputted_called_from = "")
        : provided_command(inputted_command)
        , type(inputted_type)
        , name(inputted_name)
        , calledFrom(inputted_called_from)
    {
    }

    bool isInputtedCommand(const std::string& target_name) const
    {
        if (name == target_name)
            return true; // Found a match
        return false; // No match found
    }
};
std::map<std::string, Nome::Scene::CEntity*> meshMappings;
std::vector<std::string> GroupNames = {};
std::unordered_set<Scene::CSceneTreeNode*> visitedNodes;
std::vector<commandInput> provided_commands = {};
std::vector<commandInput> completed_commands = {};
std::vector<std::string> localInstanceIDs = {};
std::map<std::string, std::vector<std::string>> groupSurfaceMappings = {};//TAutoPtr<CSurface>
std::map<std::string, TAutoPtr<CSurface>*> groupSurfaces = {}; // TAutoPtr<CSurface>
std::map<std::string, std::vector<Nome::Scene::CSceneNode*>> groupSourceNode = {}; // TAutoPtr<CSurface>
std::map<std::string, size_t> groupSizePairings = {};
//std::vector<TAutoPtr<CEntity>> keepAliveEntities = {};
// 
//  Needed to bypass the flow node issue to convert the currentMatrix to be able to connect the transforms
class CStaticMatrixSource : public Flow::CFlowNode
{
public:
    Flow::TOutput<tc::Matrix3x4> MatrixOut;

    CStaticMatrixSource(const tc::Matrix3x4& mat)
        : MatrixOut(this, []() {})
    {
        MatrixOut.UpdateValue(mat);
    }
};
void CASTSceneAdapter::VisitCommandSyncScene(AST::ACommand* cmd, CScene& scene, bool insubMesh,
                                             std::string groupName)
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
        int i = 0;
        commandInput command_to_review = commandInput(nullptr, "", "dummy");
        for (int i =0; i < provided_commands.size(); i++)
        {
            const commandInput command_to_check = provided_commands.at(i);
            if (command_to_check.isInputtedCommand(cmd->GetPositionalIdentAsString(1)))
            {
                command_to_review = command_to_check;
                provided_commands.erase(provided_commands.begin() + i);
                completed_commands.push_back(command_to_check);
                break;
            }
        }
        //std::cout << "||" << command_to_review.name << "||" << command_to_review.type;

        if (command_to_review.name != "dummy")
        {
            auto surface = cmd->GetNamedArgument("surface");
            std::string surfaceName = "";
            std::string backfaceName = "";
            if (surface)
            {
                auto surfaceEntityNameExpr = surface->GetArgument(
                    0)[0]; // Returns a casted AExpr that was an AIdent before casting
                auto surfaceIdentifier = static_cast<AST::AIdent*>(&surfaceEntityNameExpr)
                                             ->ToString(); // Downcast it back to an AIdent
                auto surfaceEntity = GEnv.Scene->FindEntity(surfaceIdentifier);
                if (surfaceEntity)
                    surfaceName = surfaceEntity.Get()->GetName();
                std::cout << "\nsName: " << surfaceName;
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
                    backfaceName = backfaceEntity.Get()->GetName();
                std::cout << "\nbName: " << backfaceName;
            }
            CExprEvalDirect eval;
            bool hasFaceNormal = false;
            float faceNormalMultiplier = 0;
            bool hasVertexNormal = false;
            float vertexNormalMultiplier = 0;
            auto fn = cmd->GetNamedArgument("facenormal");
            if (fn)
            {
                hasFaceNormal = true;
                auto faceNormalExpr = fn->GetArgument(0);
                try
                {
                    // Checks first for slider value upon initialization
                    std::ostringstream oss;
                    oss << fn;
                    std::string result = oss.str();
                    result = result.substr(10, result.length());
                    if (GEnv.Scene->GetBankAndSet().GetSlider(result) == nullptr)
                        throw AST::CSemanticError("Facenormal length is not slider, will now check for value",fn);
                    faceNormalMultiplier = GEnv.Scene->GetBankAndSet().GetSlider(result)->GetValue();
                }
                catch (const AST::CSemanticError& e)
                {
                    // Checks for a numerical value if no slider value is provided
                    try
                    {
                        auto temp = std::any_cast<float>(faceNormalExpr->Accept(&eval));
                        std::ostringstream oss;
                        oss << temp;
                        std::string result = oss.str();
                        char* charArray = new char[result.length() + 1];
                        std::strcpy(charArray, result.c_str());
                        faceNormalMultiplier = std::atof(charArray);
                    }
                    catch (int e)
                    {
                        // The input was invalid
                        // Currently the error message is misleading for an invalid input
                        // "Direct expression does not support variable references"
                        // Comes from the CExprEvalDirect Accept function
                        throw AST::CSemanticError("Facenormal multiplier is not properly defined", fn);
                    }
                }
            }
            auto vn = cmd->GetNamedArgument("vertexnormal");
            if (vn)
            {
                hasVertexNormal = true;
                auto vertexNormalExpr = vn->GetArgument(0);
                try
                {
                    // Checks first for slider value upon initialization
                    std::ostringstream oss;
                    oss << vn;
                    std::string result = oss.str();
                    result = result.substr(12, result.length());
                    if (GEnv.Scene->GetBankAndSet().GetSlider(result) == nullptr)
                        throw AST::CSemanticError(
                            "Vertexnormal length is not slider, will now check for value", fn);
                    vertexNormalMultiplier =
                        GEnv.Scene->GetBankAndSet().GetSlider(result)->GetValue();
                }
                catch (const AST::CSemanticError& e)
                {
                    // Checks for a numerical value if no slider value is provided
                    try
                    {
                        auto temp = std::any_cast<float>(vertexNormalExpr->Accept(&eval));
                        std::ostringstream oss;
                        oss << temp;
                        std::string result = oss.str();
                        char* charArray = new char[result.length() + 1];
                        std::strcpy(charArray, result.c_str());
                        vertexNormalMultiplier = std::atof(charArray);
                    }
                    catch (int e)
                    {
                        // The input was invalid
                        // Currently the error message is misleading for an invalid input
                        // "Direct expression does not support variable references"
                        // Comes from the CExprEvalDirect Accept function
                        throw AST::CSemanticError("Vertexnormal multiplier is not properly defined",
                                                  fn);
                    }
                }
            }
            std::cout << "f:" << faceNormalMultiplier << ", v:" << vertexNormalMultiplier;
            if (command_to_review.type == "merge")
            {
                // Robert 2/5/2025
                // Took the code from CMainWindow::on_actionMerge_triggered() and modified it so
                // referenced instances will be merged.
                // If you wish to change how merging works, modify the MergeIn function in MeshMerger.cpp
                
                // One shot merging, and add a new entity and its corresponding node

                // Gather all instances to merge
                std::string instanceNames = "";
                std::vector<std::string> instance_strings = {};
                for (auto* sub : command_to_review.provided_command->GetSubCommands())
                {
                    // std::cout << "sub: " << sub->GetName() << "\n" << endl;
                    instanceNames += sub->GetName() + ":::";
                    instance_strings.push_back(sub->GetName());
                    VisitCommandSyncScene(sub, scene, false);
                }
                GEnv.Scene->Update();
                // CMeshMerger is basically a CMesh, but with a MergeIn method. 
                // Merger will contain ALL the merged vertices (from various meshes)
                // The name of the CMeshMerger is an identifier
                tc::TAutoPtr<Scene::CMeshMerger> merger =
                    new Scene::CMeshMerger(command_to_review.provided_command->GetName());
                bool flag = false;
                bool isInstance = false;

                std::deque<Scene::CSceneTreeNode*> nodesToVisit;
                std::unordered_set<Scene::CSceneTreeNode*> visitedNodes;

                // Seed only the selected group/root nodes you actually want to merge.
                GEnv.Scene->ForEachSceneTreeNode(
                    [&](Scene::CSceneTreeNode* node)
                    { 
                        for (int i = 0; i < instance_strings.size(); i++)
                        {
                            if (instance_strings[i] == node->GetOwner()->GetName())
                                nodesToVisit.push_back(node);
                        }
                    });
                

                while (!nodesToVisit.empty())
                {
                    Scene::CSceneTreeNode* node = nodesToVisit.front();
                    nodesToVisit.pop_front();

                    if (!node)
                        continue;

                    if (!visitedNodes.insert(node).second)
                    {
                        std::cout << "\nSkipping already visited node: "
                                  << node->GetOwner()->GetName();
                        continue;
                    }

                    if (node->GetOwner()->GetName() == "globalMergeNode")
                    {
                        flag = true;
                        continue;
                    }

                    // Queue children exactly once.
                    for (auto* child : node->GetChildren())
                    {
                        if (!child)
                            continue;

                        if (child->GetOwner()->GetName() == node->GetOwner()->GetName())
                            continue;

                        nodesToVisit.push_back(child);
                    }

                    auto* entity = node->GetInstanceEntity();
                    if (!entity)
                        entity = node->GetOwner()->GetEntity();

                    if (auto* mesh = dynamic_cast<Scene::CMeshInstance*>(entity))
                    {
                        std::cout << "\nMERGING node=" << node->GetOwner()->GetName()
                                  << " nodePtr=" << node << " entityPtr=" << entity
                                  << " meshPtr=" << mesh;

                        mesh->GetDSMesh().computeNormals();

                        std::vector<Face*> myFaceList = mesh->GetDSMesh().faceList;

                        surfaceName = "";
                        backfaceName = "";

                        if (node->GetOwner()->GetSurface())
                            surfaceName = node->GetOwner()->GetSurface().Get()->GetName();

                        if (!surfaceName.empty())
                        {
                            for (Face* f : myFaceList)
                            {
                                if (f->surfaceName.empty())
                                    f->surfaceName = surfaceName;
                            }
                        }

                        if (node->GetOwner()->GetBackface())
                            backfaceName = node->GetOwner()->GetBackface().Get()->GetName();

                        if (!backfaceName.empty())
                        {
                            for (Face* f : myFaceList)
                            {
                                if (f->backfaceName.empty())
                                    f->backfaceName = backfaceName;
                            }
                        }

                        merger->MergeIn(*mesh, true);
                        entity->isMerged = true;

                        merger->CreateNormals(mesh->GetDSMesh(), hasFaceNormal,
                                              faceNormalMultiplier, hasVertexNormal,
                                              vertexNormalMultiplier);
                    }
                }
                
                GEnv.Scene->Update();
                if (flag)
                {
                    // repeated merges will not do anything which is cool.
                    return;
                }
                // TODO: 10/22 added.  These lines work to reset the scene
                GEnv.Scene->ForEachSceneTreeNode(
                    [&](Scene::CSceneTreeNode* node)
                    {
                        if (node->GetOwner()->GetName() != "globalMergeNode")
                            node->GetOwner()->SetEntity(nullptr);
                    });

                GEnv.Scene->AddEntity(tc::static_pointer_cast<Scene::CEntity>(
                    merger)); // Merger now has all the vertices set, so we can add it into the
                              // scene as a new entity
                auto* sn = GEnv.Scene->GetRootNode()->FindOrCreateChildNode(
                    "globalMergeNode"); // Add it into the Scene Tree by creating a new node called
                                        // globalMergeNode. Notice, this is the same name everytime
                                        // you Merge. This means you can only have one merger mesh
                                        // each time. It will override previous merger meshes with
                                        // the new vertices.
                sn->SetEntity(
                    merger.Get()); // Set sn, which is the scene node, to point to entity merger
                std::cout << "\ncmdName: " << command_to_review.name << "\n";
                auto* mergedEntity = merger.Get();

                std::string cmdName = command_to_review.name;
                std::string providedName = command_to_review.provided_command->GetName();
                std::string entityName = mergedEntity->GetName();

                std::cout << "\n[Merge Register]"
                          << "\n  command_to_review.name: " << cmdName
                          << "\n  provided_command name: " << providedName
                          << "\n  merger entity name: " << entityName;

                // Main aliases
                meshMappings[cmdName] = mergedEntity;
                meshMappings[providedName] = mergedEntity;
                meshMappings[entityName] = mergedEntity;

                // Register the inputs that produced this merge.
                for (const auto& name : instance_strings)
                {
                    std::cout << "\n  alias: " << name;
                    meshMappings[name] = mergedEntity;
                }
            }
            else if (command_to_review.type == "offset")
            {
                double height = 0.1f;
                double width = 0.0f;
                CExprEvalDirect eval;

                // Parse height
                auto* heightInfo = command_to_review.provided_command->GetNamedArgument("height");

                auto items = std::any_cast<AST::ANamedArgument*>(heightInfo)->GetArguments();

                try
                {
                    std::ostringstream oss;
                    oss << heightInfo[0];

                    std::string result = oss.str();
                    result = result.substr(7, result.length());

                    if (GEnv.Scene->GetBankAndSet().GetSlider(result) == nullptr)
                        throw AST::CSemanticError("Height is not slider, will now check for value",
                                                  items.at(0));

                    height = GEnv.Scene->GetBankAndSet().GetSlider(result)->GetValue();
                }
                catch (const AST::CSemanticError& e)
                {
                    try
                    {
                        auto tempHeight = std::any_cast<float>(items.at(0)->Accept(&eval));
                        height = static_cast<double>(tempHeight);
                    }
                    catch (const AST::CSemanticError& e)
                    {
                        throw AST::CSemanticError("Height is not properly defined", items.at(0));
                    }
                }

                // Parse hole / width
                auto* widthInfo = command_to_review.provided_command->GetNamedArgument("hole");

                items = std::any_cast<AST::ANamedArgument*>(widthInfo)->GetArguments();

                try
                {
                    std::ostringstream oss;
                    oss << widthInfo[0];

                    std::string result = oss.str();
                    result = result.substr(6, result.length());

                    if (GEnv.Scene->GetBankAndSet().GetSlider(result) == nullptr)
                        throw AST::CSemanticError("Width is not slider, will now check for value",
                                                  items.at(0));

                    width = GEnv.Scene->GetBankAndSet().GetSlider(result)->GetValue();
                }
                catch (const AST::CSemanticError& e)
                {
                    try
                    {
                        auto tempWidth = std::any_cast<float>(items.at(0)->Accept(&eval));
                        width = static_cast<double>(tempWidth);
                    }
                    catch (const AST::CSemanticError& e)
                    {
                        throw AST::CSemanticError("Width is not properly defined", items.at(0));
                    }
                }

                std::string outputName = command_to_review.name;
                std::string providedName = command_to_review.provided_command->GetName();

                std::cout << "\n[Offset]"
                          << "\n  output command name: " << outputName
                          << "\n  provided command name: " << providedName
                          << "\n  height: " << height << "\n  width/hole: " << width;

                Scene::CMeshMerger* sourceMesh = nullptr;
                std::string sourceName;
                std::string sourceInstanceName;

                for (auto* sub : command_to_review.provided_command->GetSubCommands())
                {
                    if (!sub)
                        continue;

                    // Ensure the instance/source command is processed.
                    VisitCommandSyncScene(sub, scene, false);

                    sourceInstanceName = sub->GetName();

                    try
                    {
                        sourceName = sub->GetPositionalIdentAsString(1);
                    }
                    catch (const AST::CSemanticError& e)
                    {
                        std::cout << "\n[Offset] Could not read source from subcommand: "
                                  << sourceInstanceName;
                        continue;
                    }

                    std::cout << "\n  sub instance: " << sourceInstanceName
                              << "\n  source target: " << sourceName;

                    auto it = meshMappings.find(sourceName);
                    if (it == meshMappings.end())
                    {
                        std::cout << "\n[Offset] source not found in meshMappings: " << sourceName;
                        continue;
                    }

                    sourceMesh = dynamic_cast<Scene::CMeshMerger*>(it->second);
                    if (!sourceMesh)
                    {
                        std::cout << "\n[Offset] source is not CMeshMerger: " << sourceName;
                        continue;
                    }

                    break;
                }

                if (!sourceMesh)
                {
                    std::cout << "\n[Offset Error] no valid merged source found for: " << outputName
                              << std::endl;

                    std::cout << "\nAvailable meshMappings keys:";
                    for (const auto& pair : meshMappings)
                        std::cout << "\n  " << pair.first;

                    return;
                }

                // Offset the existing merged mesh in place.
                std::cout << "\nOffset resolved source: " << sourceName;
                std::cout << "\nOffset output alias: " << outputName;
                std::cout << "\nOffset height: " << height;
                std::cout << "\nOffset width/hole: " << width;

                sourceMesh->setOffsetHeightWidth(height, width);
                sourceMesh->setOffset(true);
                sourceMesh->Catmull();
                sourceMesh->setOffset(false);
                sourceMesh->MarkDirty();
                sourceMesh->CreateNormalsCurr(hasFaceNormal, faceNormalMultiplier, hasVertexNormal,
                                              vertexNormalMultiplier);

                // Register the offset command name as another alias to the same modified mesh.
                meshMappings[outputName] = sourceMesh;
                meshMappings[providedName] = sourceMesh;

                if (!sourceInstanceName.empty())
                    meshMappings[sourceInstanceName] = sourceMesh;

                std::cout << "\n[Offset Register]"
                          << "\n  output: " << outputName << "\n  provided: " << providedName
                          << "\n  source: " << sourceName
                          << "\n  local source instance: " << sourceInstanceName;

                GEnv.Scene->Update();
            }
            else if (command_to_review.type == "subdivision")
            {
                // Parse sd_level
                auto* widthInfo = command_to_review.provided_command->GetNamedArgument("sd_level");

                int width = 1;
                CExprEvalDirect eval;
                auto items = std::any_cast<AST::ANamedArgument*>(widthInfo)->GetArguments();

                try
                {
                    std::ostringstream oss;
                    oss << widthInfo[0];

                    std::string result = oss.str();
                    result = result.substr(6, result.length());

                    if (GEnv.Scene->GetBankAndSet().GetSlider(result) == nullptr)
                        throw AST::CSemanticError("Level is not slider, will now check for value",
                                                  items.at(0));

                    width =
                        static_cast<int>(GEnv.Scene->GetBankAndSet().GetSlider(result)->GetValue());
                }
                catch (const AST::CSemanticError& e)
                {
                    try
                    {
                        auto tempWidth = std::any_cast<float>(items.at(0)->Accept(&eval));
                        width = static_cast<int>(tempWidth);
                    }
                    catch (const AST::CSemanticError& e)
                    {
                        throw AST::CSemanticError("Level is not properly defined", items.at(0));
                    }
                }

                std::string outputName = command_to_review.name;
                std::string providedName = command_to_review.provided_command->GetName();

                std::cout << "\n[Subdivision]"
                          << "\n  output command name: " << outputName
                          << "\n  provided command name: " << providedName
                          << "\n  level: " << width;

                Scene::CMeshMerger* sourceMesh = nullptr;
                std::string sourceName;
                std::string sourceInstanceName;

                for (auto* sub : command_to_review.provided_command->GetSubCommands())
                {
                    if (!sub)
                        continue;

                    // Ensure the instance command is processed.
                    VisitCommandSyncScene(sub, scene, false);

                    sourceInstanceName = sub->GetName();

                    try
                    {
                        sourceName = sub->GetPositionalIdentAsString(1);
                    }
                    catch (const AST::CSemanticError& e)
                    {
                        std::cout << "\n[Subdivision] Could not read source from subcommand: "
                                  << sourceInstanceName;
                        continue;
                    }

                    std::cout << "\n  sub instance: " << sourceInstanceName
                              << "\n  source target: " << sourceName;

                    auto it = meshMappings.find(sourceName);
                    if (it == meshMappings.end())
                    {
                        std::cout << "\n[Subdivision] source not found in meshMappings: "
                                  << sourceName;
                        continue;
                    }

                    sourceMesh = dynamic_cast<Scene::CMeshMerger*>(it->second);
                    if (!sourceMesh)
                    {
                        std::cout << "\n[Subdivision] source is not CMeshMerger: " << sourceName;
                        continue;
                    }

                    break;
                }

                if (!sourceMesh)
                {
                    std::cout << "\n[Subdivision Error] no valid merged source found for: "
                              << outputName << std::endl;

                    std::cout << "\nAvailable meshMappings keys:";
                    for (const auto& pair : meshMappings)
                        std::cout << "\n  " << pair.first;

                    return;
                }
                // Subdivide the existing merged mesh in place.
                std::cout << "\nSubdivision resolved source: " << sourceName;
                std::cout << "\nSubdivision output alias: " << outputName;
                std::cout << "\nSubdivision level: " << width;

                // Create a new merger for the subdivided result.
                tc::TAutoPtr<Scene::CMeshMerger> subdivided = new Scene::CMeshMerger(outputName);

                sourceMesh->changeColors(surfaceName, backfaceName);
                sourceMesh->setSubLevel(width);
                sourceMesh->Catmull();
                sourceMesh->MarkDirty();
                sourceMesh->CreateNormalsCurr(hasFaceNormal, faceNormalMultiplier, hasVertexNormal,
                                              vertexNormalMultiplier);

                // Register the subdivision command name as another alias to the same merged mesh.
                meshMappings[outputName] = sourceMesh;
                meshMappings[providedName] = sourceMesh;

                if (!sourceInstanceName.empty())
                    meshMappings[sourceInstanceName] = sourceMesh;

                GEnv.Scene->Update();
                std::cout << "\n[Subdivision Register]"
                          << "\n  output: " << outputName << "\n  provided: " << providedName
                          << "\n  source: " << sourceName
                          << "\n  local source instance: " << sourceInstanceName;

                GEnv.Scene->Update();
            }
        }
        else
        {
            // CreateChildNode() adds a node to the scene graph IF it hasn't been added already, and
            // always adds a node to the scene tree This means ONE sceneNode could correspond to
            // multiple scene tree nodes, which is how we want to represent the scene
            auto* sceneNode = InstanciateUnder->CreateChildNode(cmd->GetName());
            // std::cout << "Name:" << cmd->GetName();
            //  To perform rotation
            sceneNode->SyncFromAST(cmd, scene);

            // TODO: move the following logic into SyncFromAST
            // Check to see if there is a surface color associated with this instance or group scene
            // node. If the surface argument exists, then set it to be the scene node's surface.
            // Surface color for group vs mesh instance logic is handled in InteractiveMesh.cpp (at
            // the rendering stage).
            std::string surfaceName = "";
            auto surface = cmd->GetNamedArgument("surface");
            TAutoPtr<CEntity> surfaceEntity;
            TAutoPtr<CSurface> sfE; // For groups
            if (surface)
            {
                auto surfaceEntityNameExpr = surface->GetArgument(
                    0)[0]; // Returns a casted AExpr that was an AIdent before casting
                auto surfaceIdentifier = static_cast<AST::AIdent*>(&surfaceEntityNameExpr)
                                             ->ToString(); // Downcast it back to an AIdent
                surfaceName = surfaceIdentifier;
                surfaceEntity = GEnv.Scene->FindEntity(surfaceIdentifier);

                surfaceEntity = GEnv.Scene->FindEntity(surfaceIdentifier);
                if (auto* asSurface = dynamic_cast<Nome::Scene::CSurface*>(surfaceEntity.Get()))
                {
                    sfE = TAutoPtr<CSurface>(asSurface);
                    //std::cout << "SFE Cast Successful";
                }
                else
                {
                    std::cout << "\n ---Bad SFE Cast---\n";
                }
            }
            std::string backfaceName = "";
            auto backface = cmd->GetNamedArgument("backface");
            if (backface)
            {
                auto backfaceEntityNameExpr = backface->GetArgument(
                    0)[0]; // Returns a casted AExpr that was an AIdent before casting
                auto backfaceIdentifier = static_cast<AST::AIdent*>(&backfaceEntityNameExpr)
                                              ->ToString(); // Downcast it back to an AIdent
                backfaceName = backfaceIdentifier;
                auto backfaceEntity = GEnv.Scene->FindEntity(backfaceIdentifier);
                if (backfaceEntity)
                    sceneNode->SetBackface(dynamic_cast<CBackface*>(backfaceEntity.Get()));
            }
            CExprEvalDirect eval;
            bool hasFaceNormal = false;
            float faceNormalMultiplier = 0;
            bool hasVertexNormal = false;
            float vertexNormalMultiplier = 0;
            auto fn = cmd->GetNamedArgument("facenormal");
            if (fn)
            {
                hasFaceNormal = true;
                auto faceNormalExpr = fn->GetArgument(0);
                try
                {
                    // Checks first for slider value upon initialization
                    std::ostringstream oss;
                    oss << fn;
                    std::string result = oss.str();
                    result = result.substr(10, result.length());
                    if (GEnv.Scene->GetBankAndSet().GetSlider(result) == nullptr)
                        throw AST::CSemanticError(
                            "Facenormal length is not slider, will now check for value", fn);
                    faceNormalMultiplier =
                        GEnv.Scene->GetBankAndSet().GetSlider(result)->GetValue();
                }
                catch (const AST::CSemanticError& e)
                {
                    // Checks for a numerical value if no slider value is provided
                    try
                    {
                        auto temp = std::any_cast<float>(faceNormalExpr->Accept(&eval));
                        std::ostringstream oss;
                        oss << temp;
                        std::string result = oss.str();
                        char* charArray = new char[result.length() + 1];
                        std::strcpy(charArray, result.c_str());
                        faceNormalMultiplier = std::atof(charArray);
                    }
                    catch (int e)
                    {
                        // The input was invalid
                        // Currently the error message is misleading for an invalid input
                        // "Direct expression does not support variable references"
                        // Comes from the CExprEvalDirect Accept function
                        throw AST::CSemanticError("Facenormal multiplier is not properly defined",
                                                  fn);
                    }
                }
            }
            auto vn = cmd->GetNamedArgument("vertexnormal1");
            if (vn)
            {
                hasVertexNormal = true;
                auto vertexNormalExpr = vn->GetArgument(0);
                try
                {
                    // Checks first for slider value upon initialization
                    std::ostringstream oss;
                    oss << vn;
                    std::string result = oss.str();
                    result = result.substr(12, result.length());
                    if (GEnv.Scene->GetBankAndSet().GetSlider(result) == nullptr)
                        throw AST::CSemanticError(
                            "Vertexnormal length is not slider, will now check for value", fn);
                    vertexNormalMultiplier =
                        GEnv.Scene->GetBankAndSet().GetSlider(result)->GetValue();
                }
                catch (const AST::CSemanticError& e)
                {
                    // Checks for a numerical value if no slider value is provided
                    try
                    {
                        auto temp = std::any_cast<float>(vertexNormalExpr->Accept(&eval));
                        std::ostringstream oss;
                        oss << temp;
                        std::string result = oss.str();
                        char* charArray = new char[result.length() + 1];
                        std::strcpy(charArray, result.c_str());
                        vertexNormalMultiplier = std::atof(charArray);
                    }
                    catch (int e)
                    {
                        // The input was invalid
                        // Currently the error message is misleading for an invalid input
                        // "Direct expression does not support variable references"
                        // Comes from the CExprEvalDirect Accept function
                        throw AST::CSemanticError("Vertexnormal multiplier is not properly defined",
                                                  fn);
                    }
                }
            }
            std::cout << "f:" << faceNormalMultiplier << ", v:" << vertexNormalMultiplier;
            auto entityName = cmd->GetPositionalIdentAsString(1);
            // std::cout << "name: " << entityName << endl;
            auto entity = GEnv.Scene->FindEntity(entityName);
            // sceneNode->FindChildNode(entityName);
            //  <CMeshInstance> entity . check to see if this cast works
            //  use the casted object, which is successfully casted as a CMesh instance
            //  so now just do dsMesh = entity.GetDSMesh()
            //  dsMesh.faces, dsMesh.edgeList,
            if (entity)
            {
                sceneNode->SetEntity(
                    entity.Get()); // This line is very important. It attaches an entity
                groupSurfaceMappings[groupName].push_back(sceneNode->GetEntity()->GetName());
                if (groupName != "")
                {
                    // sceneNode->SetSurface(*groupSurfaces[groupName]);
                    groupSourceNode[groupName].push_back(sceneNode); // if you instantiate a group twice or more, the total sourcenodes will be equal to the original size of the group
                    std::cout << "\nPushed back sourceNode to " << groupName << " - " << entityName
                              << ", current total size: " << groupSourceNode[groupName].size()
                              << "\n";
                }    
                if (surfaceEntity)
                {
                    //if (groupName == "")
                        sceneNode->SetSurface(dynamic_cast<CSurface*>(surfaceEntity.Get()));
                    //else
                                
                    if (sceneNode->GetEntity()->IsMesh())
                    {
                        sceneNode->ForEachTreeNode(
                            [&](Scene::CSceneTreeNode* node)
                            {
                                auto* entity = node->GetInstanceEntity(); // Else, get the instance

                                if (!entity) // Check to see if the an entity is instantiable
                                {
                                    entity =
                                        node->GetOwner()->GetEntity(); // If it's not instantiable,
                                }
                                //std::cout << "Surface Name: " << surfaceName << "\n";
                                if (auto* mesh = dynamic_cast<Scene::CMeshInstance*>(entity))
                                {
                                    for (auto face : mesh->GetDSMesh().faceList)
                                    {
                                        if (face->surfaceName == "")
                                            face->surfaceName = surfaceName;
                                    }
                                }
                            });
                    }
                }
                GEnv.Scene->Update();

                if (sceneNode->GetEntity()->IsMesh())
                {
                    sceneNode->ForEachTreeNode(
                        [&](Scene::CSceneTreeNode* node)
                        {
                            auto* entity = node->GetInstanceEntity(); // Else, get the instance

                            if (!entity) // Check to see if the an entity is instantiable
                            {
                                entity = node->GetOwner()->GetEntity(); // If it's not instantiable,
                            }
                            // std::cout << "Surface Name: " << surfaceName << "\n";
                            if (auto* mesh = dynamic_cast<Scene::CMeshInstance*>(entity))
                            {
                                tc::TAutoPtr<Scene::CMeshMerger> merger =
                                    new Scene::CMeshMerger(entity->GetName() + "_m");

                                //merger->MergeIn(*mesh, false);
                                //entity->isMerged = true;
                                auto* normalNode = GEnv.Scene->GetRootNode()->FindOrCreateChildNode(
                                    entity->GetName() + "_normals");

                                DSMesh d = mesh->GetDSMesh();
                                merger->CreateNormals(mesh->GetDSMesh(), hasFaceNormal,
                                                      faceNormalMultiplier, hasVertexNormal,
                                                      vertexNormalMultiplier);
                                merger->MarkDirty();
                                normalNode->SetEntity(merger.Get());

                                GEnv.Scene->Update();
                            }
                        });
                }


                /*
                if (auto* mesh = dynamic_cast<Scene::CMesh*>(entity.Get()))
                {
                    for (size_t i = 0; i < mesh->Faces.GetSize(); i++)
                    {
                        // We assume the nullptr value is never returned, of course
                        auto* face = mesh->Faces.GetValue(i, nullptr);
                        face->faceSurfaceIdent = surfaceName;
                        face->faceBackfaceIdent = backfaceName;
                    }
                    mesh->UpdateEntity();
                }
                */
                // std::cout << sceneNode->GetEntity()->GetName();
                // sceneNode->SetEntity(entity);
                /*
                auto* mesh = dynamic_cast<Scene::CMeshInstance*>(sceneNode->GetEntity());
                tc::TAutoPtr<Scene::CMeshMerger> merger = new Scene::CMeshMerger(entityName + "_m");
                std::cout << mesh->GetDSMesh().faceList.size() << "<<<";
                merger->CreateNormals((mesh->GetDSMesh()), hasFaceNormal, faceNormalMultiplier,
                                      hasVertexNormal, vertexNormalMultiplier);
                auto* sn = GEnv.Scene->GetRootNode()->FindOrCreateChildNode(entityName +
                "_normals"); sn->SetEntity( merger.Get());
                    */
            }
            else if (auto group = GEnv.Scene->FindGroup(entityName))
            { // If the entityName is a group identifier 
                std::cout << "\nstuck here3";
                std::cout << "A group is called " << sceneNode->GetName() << "\n";
                GroupNames.push_back(sceneNode->GetName());
                
                std::deque<TAutoPtr<CSceneNode>> safeChildrenSnapshot;
                std::deque<tc::Matrix3x4> pos;

                for (const auto& node_ : group->GetSceneNodeChildren())
                {
                    if (node_)
                    {
                        safeChildrenSnapshot.push_back(node_);
                        pos.push_back(node_->Transform.GetValue(tc::Matrix3x4::IDENTITY));
                    }
                }
                while (!safeChildrenSnapshot.empty())
                {
                    auto node = safeChildrenSnapshot.front();
                    auto currentMatrix = pos.front();
                    safeChildrenSnapshot.pop_front();
                    pos.pop_front();
                    if (!node)
                        continue;
                    tc::TAutoPtr<Nome::Scene::CSceneNode> tempNode =
                        sceneNode->CreateChildNode(node->GetName());
                    if (tempNode)
                    {
                        auto* e = node->GetEntity();
                        if (!e)
                        {
                            if (node.Get()->GetSceneNodeChildren().size() > 0)
                            {
                                for (const auto& node_ : node->GetSceneNodeChildren())
                                {
                                    if (node_)
                                    {
                                        safeChildrenSnapshot.push_back(node_);
                                        tc::Matrix3x4 childLocal =
                                            node_->Transform.GetValue(tc::Matrix3x4::IDENTITY);
                                        pos.push_back(currentMatrix * childLocal);
                                    }
                                }
                            }
                            else
                            {
                                std::cout << "NULL entity on " << node->GetName() << "\n";
                                continue;
                            }
                        }
                        tempNode->SetEntity(node->GetEntity());
                        
                        
                        auto* matrixSource = new CStaticMatrixSource(currentMatrix);
                        // Allows nested groups to be in the correct location
                        tempNode->Transform.Connect(matrixSource->MatrixOut);
                        if (node->GetSurface())
                        {
                            tempNode->SetSurface(node->GetSurface());
                        }
                        else
                        {
                            if (sfE)
                            {
                                tempNode->SetSurface(sfE);
                            }
                        }
                    }
                }
                
            }
            else if (GEnv.Scene->ExistMerge(entityName))
            {
                std::pair<TAutoPtr<CSceneNode>, int> merge_obj = GEnv.Scene->FindMerge(entityName);
                // std::cout << "name: " << entityName << " : " <<
                // GEnv.Scene->ExistMerge(entityName) << "\n";
                auto merge = merge_obj.first;
                tc::TAutoPtr<Scene::CMeshMerger> merger = new Scene::CMeshMerger(entityName);
                for (auto node : merge->GetTreeNodes())
                {
                    std::queue<CSceneTreeNode*> q;
                    q.push(node);
                    while (!q.empty())
                    {
                        CSceneTreeNode* candidate = q.front();
                        if (candidate->GetOwner()->GetName() == "globalMergeNode")
                        {
                            continue;
                        }
                        auto* entity = candidate->GetInstanceEntity(); // Else, get the instance
                        if (!entity) // Check to see if the an entity is instantiable
                        {
                            entity =
                                candidate->GetOwner()
                                    ->GetEntity(); // If it's not instantiable, get entity instead
                        }
                        if (auto* mesh = dynamic_cast<Scene::CMeshInstance*>(entity))
                        {
                            merger->MergeIn(*mesh, true);
                            entity->isMerged = true;
                        }
                        const auto& childNodes = q.front()->GetChildren();
                        for (CSceneTreeNode* child : childNodes)
                            q.push(child);
                        q.pop();
                    }
                }
                GEnv.Scene->Update();
                GEnv.Scene->ForEachSceneTreeNode(
                    [&](Scene::CSceneTreeNode* node)
                    {
                        if (node->GetOwner()->GetName() != "globalMergeNode")
                            node->GetOwner()->SetEntity(nullptr);
                    });
                GEnv.Scene->AddEntity(tc::static_pointer_cast<Scene::CEntity>(merger));
                auto* sn = GEnv.Scene->GetRootNode()->FindOrCreateChildNode(cmd->GetName());
                merge->SetEntity(merger.Get());
                merge->AddParent(sceneNode);
                GEnv.Scene->AddEntity(tc::static_pointer_cast<Scene::CEntity>(
                    merger)); // Merger now has all the vertices set, so we can add it into the
                              // scene as a new
            }
            else
            {
                throw AST::CSemanticError(
                    tc::StringPrintf("Instantiation failed, unknown generator: %s",
                                     entityName.c_str()),
                    cmd);
            }
        }
    }
    else if (cmd->GetCommand() == "group")
    {
        InstanciateUnder = GEnv.Scene->CreateGroup(cmd->GetName());
        InstanciateUnder->SyncFromAST(cmd, scene);
        std::cout << "\nOuter Group Name: " << cmd->GetName() << "\n";
        //TAutoPtr<CSurface>* groupSurfacePointer;
        //groupSurfaces[cmd->GetName()] = groupSurfacePointer;
        std::cout << "\nSubcommands for " << cmd->GetName() << ": " << cmd->GetSubCommands().size()
                  << "\n";
        groupSizePairings[cmd->GetName()] = cmd->GetSubCommands().size();
        for (auto* sub : cmd->GetSubCommands())
            VisitCommandSyncScene(sub, scene, false, cmd->GetName());
        InstanciateUnder = GEnv.Scene->GetRootNode();
    }
    else if (cmd->GetCommand() == "merge") {
        provided_commands.push_back(commandInput(cmd, "merge", cmd->GetName()));
    }
    /*
    normals norm_pyramid vertexnormal 0.5

      instance imerge_pyramid merge_pyramid  endinstance

    endnormals

    instance inorm_pyramid norm_pyramid endinstance
    */
    else if (cmd->GetCommand() == "facenormal1")
    {
        // Robert 2/19/2025
        // Gather all instances to merge
        std::cout << "\nranfacehereidkwhy\n";
        std::string instanceNames = "";
        for (auto* sub : cmd->GetSubCommands())
        {
            // std::cout << "sub: " << sub->GetName() << "\n" << endl;
            instanceNames += sub->GetName() + ":::";
            VisitCommandSyncScene(sub, scene, false);
        }
        GEnv.Scene->Update();

        // tc::TAutoPtr<Scene::CMeshMerger> merger = new Scene::CMeshMerger(cmd->GetName());
        tc::TAutoPtr<Scene::CMeshMerger> merger =
            new Scene::CMeshMerger(cmd->GetName());
        bool flag = false;
        bool isInstance = false;
        GEnv.Scene->ForEachSceneTreeNode(
            [&](Scene::CSceneTreeNode* node)
            {
                /*
                if (node->GetOwner()->GetName() == "globalMergeNode")
                {
                    flag = true;
                    return;
                }
                */
                auto* entity = node->GetInstanceEntity(); // Else, get the instance

                if (!entity) // Check to see if the an entity is instantiable
                {
                    entity = node->GetOwner()
                                 ->GetEntity(); // If it's not instantiable, get entity instead
                }
                if (auto* mesh = dynamic_cast<Scene::CMeshInstance*>(entity))
                {
                    // std::cout << "meshName: " << node->GetOwner()->GetName() << "\n" << endl;
                    size_t start = 0;
                    size_t end;

                    // Loop through and extract words split by ":::".
                    while ((end = instanceNames.find(":::", start)) != std::string::npos)
                    {
                        // Extract word between start and end
                        std::string word = instanceNames.substr(start, end - start);
                        // std::cout << "Word: " << word << std::endl;
                        if (word.compare(node->GetOwner()->GetName()) == 0)
                            isInstance = true;
                        // Move start position to the next part after ":::"
                        start = end + 3;
                    }
                    if (isInstance)
                    {
                        // merger->MergeIn(*mesh, true);
                        // entity->isMerged = true;
                        // auto* meshObj = dynamic_cast<Scene::>(entity);
                        mesh->GetDSMesh().computeNormals();
                        std::vector<tc::TAutoPtr<Scene::CPolyline>> polyLines = {};
                        std::vector<Face*> faceList = mesh->GetDSMesh().faceList;
                        for (auto* f : faceList)
                        {
                            Vertex center =
                                mesh->GetDSMesh().centerPoint(f); // Gets center point of face
                            Vertex distant = Vertex();
                            distant.position =
                                center.position + f->normal; // Get the point the normal points to
                            std::vector<Vertex*> vertList = {};

                            Vertex temp = Vertex();
                            temp.SetPosition((center.position.x + distant.position.x) / 2 + 0.1,
                                             (center.position.y + distant.position.y) / 2 + 0.1,
                                             (center.position.z + distant.position.z) / 2 + 0.1);
                            // vertList.push_back(&temp);
                            std::cout << center.position.x << "\t" << center.position.y << "\t"
                                      << center.position.z << "\n";
                            std::cout << distant.position.x << "\t" << distant.position.y << "\t"
                                      << distant.position.z << "\n";
                            vertList.push_back(&center);
                            vertList.push_back(&temp);
                            vertList.push_back(&distant);
                            // mesh->GetDSMesh().createEdge(&center, &distant);
                            // AddLineStrip("normalLine" + f->name, vertList);
                            // mesh->GetDSMesh().addVertex(&center);
                            // mesh->GetDSMesh().addVertex(&distant);
                            // mesh->GetDSMesh().createEdge(&center, &distant);
                            mesh->GetDSMesh().addVertex(&center);
                            mesh->GetDSMesh().addVertex(&temp);
                            mesh->GetDSMesh().addVertex(&distant);
                            mesh->GetDSMesh().createEdge(&center, &distant);
                            // mesh->GetDSMesh().addFace(vertList, "", "", false);
                            // CPolyline p = CPolyline();
                            // mesh->GetDSMesh();
                            // std::vector<std::string> huh;
                            // huh.push_back(f->name);
                            // mesh->RemoveFace(huh);
                            // mesh->Instantiate(node);

                            // std::cout << vertList.size() << "size\n";
                            // meshObj->AddLineStrip("normalLine" + f->name, vertList);
                            // mesh->GetDSMesh().addFace(vertList, "", "", false);
                            // p.AddLineStrip("normalLine" + f->name, vertList);
                            // auto* sceneNode =
                            // InstanciateUnder->CreateChildNode(cmd->GetName()+f->name);
                            // sceneNode->SetEntity(entity);
                            //->SetEntity((Scene::CEntity)(&mesh));
                            // p.Instantiate(node);
                            // tc::TAutoPtr<Scene::CMesh> poly = &p;
                            // polyLines.push_back(&p);
                            // tc::static_pointer_cast<Scene::CEntity>
                            // GEnv.Scene->AddEntity(&p);
                            // GEnv.Scene->Update();
                            // entity->MarkDirty();
                            // entity->UpdateEntity();
                        }
                        mesh->RemoveFace({ faceList.at(1)->name });
                        merger->MergeIn(*mesh, false);
                        entity->isMerged = true;
                        GEnv.Scene->Update();
                        if (flag)
                        {
                            // repeated merges will not do anything which is cool.
                            return;
                        }

                        // TODO: 10/22 added.  These lines work to reset the scene
                        GEnv.Scene->ForEachSceneTreeNode(
                            [&](Scene::CSceneTreeNode* node)
                            {
                                if (node->GetOwner()->GetName() != "globalMergeNode")
                                    node->GetOwner()->SetEntity(nullptr);
                            });

                        GEnv.Scene->AddEntity(tc::static_pointer_cast<Scene::CEntity>(
                            merger)); // Merger now has all the vertices set, so we can add it into
                                      // the scene as a new entity
                        auto* sn = GEnv.Scene->GetRootNode()->FindOrCreateChildNode(
                            "globalMergeNode"); // Add it into the Scene Tree by creating a new node
                                                // called globalMergeNode. Notice, this is the same
                                                // name everytime you Merge. This means you can only
                                                // have one merger mesh each time. It will override
                                                // previous merger meshes with the new vertices.
                        sn->SetEntity(merger.Get()); // Set sn, which is the scene node, to point to
                                                     // entity merger
                        std::cout << merger.Get()->Faces.GetSize() << "\t ~\n";
                    }
                    if (entity)
                    {
                        // node->GetOwner()->GetScene()->RemoveEntity(entity->GetName(), true);
                        // node->GetOwner()->GetScene()->AddEntity(entity);
                    }
                    
                    isInstance = false;
                }
            });
        /*
        for (auto* sub : cmd->GetSubCommands())
        {
            std::cout << "sub: " << sub->GetName() << "\n" << endl;
            //instanceNames += sub->GetName() + ":::";
            //VisitCommandSyncScene(sub, scene, false);
        }
        */
        GEnv.Scene->Update();
        /*
        if (flag)
        {
            return;
        }
        GEnv.Scene->ForEachSceneTreeNode(
            [&](Scene::CSceneTreeNode* node)
            {
                if (node->GetOwner()->GetName() != "globalMergeNode")
                    node->GetOwner()->SetEntity(nullptr);
            });
            */
        // GEnv.Scene->AddEntity(tc::static_pointer_cast<Scene::CEntity>(merger)); // Merger now has
        // all the vertices set, so we can add it into the scene as a
        //  new entity
        // auto* sn = GEnv.Scene->GetRootNode()->FindOrCreateChildNode("globalMergeNode"); // Add it
        // into the Scene Tree by creating a new node called
        //  globalMergeNode. Notice, this is the same name everytime you
        //  Merge. This means you can only have one merger mesh each time. It
        //  will override previous merger meshes with the new vertices.
        // sn->SetEntity(merger.Get()); // Set sn, which is the scene node, to point to entity
        // merger
    }
    else if (cmd->GetCommand() == "vertexnormal")
    {
        /*
            Way to fix instantiation issue
            When any of these are run, add them to a list with (ID, cmd) ie (myMerge, merge)
            Within the instance field in this file i will check if the id to instantiate matches one in the list, if it does execute that instead
        */
        // Robert 2/19/2025
        std::string instanceNames = "";
        for (auto* sub : cmd->GetSubCommands())
        {
            // std::cout << "sub: " << sub->GetName() << "\n" << endl;
            instanceNames += sub->GetName() + ":::";
            VisitCommandSyncScene(sub, scene, false);
        }
        GEnv.Scene->Update();

        // tc::TAutoPtr<Scene::CMeshMerger> merger = new Scene::CMeshMerger(cmd->GetName());
        bool flag = false;
        bool isInstance = false;
        GEnv.Scene->ForEachSceneTreeNode(
            [&](Scene::CSceneTreeNode* node)
            {
                /*
                if (node->GetOwner()->GetName() == "globalMergeNode")
                {
                    flag = true;
                    return;
                }
                */
                auto* entity = node->GetInstanceEntity(); // Else, get the instance

                if (!entity) // Check to see if the an entity is instantiable
                {
                    entity = node->GetOwner()
                                 ->GetEntity(); // If it's not instantiable, get entity instead
                }
                if (auto* mesh = dynamic_cast<Scene::CMeshInstance*>(entity))
                {
                    // std::cout << "meshName: " << node->GetOwner()->GetName() << "\n" << endl;
                    size_t start = 0;
                    size_t end;

                    // Loop through and extract words split by ":::".
                    while ((end = instanceNames.find(":::", start)) != std::string::npos)
                    {
                        // Extract word between start and end
                        std::string word = instanceNames.substr(start, end - start);
                        // std::cout << "Word: " << word << std::endl;
                        if (word.compare(node->GetOwner()->GetName()) == 0)
                            isInstance = true;
                        // Move start position to the next part after ":::"
                        start = end + 3;
                    }
                    if (isInstance)
                    {
                        // merger->MergeIn(*mesh, true);
                        // entity->isMerged = true;
                        mesh->GetDSMesh().computeNormals();
                        std::vector<tc::TAutoPtr<Scene::CPolyline>> polyLines = {};
                        std::vector<Vertex*> vertList = mesh->GetDSMesh().vertList;
                        for (auto* f : vertList)
                        {
                            //Vertex center = mesh->GetDSMesh().centerPoint(f); // Gets center point of face
                            Vertex distant = Vertex();
                            distant.position =
                                (Vector3)(f->GetPosition()) + f->normal; // Get the point the normal points to //add scale
                            std::vector<Vertex*> vertList = {};
                            Vector3 pos = (Vector3)(f->GetPosition());
                            //vertList.push_back(&center);
                            //vertList.push_back(&distant);
                            std::cout << pos.x << "\t" << pos.y << "\t"
                                      << pos.z << "\n";
                            std::cout << distant.position.x << "\t" << distant.position.y << "\t"
                                      << distant.position.z << "\n";
                            CPolyline p = CPolyline();
                            //mesh->GetDSMesh().createEdge(&center, &distant);
                            p.AddLineStrip("normalLine" + f->name, vertList);
                            tc::TAutoPtr<Scene::CPolyline> poly = &p;
                            // polyLines.push_back(&p);
                            p.MarkDirty();
                            p.UpdateEntity();
                            //GEnv.Scene->AddEntity(tc::static_pointer_cast<Scene::CEntity>(poly));
                            //GEnv.Scene->Update();
                            //std::cout << "ra2\n";

                            GEnv.Scene->AddEntity(tc::static_pointer_cast<Scene::CEntity>(
                                poly)); // Merger now has all the vertices set, so we can add it
                                          // into the scene as a new entity
                            auto* sn = GEnv.Scene->GetRootNode()->FindOrCreateChildNode(
                                "tempNode"); // Add it into the Scene Tree by creating a new
                                                    // node called globalMergeNode. Notice, this is
                                                    // the same name everytime you Merge. This means
                                                    // you can only have one merger mesh each time.
                                                    // It will override previous merger meshes with
                                                    // the new vertices.
                            sn->SetEntity(tc::static_pointer_cast<Scene::CEntity>(
                                poly)); // Set sn, which is the scene node, to
                                                         // point to entity merger

                        }
                        // node->GetOwner()->SetEntity((polyLines.at(0).Get()));
                        std::cout << "ra\n";
                    }
                    isInstance = false;
                }
            });
        for (auto* sub : cmd->GetSubCommands())
        {
            std::cout << "sub: " << sub->GetName() << "\n" << endl;
            // instanceNames += sub->GetName() + ":::";
            VisitCommandSyncScene(sub, scene, false);
        }
        GEnv.Scene->Update();
    }
    else if (cmd->GetCommand() == "offset")
    {
        provided_commands.push_back(commandInput(cmd, "offset", cmd->GetName()));
    }
    else if (cmd->GetCommand() == "subdivision")//subdivide
    {
        provided_commands.push_back(commandInput(cmd, "subdivision", cmd->GetName()));     
    } 
    else if (cmd->GetCommand() == "frontcolor") {
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