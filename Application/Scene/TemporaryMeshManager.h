#pragma once
#include "Scene.h"
#include <Parsing/ASTContext.h>
#include <Parsing/SourceManager.h>
#include <string>
#include <vector>
#include <map>

namespace Nome::Scene
{

class CMesh;
class CPolyline;

class CTemporaryMeshManager
{
public:
    CTemporaryMeshManager(const TAutoPtr<CScene>& scene,
                          const std::shared_ptr<CSourceManager>& sourceMgr)
        : Scene(scene)
        , SourceMgr(sourceMgr)
    {
    }

    ~CTemporaryMeshManager() = default;

    //void ResetTemporaryMesh(); Randy note: after 10/1, not used anymore as it's equivalent to reloading a file

    void RemoveFace(const std::vector<std::string>& facePoints); // Randy added this. Not sure if TemporaryMeshManager is best place for it, but putting it here for now.

    void AddFace(const std::vector<std::string>& facePoints);
    void AddPolyline(const std::vector<std::string>& facePoints);
    void AddPoint(const std::vector<std::string> pos);
    std::string CommitChanges(
        AST::CASTContext& ctx); // const std::string& entityName, const std::string& nodeName);
    std::vector<CSceneNode *> addedSceneNodes;
    std::vector<CMesh *> addedMeshes;

private:
    TAutoPtr<CScene> Scene;
    std::shared_ptr<CSourceManager> SourceMgr;

    CMesh* TempMesh = nullptr;
    CPolyline* TempPolyline = nullptr; 
    CPolyline* TempPolylinePoint = nullptr;

    CSceneNode* TempMeshNode = nullptr;
    CSceneNode* TempPolylineNode = nullptr;
    CSceneNode* TempPointNode = nullptr;
    CSceneNode* TempPolylinePointNode = nullptr;

    unsigned int FaceCounter = 0;
    unsigned int num_polylines = 0;
    unsigned int num_points = 0;
    unsigned int polyline_prev_num_points = 0;
    std::map<std::string, std::string> pointMap;
};

}
