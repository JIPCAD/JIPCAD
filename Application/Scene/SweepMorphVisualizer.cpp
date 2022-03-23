#include "Sweep.h"
#include "math.h"
#include <vector>
#include "SweepMorphVisualizer.h"

namespace Nome::Scene
{

DEFINE_META_OBJECT(CSweepMorphVisualizer)
{
    BindPositionalArgument(&CSweepMorphVisualizer::Sweep, 1, 0);
    BindNamedArgument(&CSweepMorphVisualizer::MorphIndex, "index", 0);
}

void CSweepMorphVisualizer::UpdateEntity()
{
    if (!IsDirty())
        return;

    Super::UpdateEntity();

    // Get SweepMorph and MorphIndex values
    CSweep *sweepInfo = Sweep.GetValue(nullptr);
    int index = round(MorphIndex.GetValue(0));
    if (sweepInfo == nullptr) { return; }

    // Check that the path and index are valid
    CSweepPathInfo *pathInfo = sweepInfo->Path.GetValue(nullptr);
    if (pathInfo == nullptr) { return; }
    int numCutBegin = sweepInfo->bCutBegin ? 1 : 0;
    int numCutEnd = sweepInfo->bCutEnd ? 1 : 0;
    if (index < numCutBegin || index >= pathInfo->Positions.size() - numCutEnd) { return; }

    std::vector<Vertex*> vertices;
    // Control scale at index'th point
    Vector3 controlScale = sweepInfo->controlScales[index];

    for (size_t i = 0; i < sweepInfo->crossSections[index].size(); i++)
    {
        Vector3 point = sweepInfo->crossSections[index][i];
        float x = point.x * controlScale.x;
        float y = point.y * controlScale.y;

        vertices.push_back(AddVertex("v" + std::to_string(index) + "_" + std::to_string(i),
                                     { x, y, 0 }));
    }

    AddLineStrip("crosssection", vertices);
}

}
