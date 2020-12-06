#include "TemporaryMeshManager.h"

#include "Mesh.h"
#include "Polyline.h"
#include <sstream>
#include <utility>
#include "ASTSceneAdapter.h"

namespace Nome::Scene
{

void CTemporaryMeshManager::ResetTemporaryMesh()
{
    if (!TempMeshNode)
        TempMeshNode = Scene->GetRootNode()->CreateChildNode("__tempMeshNode");
    else
        TempMeshNode->SetEntity(nullptr);

    if (!TempPolylineNode)
        TempPolylineNode = Scene->GetRootNode()->CreateChildNode("__tempPolylineNode");
    else
        TempPolylineNode->SetEntity(nullptr);

    if (!TempPointNode)
        TempPointNode = Scene->GetRootNode()->CreateChildNode("__tempPointNode");
    else
        TempPointNode->SetEntity(nullptr);

    if (!TempPolylinePointNode)
        TempPolylinePointNode = Scene->GetRootNode()->CreateChildNode("__tempPolylinePointNode");
    else
        TempPolylinePointNode->SetEntity(nullptr);
    // Make entity and its corresponding scene node
    Scene->RemoveEntity("__tempMesh", true);
    TempMesh = new CMesh("__tempMesh");
    Scene->AddEntity(TempMesh);
    FaceCounter = 0;

    Scene->RemoveEntity("__tempPolyline", true);
    TempPolyline = new CPolyline("__tempPolyline");
    Scene->AddEntity(TempPolyline);
    num_polylines = 0;
    
    Scene->RemoveEntity("__tempPolylinePoint", true);
    TempPolylinePoint = new CPolyline("__tempPolylinePoint");
    Scene->AddEntity(TempPolylinePoint);
    num_points = 0;

    TempMeshNode->SetEntity(TempMesh);
    TempPolylineNode->SetEntity(TempPolyline);
    TempPolylinePointNode->SetEntity(TempPolylinePoint);
}

void CTemporaryMeshManager::AddFace(const std::vector<std::string>& facePoints)
{
    if (!TempMesh || !TempMeshNode)
        ResetTemporaryMesh();

    // This function should roughly mirror the structure of CASTSceneBuilder::VisitFace
    const std::string entityNamePrefix = "__tempMesh.";
    const std::string faceName = "f" + std::to_string(FaceCounter);

    TAutoPtr<CFace> face = new CFace(entityNamePrefix + faceName);
    face->SetPointSourceNames(Scene, facePoints);
    Scene->AddEntity(tc::static_pointer_cast<CEntity>(face));

    TempMesh->Faces.Connect(face->Face);
}

void CTemporaryMeshManager::AddPolyline(const std::vector<std::string>& facePoints)
{
    for(auto i : facePoints) {
        printf("%s\n", i.c_str());
    }
    std::vector<std::string> currPoints =
        std::vector<std::string>(facePoints.begin() + polyline_prev_num_points, facePoints.end());
    CSceneNode* TempPolylineNode = nullptr;
    if (!TempPolylineNode)
        TempPolylineNode = Scene->GetRootNode()->CreateChildNode("__tempPolylineNode"
                                                                 + std::to_string(num_polylines));
    else
        TempPolylineNode->SetEntity(nullptr);

    // Scene->AddEntity(TempPolyline);
    TempPolyline = new CPolyline("__tempPolyline." + std::to_string(num_polylines));
    TempPolylineNode->SetEntity(TempPolyline);
    TempPolyline->SetPointSourceNames(Scene, currPoints);
    TempPolyline->SetClosed(false); // Hardcoding the closed bool to true. Change in the future.
    num_polylines += 1;
    polyline_prev_num_points += 1;
    // polyline_prev_num_points += currPoints.size();
}

void CTemporaryMeshManager::AddPoint(std::vector<std::string> pos)
{
    std::string pointName = "__tempPoint" + std::to_string(num_points);
    AST::ACommand *pointCmd = new AST::ACommand(new AST::CToken("point", 0, 1), new AST::CToken("endpoint", 0, 1));
    AST::AIdent *tempPointIdent = new AST::AIdent(new AST::CToken(pointName, 0, 1));
    AST::AVector *posAExpr = new AST::AVector(new AST::CToken("(", 0, 1), new AST::CToken(")", 0, 1));

    AST::ANumber *num1A = new AST::ANumber(new AST::CToken(pos.at(0), 0, 2));
    AST::ANumber *num2A = new AST::ANumber(new AST::CToken(pos.at(1), 0, 2));
    AST::ANumber *num3A = new AST::ANumber(new AST::CToken(pos.at(2), 0, 2));

    posAExpr->AddChild(num1A);
    posAExpr->AddChild(num2A);
    posAExpr->AddChild(num3A);

    pointCmd->PushPositionalArgument(tempPointIdent);
    pointCmd->PushPositionalArgument(posAExpr);

    TAutoPtr<CEntity> tempPoint = CASTSceneAdapter::MakeEntity(pointCmd->GetCommand(), pointName);
    tempPoint->GetMetaObject().DeserializeFromAST(*pointCmd, *tempPoint); 
    Scene->AddEntity(tempPoint);

    std::vector<std::string> points = { pointName };
    CSceneNode* TempPolylineNode = nullptr;
    if (!TempPolylineNode)
        TempPolylineNode = Scene->GetRootNode()->CreateChildNode("__tempPolylineNodePoint" + std::to_string(num_points));
    else
        TempPolylineNode->SetEntity(nullptr);

    TempPolylinePoint = new CPolyline("__tempPolylinePointNode." + std::to_string(num_points));
    TempPolylineNode->SetEntity(TempPolylinePoint);
    TempPolylinePoint->SetPointSourceNames(Scene, points);
    TempPolylinePoint->SetClosed(false);
    num_points += 1;
    polyline_prev_num_points += 1;
    savedPointCmd = pointCmd;
    SourceMgr->AppendCmdEndOfFile(savedPointCmd);

    SourceMgr->AppendCmdEndOfFile(savedPointCmd);

    SourceMgr->AppendCmdEndOfFile(savedPointCmd);
}

std::string CTemporaryMeshManager::CommitTemporaryPoint(AST::CASTContext& ctx,
                                                       const std::string& entityName,
                                                       const std::string& nodeName)
{
    if (!TempPolylinePoint || !TempPolylinePointNode)
        return "";

    if (!Scene->RenameEntity("__tempPolylineNodePoint", entityName))
        throw std::runtime_error("Cannot rename the temporary point, new name already exists");
    if (!TempPolylinePointNode->SetName(nodeName))
        throw std::runtime_error("Cannot rename the scene node to the desired name");

    auto* meshCmd = TempPolylinePoint->SyncToAST(ctx, true);
    SourceMgr->AppendCmdEndOfFile(meshCmd);
    // SourceMgr->AppendCmdEndOfFile(savedPointCmd);
    auto* instanceCmd = TempPolylinePointNode->BuildASTCommand(ctx);
    SourceMgr->AppendCmdEndOfFile(instanceCmd);

    TempPolylinePoint = nullptr;
    TempPolylinePointNode = nullptr;
    return "";
}

std::string CTemporaryMeshManager::CommitTemporaryMesh(AST::CASTContext& ctx,
                                                       const std::string& entityName,
                                                       const std::string& nodeName)
{
    if (!TempMesh || !TempMeshNode)
        return "";

    if (!Scene->RenameEntity("__tempMesh", entityName))
        throw std::runtime_error("Cannot rename the temporary mesh, new name already exists");
    if (!TempMeshNode->SetName(nodeName))
        throw std::runtime_error("Cannot rename the scene node to the desired name");

    auto* meshCmd = TempMesh->SyncToAST(ctx, true);
    SourceMgr->AppendCmdEndOfFile(meshCmd);
    auto* instanceCmd = TempMeshNode->BuildASTCommand(ctx);
    SourceMgr->AppendCmdEndOfFile(instanceCmd);

    TempMesh = nullptr;
    TempMeshNode = nullptr;
    return "";
}

}
