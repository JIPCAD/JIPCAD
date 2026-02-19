#include "Arc.h"
#include <cmath>

#undef M_PI

namespace Nome::Scene
{

typedef struct
{
    float x;
    float y;
    float z;
} Point;

DEFINE_META_OBJECT(CArc)
{
    BindPositionalArgument(&CArc::maj_rad, 1, 0);
    BindPositionalArgument(&CArc::min_rad, 1, 1);
    BindPositionalArgument(&CArc::theta_max, 1, 2);
    BindPositionalArgument(&CArc::phi_min, 1, 3);
    BindPositionalArgument(&CArc::phi_max, 1, 4);
    BindPositionalArgument(&CArc::theta_segs, 1, 5);
    BindPositionalArgument(&CArc::phi_segs, 1, 6);
}

void CArc::MarkDirty()
{
    // Mark this entity dirty
    Super::MarkDirty();

    // And also mark the Face output dirty
    Arc.MarkDirty();
}

void CArc::UpdateEntity()
{
    if (!IsDirty())
        return;

    // Clear mesh
    Super::UpdateEntity();
    

    // Initialize torus parameters from document
    float majorRadius = maj_rad.GetValue(1.0f);
    float minorRadius = min_rad.GetValue(1.0f);
    float thetaMax = theta_max.GetValue(1.0f);
    float phiMin = phi_min.GetValue(1.0f);
    float phiMax = phi_max.GetValue(1.0f);
    // number of circles or cross sections
    int thetaSegs = static_cast<int>(theta_segs.GetValue(1.0f));
    int phiSegs = static_cast<int>(phi_segs.GetValue(5.0f));

    const float epsilon = 1e-4;
    const float dt = (thetaMax * (float)tc::M_PI / 180.0f) / (thetaSegs);
    const float du = ((phiMax - phiMin) * (float)tc::M_PI / 180.0f)
        / phiSegs; // convert phiMax to radians then divide by # of segs on circle
    const float du_offset =
        (phiMin * (float)tc::M_PI / 180.0f); // convert phiMin to radians. This will be used to
                                             // offset so it starts at phiMin instead of 0 .
    std::vector<Vertex*> handles;
    std::vector<CVertexInfo*> positions;
    // Create torus, creating one cross section at each iteration
    for (int i = 0; i < thetaSegs + 1;
         i++) // thetaSegs + 1; for some reason thetaSegs was outputting an off by one torus...
    {
        float t0 = i * dt;

        Point p0 = { majorRadius * cos(t0), majorRadius * sinf(t0), 0 };

        Point curr_vertexi;
        curr_vertexi.x = p0.x;
        curr_vertexi.y = p0.y;
        curr_vertexi.z = p0.z;
        Vector3 pos = Vector3(curr_vertexi.x, curr_vertexi.y, curr_vertexi.z);
        CVertexInfo* point = new CVertexInfo();
        point->Position = pos;
        positions.push_back(point);
        std::cout << curr_vertexi.x << ", " << curr_vertexi.y << ", " << curr_vertexi.z << "\n";
        handles.push_back(AddVertex("iv" + std::to_string(i + 1),
                                    { curr_vertexi.x, curr_vertexi.y, curr_vertexi.z }));
    }
    AddLineStrip("arc", handles);
    // Sweep path info
    SI.Positions = positions;
    SI.IsClosed = false;
    SI.IsBSpline = false;
    SI.Name = GetName();
    Arc.UpdateValue(&SI);
    SetValid(true);
}
}
