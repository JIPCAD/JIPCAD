#include "BSpline.h"
#include "SweepControlPoint.h"

namespace Nome::Scene {

DEFINE_META_OBJECT(CBSpline)
{
    BindNamedArgument(&CBSpline::Order, "order", 0);
    BindPositionalArgument(&CBSpline::ControlPoints, 1);
    BindNamedArgument(&CBSpline::Segments, "segs", 0);

}

void CBSpline::MarkDirty()
{
    // Mark this entity dirty
    Super::MarkDirty();

    // And also mark the Face output dirty
    BSpline.MarkDirty();
}


Matrix3 CBSplineMath::FrenetFrameAt(float t) {
    return Matrix3();
}

void CBSpline::SetClosed(bool closed) {
    bClosed = closed;
}

std::vector<float> CBSplineMath::GetDefaultKnots() {
    std::vector<float> result = {0.0f, 0.5f, 1.0f};
    return result;
}

std::vector<float> CBSpline::GetDefKnots() {
    float numKnots = Order.GetValue(3) + ((float) ControlPoints.GetSize());
    std::vector<float> result = {};
    for (int i = 0; i < numKnots; i++) {
        result.push_back(i);
    }
    return result;
}

float CBSpline::NFactor(int i, int k, float t) {
    std::vector<float> knots = GetDefKnots();
    if (k == 1) {
        if (knots[i - 1] <= t && t < knots[i]) {
            return 1;
        } else {
            return 0;
        }
    }
    float numA = (t - knots[i-1]);
    float denA = (knots[i + k - 2] - knots[i-1]);
    float numB = (knots[i + k - 1] - t);
    float denB = (knots[i + k - 1] - knots[i]);
    float weightA = 0;
    float weightB = 0;

    if (denA != 0) {
        weightA = (numA / denA) * NFactor(i, k-1, t);
    }
    if (denB != 0) {
        weightB = (numB / denB) * NFactor(i + 1, k-1, t);
    }
    return weightA + weightB;
}


void CBSpline::UpdateEntity() {
    if (!IsDirty())
        return;

    Super::UpdateEntity();

    int n = (int) Segments.GetValue(8.0f);
    size_t howMany = ControlPoints.GetSize();
    int order = (int) Order.GetValue(3);
    SamplePositions.clear();
    SampleScales.clear();
    SampleRotates.clear();

    // Calculate positions and smoothing for scale/rotate at sweep control points
    for (float steps = 0; steps <= n; steps++) {

        float t =  ((steps / Segments.GetValue(8.0f)) * (howMany - order + 1)) + (order - 1);

        Vector3 ret = {0,0,0};
        Vector3 retScale = {0,0,0};
        Vector3 retRotate = {0,0,0};

        for (int i = 0; i < howMany; i++) {
            float weight = NFactor(i+1,order,t);

            Vector3 ControlPointScale = {1, 1, 1};
            Vector3 ControlPointRotate = {0, 0, 0};
            if (dynamic_cast<CSweepControlPointInfo*>(ControlPoints.GetValue(i, nullptr))) {
                ControlPointScale = dynamic_cast<CSweepControlPointInfo*>(ControlPoints.GetValue(i, nullptr))->Scale;
                ControlPointRotate = dynamic_cast<CSweepControlPointInfo*>(ControlPoints.GetValue(i, nullptr))->Rotate;
            }

            ret += weight * ControlPoints.GetValue(i, nullptr)->Position;
            retScale += weight * ControlPointScale;
            retRotate += weight * ControlPointRotate;
        }

        SamplePositions.emplace_back(ret.x, ret.y, ret.z);
        SampleScales.emplace_back(retScale.x, retScale.y, retScale.z);
        SampleRotates.emplace_back(retRotate.x, retRotate.y, retRotate.z);
    }

    std::vector<Vertex*> handles;
    handles.reserve(n + 1);

    std::vector<CVertexInfo *> positions;
    for (int i = 0; i < n + 1; i++)
    {
        handles.push_back(AddVertex("v" + std::to_string(i), SamplePositions[i]));
        CSweepControlPointInfo *point = new CSweepControlPointInfo();
        point->Position = SamplePositions[i];
        point->Scale = SampleScales[i];
        point->Rotate = SampleRotates[i];
        positions.push_back(point);
    }

    // Sweep path info
    SI.Positions = positions;
    SI.Name = GetName();
    SI.IsClosed = bClosed;
    BSpline.UpdateValue(&SI);
    SetValid(true);

    AddLineStrip("curve", handles);
}



void CBSpline::Draw(IDebugDraw* draw, bool toggle_wireframe)
{
    tc::Color c1 = tc::Color::YELLOW;
    tc::Color c2 = tc::Color::RED;
    auto iter = SamplePositions.begin();
    while (iter != SamplePositions.end() - 1)
    {
        const auto& p0 = *iter;
        const auto& p1 = *(++iter);
        draw->LineSegment(p0, c1, p1, c2);
        std::swap(c1, c2);
    }
}

}
