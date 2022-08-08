#include "Polyline.h"
#include "SweepControlPoint.h"

namespace Nome::Scene
{

DEFINE_META_OBJECT(CPolyline)
{
    BindPositionalArgument(&CPolyline::Points, 1);
    BindNamedArgument(&CPolyline::bClosed, "closed", 0);
}

void CPolyline::MarkDirty()
{
    // Mark this entity dirty
    Super::MarkDirty();

    // And also mark the Face output dirty
    Polyline.MarkDirty();
}

void CPolyline::UpdateEntity()
{
    if (!IsDirty())
        return;
    Super::UpdateEntity();

    std::vector<Vertex*> vertArray;
    std::vector<CVertexInfo *> positions;
    // Existence of points with cross-section information => path is for a sweep morph
    std::vector<size_t> csIndices; // Indices of the points that have cross-section info

    auto numPoints = Points.GetSize();
    Vertex* firstVert;
    for (size_t i = 0; i < numPoints; i++)
    {
        CVertexInfo* point = Points.GetValue(i, nullptr);
        if (point)
        {
            auto vertHandle = AddVertex(point->Name, point->Position);
            if (i == 0)
                firstVert = vertHandle;
            vertArray.push_back(vertHandle);
            positions.push_back(point);

            if (dynamic_cast<CSweepControlPointInfo*>(point) &&
                dynamic_cast<CSweepControlPointInfo*>(point)->CrossSection)
                csIndices.push_back(i);
        }

    }
    if (bClosed)
    {
        vertArray.push_back(firstVert);
        positions.push_back(positions[0]);
    }
    AddLineStrip("polyline", vertArray);

    SI.Positions = positions;
    SI.Name = GetName();
    SI.IsClosed = bClosed;
    if (SI.CrossSectionIndices.empty()) // May not have been initialized yet
        SI.CrossSectionIndices = csIndices;
    Polyline.UpdateValue(&SI);
    SetValid(true);
}

void CPolyline::SetPointSourceNames(const TAutoPtr<CScene>& scene, std::vector<std::string> points)
{
    PointSource = std::move(points);
    for (const auto& point : PointSource)
    {
        Flow::TOutput<CVertexInfo*>* pointOutput = scene->FindPointOutput(point);
        if (!pointOutput)
        {
            continue;
        }
        Points.Connect(*pointOutput);
    }
}

void CPolyline::SetClosed(bool closed)
{
    bClosed = closed;
    MarkDirty();
}

AST::ACommand* CPolyline::SyncToAST(AST::CASTContext& ctx, bool createNewNode)
{
    auto* polylineNode =
        ctx.Make<AST::ACommand>(ctx.MakeToken("polyline"), ctx.MakeToken("endpolyline"));
    polylineNode->PushPositionalArgument(ctx.MakeIdent(GetName())); // 1st positional arg is name
    // 2nd positional arg is point ident vector
    auto pointNames = Points.MapOutput<std::string>([](const auto& output) {
        auto* vs = dynamic_cast<CVertexSelector*>(output.GetOwner());
        return vs->GetPath();
    });
    std::vector<AST::AExpr*> identList;
    for (const auto& pointName : pointNames)
        identList.push_back(ctx.MakeIdent(pointName));
    polylineNode->PushPositionalArgument(ctx.MakeVector(identList));
    return polylineNode;
}

}
