#include "Sweep.h"
#include <vector>
#include "SweepControlPoint.h"
#include "SweepMorph.h"

namespace Nome::Scene
{

DEFINE_META_OBJECT(CSweepMorph)
{
    BindPositionalArgument(&CSweepMorph::Path, 1, 0);
    BindPositionalArgument(&CSweepMorph::CrossSection, 2, 0);
    BindNamedArgument(&CSweepMorph::Azimuth, "azimuth", 0);
    BindNamedArgument(&CSweepMorph::Twist, "twist", 0);
    BindNamedArgument(&CSweepMorph::bBeginCap, "begincap", 0);
    BindNamedArgument(&CSweepMorph::bEndCap, "endcap", 0);
    BindNamedArgument(&CSweepMorph::bReverse, "reverse", 0);
    BindNamedArgument(&CSweepMorph::bMintorsion, "mintorsion", 0);
    BindNamedArgument(&CSweepMorph::bCutBegin, "cutbegin", 0);
    BindNamedArgument(&CSweepMorph::bCutEnd, "cutend", 0);
}

void CSweepMorph::MarkDirty()
{
    Super::MarkDirty();
    MorphPath.MarkDirty();
    MorphFirstCrossSection.MarkDirty();
}

void CSweepMorph::UpdateEntity()
{
    if (!IsDirty())
        return;

    CSweepPathInfo *pathInfo = Path.GetValue(nullptr);
    if (pathInfo == nullptr) { return; }

    // Total number of points
    size_t n = pathInfo->Positions.size();

    // Dummy cross-section to pass into Sweep generator
    CSweepPathInfo *firstCrossSection;

    // Find the points with cross-section information
    std::vector<size_t> csPositions;
    for (size_t i = 0; i < n; i++)
    {
        if (dynamic_cast<CSweepControlPointInfo*>(pathInfo->Positions[i]))
        {
            if (dynamic_cast<CSweepControlPointInfo*>(pathInfo->Positions[i])->CrossSection)
                csPositions.push_back(i);
        }
        else
        {
            pathInfo->Positions[i] = new CSweepControlPointInfo(pathInfo->Positions[i]);
        }
    }

    // Number of points with cross-section information
    size_t csn = csPositions.size();

    // Only one point with cross-section info is defined
    if (csn == 1)
    {
        // Assign this cross-section to all points
        size_t t = csPositions[0];
        CSweepPathInfo *sectionInfo = dynamic_cast<CSweepControlPointInfo*>(pathInfo->Positions[t])->CrossSection;
        for (size_t i = 0; i < n; i++)
        {
            if (i != t)
                dynamic_cast<CSweepControlPointInfo*>(pathInfo->Positions[i])->CrossSection = sectionInfo;
        }
        firstCrossSection = sectionInfo;
    }
    else
    {
        // Assign first cross-section to points at the beginning of the sweep
        size_t first_t = csPositions[0];
        CSweepPathInfo *firstSectionInfo = dynamic_cast<CSweepControlPointInfo*>(pathInfo->Positions[first_t])->CrossSection;
        for (size_t t = 0; t < first_t; t++)
        {
            dynamic_cast<CSweepControlPointInfo*>(pathInfo->Positions[t])->CrossSection = firstSectionInfo;
        }
        firstCrossSection = firstSectionInfo;

        // Assign last cross-section to points at the end of the sweep
        size_t last_t = csPositions[csn - 1];
        CSweepPathInfo *lastSectionInfo = dynamic_cast<CSweepControlPointInfo*>(pathInfo->Positions[last_t])->CrossSection;
        for (size_t t = last_t + 1; t < n; t++)
        {
            dynamic_cast<CSweepControlPointInfo*>(pathInfo->Positions[t])->CrossSection = lastSectionInfo;
        }

        // Linearly interpolate cross-sections
        for (size_t i = 0; i < csn - 1; i++)
        {
            size_t a = csPositions[i];
            size_t b = csPositions[i + 1];
            CSweepPathInfo *sectionInfoA = dynamic_cast<CSweepControlPointInfo*>(pathInfo->Positions[a])->CrossSection;
            CSweepPathInfo *sectionInfoB = dynamic_cast<CSweepControlPointInfo*>(pathInfo->Positions[b])->CrossSection;
            for (size_t t = 1; t < (b - a); t++)
            {
                float_t weight = float(t) / (b - a);
                CSweepPathInfo *section = new CSweepPathInfo();
                for (size_t k = 0; k < sectionInfoA->Positions.size(); k++)
                {
                    CVertexInfo *pointA = sectionInfoA->Positions[k];
                    CVertexInfo *pointB = sectionInfoB->Positions[k];
                    CVertexInfo *point = new CVertexInfo();
                    point->Position = (1 - weight) * pointA->Position + weight * pointB->Position;
                    section->Positions.push_back(point);
                }
                dynamic_cast<CSweepControlPointInfo*>(pathInfo->Positions[a + t])->CrossSection = section;
            }
        }
    }

    // Assign the SweepMorph path and cs to the Sweep's path and cs
    Super::Path.Connect(MorphPath);
    Super::CrossSection.Connect(MorphFirstCrossSection);
    MorphPath.UpdateValue(pathInfo);
    MorphFirstCrossSection.UpdateValue(firstCrossSection);

    Super::UpdateEntity();
}

}
