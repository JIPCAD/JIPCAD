
#include "Face.h"
#include "Mesh.h"
#include <StringUtils.h>

namespace Nome::Scene
{

DEFINE_META_OBJECT(CFace)
{
    BindPositionalArgument(&CFace::Points, 1);
    BindNamedArgument(&CFace::faceSurfaceIdent, "surface", 0); // Randy added on 12/12 to handle face entity coloring
    BindNamedArgument(&CFace::faceBackfaceIdent, "backface", 0);
    // Handle argSurface
    // Handle parent mesh connection
}

void CFace::MarkDirty()
{
    // Mark this entity dirty
    Super::MarkDirty();

    // And also mark the Face output dirty
    Face.MarkDirty();
}

void CFace::UpdateEntity()
{
    Super::UpdateEntity();
    Face.UpdateValue(this);
    SetValid(true);
}

size_t CFace::CountVertices() const { return Points.GetSize(); }

void CFace::SetPointSourceNames(const TAutoPtr<CScene>& scene, const std::vector<std::string>& points)
{
    for (const auto& point : points)
    {
        Flow::TOutput<CVertexInfo*>* pointOutput = scene->FindPointOutput(point);
        if (!pointOutput)
        {
            std::cout << "uhh what happened?" << std::endl;
            // Uhh what happened?
            continue;
        }
        Points.Connect(*pointOutput);
    }
}

bool CFace::AddFaceIntoMesh(CMesh* mesh) const
{
    std::vector<std::string> nameList;
    for (size_t i = 0; i < Points.GetSize(); i++)
    {
        auto* point = Points.GetValue(i, nullptr);
        // point update failed etc
        if (!point)
        {
            return false;
        }

        std::string newName = point->Name;
        if (mesh->HasVertex(point->Name))
        {
            // See if those two points are close enough
            float dist = mesh->GetVertexPos(point->Name).DistanceToPoint(point->Position);
            const float epsilon = 0.001f;
            if (dist < epsilon)
            {
                // Skip adding this vertex
                nameList.push_back(point->Name);
                continue;
            }

            // Not close enough, rename and add the vertex
            int suffix = 1;
            while (mesh->HasVertex(newName))
                newName = point->Name + std::to_string(suffix++);
            printf("[Mesh: %s] Vertex %s has been renamed to %s\n", mesh->GetName().c_str(),
                   point->Name.c_str(), newName.c_str());
        }

        mesh->AddVertex(newName, point->Position, point->sharpness);

        nameList.push_back(newName);
    }
    mesh->AddFace(GetName(), nameList, faceSurfaceIdent, faceBackfaceIdent); // Randy changed this on 12/12
    return true;
}

AST::ACommand* CFace::MakeCommandNode(AST::CASTContext& ctx, AST::ACommand* parent)
{
    if (!tc::FStringUtils::StartsWith(GetName(), parent->GetName()))
        return nullptr;
    std::string faceLocalName;
    if (parent->GetName().empty())
        faceLocalName = GetName();
    else
        faceLocalName = GetName().substr(parent->GetName().length() + 1);
    auto* faceNode = ctx.Make<AST::ACommand>(ctx.MakeToken("face"), ctx.MakeToken("endface"));
    faceNode->PushPositionalArgument(ctx.MakeIdent(faceLocalName)); // 1st positional arg is name
    // 2nd positional arg is point ident vector
    auto pointNames = Points.MapOutput<std::string>([](const auto& output) {
        auto* vs = dynamic_cast<CVertexSelector*>(output.GetOwner());
        return vs->GetPath();
    });
    std::vector<AST::AExpr*> identList;
    for (const auto& pointName : pointNames)
        identList.push_back(ctx.MakeIdent(pointName));
    faceNode->PushPositionalArgument(ctx.MakeVector(identList));
    return faceNode;
}

}