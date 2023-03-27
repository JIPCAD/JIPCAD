#include "BezierSpline.h"
#include "SweepControlPoint.h"
#include "math.h"

namespace Nome::Scene
{

DEFINE_META_OBJECT(CBezierSpline)
{
    BindPositionalArgument(&CBezierSpline::ControlPoints, 1);
    BindNamedArgument(&CBezierSpline::Segments, "segs", 0);
}

Matrix3 CBezierCurveMath::FrenetFrameAt(float t) { return Matrix3(); }

std::vector<float> CBezierCurveMath::GetDefaultKnots()
{
    std::vector<float> result;
    float step = 1.0f / Segments;
    for (int i = 0; i < Segments; i++)
        result.push_back(i * step);
    result.push_back(1.0f);
    return result;
}

void CBezierCurveMath::DeCasteljauInPlace(float t, std::vector<Vector3>& inputOutput)
{
    size_t sz = inputOutput.size();
    while (sz > 1)
    {
        for (size_t i = 0; i < sz - 1; i++)
        {
            inputOutput[i] = (1 - t) * inputOutput[i] + t * inputOutput[i + 1];
        }
        sz--;
    }
}

std::vector<Vector3> CBezierCurveMath::CalcValues(std::vector<Vector3> input)
{
    std::vector<Vector3> result;
    auto knots = GetDefaultKnots();
    for (float t : knots)
    {
        std::vector<Vector3> temp = input;
        DeCasteljauInPlace(t, temp);
        result.push_back(temp[0]);
    }
    return result;
}

void CBezierSpline::MarkDirty()
{
    // Mark this entity dirty
    Super::MarkDirty();

    // And also mark the Face output dirty
    BezierSpline.MarkDirty();
}

void CBezierSpline::UpdateEntity()
{
    if (!IsDirty())
        return;

    Super::UpdateEntity();

    int n = (int)Segments.GetValue(8.0f);

    Math.ControlPoints.clear();
    Math.Scales.clear();
    Math.Rotates.clear();

    size_t cpn = ControlPoints.GetSize();
    for (size_t i = 0; i < cpn; i++)
    {
        Math.ControlPoints.push_back(ControlPoints.GetValue(i, nullptr)->Position);
        if (dynamic_cast<CSweepControlPointInfo*>(ControlPoints.GetValue(i, nullptr))) {
            Math.Scales.push_back(dynamic_cast<CSweepControlPointInfo*>(ControlPoints.GetValue(i, nullptr))->Scale);
            Math.Rotates.push_back(dynamic_cast<CSweepControlPointInfo*>(ControlPoints.GetValue(i, nullptr))->Rotate);
        } else {
            Math.Scales.push_back({1, 1, 1});
            Math.Rotates.push_back({0, 0, 0});
        }
    }
    Math.Segments = n;
    std::vector<Vector3> positions = Math.CalcValues(Math.ControlPoints);
    std::vector<Vector3> scales = Math.CalcValues(Math.Scales);
    std::vector<Vector3> rotates = Math.CalcValues(Math.Rotates);
    assert(positions.size() == n + 1);
    assert(scales.size() == n + 1);
    assert(rotates.size() == n + 1);

    // Existence of points with cross-section information => path is for a sweep morph
    // Specify cross-section information
    std::vector<CSweepPathInfo*> crosssections(n + 1, NULL);
    std::vector<size_t> csIndices;  // Indices of the points that have cross section info
    for (size_t i = 0; i < cpn; i++)
    {
        // if a cross-section at a control point exists, assign it to the correct sample point
        if (dynamic_cast<CSweepControlPointInfo*>(ControlPoints.GetValue(i, nullptr)) &&
            dynamic_cast<CSweepControlPointInfo*>(ControlPoints.GetValue(i, nullptr))->CrossSection)
        {
            // t is the value at which the control point has greatest influence
            int t = round(n * float(i) / (cpn - 1));

            crosssections[t] = dynamic_cast<CSweepControlPointInfo*>(ControlPoints.GetValue(i, nullptr))->CrossSection;
            csIndices.push_back(t);
        }
    }

    std::vector<Vertex*> handles;
    std::vector<CVertexInfo *> Positions;
    for (int i = 0; i < n + 1; i++)
    {
        handles.push_back(AddVertex("v" + std::to_string(i), positions[i]));
        CSweepControlPointInfo *point = new CSweepControlPointInfo();
        point->Position = positions[i];
        point->Scale = scales[i];
        point->Rotate = rotates[i];
        point->CrossSection = crosssections[i];
        Positions.push_back(point);
    }

    // Sweep path info
    SI.Positions = Positions;
    SI.Name = GetName();
    SI.IsBSpline = false;
    // Initialize or update CrossSectionIndices:
    // Changing the number of segments will change the actual indices but not the number of indices,
    // so the new csIndices and the old cross-section indices (SI.CrossSectionIndices) should be the same size
    if (SI.CrossSectionIndices.empty() ||   // May not have been initialized yet
        SI.CrossSectionIndices.size() == csIndices.size()) // Number of segments may have changed
        SI.CrossSectionIndices = csIndices;
    BezierSpline.UpdateValue(&SI);
    SetValid(true);

    AddLineStrip("curve", handles);
}

}
