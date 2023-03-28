#pragma once
#include "BezierSpline.h"
#include "SweepPath.h"

namespace Nome::Scene
{

class CBSplineMath : public IParametricCurve
{
public:
    /// Get the frenet frame at t
    Matrix3 FrenetFrameAt(float t) override;

    /// Get a default list of t values to use
    std::vector<float> GetDefaultKnots() override;
};

class CBSpline : public CSweepPath
{
    DEFINE_INPUT(float, Order) { MarkDirty(); }
    DEFINE_INPUT(float, Segments) { MarkDirty(); }
    DEFINE_INPUT_ARRAY(CVertexInfo*, ControlPoints) { MarkDirty(); }

    DEFINE_OUTPUT_WITH_UPDATE(IParametricCurve*, Spline)
    {
        UpdateEntity();
        Spline.UpdateValue(&Math);
    }
    DEFINE_OUTPUT_WITH_UPDATE(CSweepPathInfo*, BSpline) { UpdateEntity(); }

public:
    DECLARE_META_CLASS(CBSpline, CSweepPath);

    CBSpline() = default;
    explicit CBSpline(const std::string& name)
        : CSweepPath(name)
    {
    }

    /// Set whether the bspline is a closed loop
    void SetClosed(bool closed);
    std::vector<float> GetDefKnots();
    void UpdateEntity() override;
    void MarkDirty() override;
    void Draw(IDebugDraw* draw, bool toggle_wireframe) override;
    float NFactor(int i, int j, float t);
    std::vector<Vector3> GetMidpoints(std::vector<Vector3> vertices);

private:
    CBSplineMath Math;
    bool bClosed = false;
    std::vector<Vector3> SamplePositions;
    std::vector<Vector3> SampleScales;
    std::vector<Vector3> SampleRotates;
};

}
