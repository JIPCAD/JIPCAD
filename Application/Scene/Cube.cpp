#include "Cube.h"

#undef M_PI

namespace Nome::Scene
{

DEFINE_META_OBJECT(CCube)
{
    BindPositionalArgument(&CCube::Height, 1, 0);
    BindNamedArgument(&CCube::bBotCap, "botcap", 0);
    BindNamedArgument(&CCube::bTopCap, "topcap", 0);
}


void CCube::UpdateEntity()
{
    if (!IsDirty())
        return;

    Super::UpdateEntity();

    float height = Height.GetValue(1.0f);
    float halfHeight = height / 2.0f;

    // Add vertices
    AddVertex("top-left-front", { -halfHeight, halfHeight, halfHeight });
    AddVertex("top-left-back", { -halfHeight, halfHeight, -halfHeight });
    AddVertex("top-right-front", { halfHeight, halfHeight, halfHeight });
    AddVertex("top-right-back", { halfHeight, halfHeight, -halfHeight });
    AddVertex("bottom-left-front", { -halfHeight, -halfHeight, halfHeight });
    AddVertex("bottom-left-back", { -halfHeight, -halfHeight, -halfHeight });
    AddVertex("bottom-right-front", { halfHeight, -halfHeight, halfHeight });
    AddVertex("bottom-right-back", { halfHeight, -halfHeight, -halfHeight });

    AddVertex("top-center", {0, 0, 0});
    AddVertex("bottom-center", {0, 0, height});

    // Add faces
    //AddFace("f-0", { "top-left-front", "top-right-front", "bottom-right-front", "bottom-left-front" });
    AddFace("f-0", { "bottom-left-front", "bottom-right-front", "top-right-front", "top-left-front" });
    AddFace("f-1", { "top-left-back", "top-right-back", "bottom-right-back", "bottom-left-back" });
    AddFace("f-2", { "top-left-front", "top-left-back", "bottom-left-back", "bottom-left-front" });
    AddFace("f-3", { "bottom-right-front", "bottom-right-back", "top-right-back", "top-right-front" });

    if (bTopCap) {
        AddFace("f-4", { "top-left-front", "top-right-front", "top-right-back", "top-left-back" });
    }

    if (bBotCap) {
        AddFace("f-5", { "bottom-left-back", "bottom-right-back", "bottom-right-front", "bottom-left-front" });
    }
}


}



