#include "Cone.h"

#undef M_PI

namespace Nome::Scene
{

DEFINE_META_OBJECT(CCone)
{
    BindPositionalArgument(&CCone::Height, 1, 0);
    BindPositionalArgument(&CCone::Radius, 1, 1);
    BindPositionalArgument(&CCone::PolySides, 1, 2);
}

void CCone::UpdateEntity()
{
    if (!IsDirty())
        return;

    Super::UpdateEntity();

    float height = Height.GetValue(1.0f);
    float radius = Radius.GetValue(1.0f);
    int polySides = PolySides.GetValue(3.0f);

    // Add vertices

    float angle = 0.0f;

    for (int i = 0; i < polySides; i++){
        float x = radius * cosf(angle);
        float y = radius * sinf(angle);

        AddVertex("v" + std::to_string(i), { x, y, 0 });

        angle += 2.0f * tc::M_PI / polySides; // Move to next vertex
    }

    // Add top vertex

    AddVertex("vTop", { 0, 0, height });

    // Add faces

    for (int i = 0; i < polySides; i++){
        AddFace("f" + std::to_string(i), {"v" + std::to_string((i + 1) % polySides), "vTop", "v" + std::to_string(i)});
    }

    // Add bottom face
    std::vector<std::string> face = {};
    for (int i = polySides - 1; i >= 0; i--){
        face.push_back("v" + std::to_string(i));
    }
    AddFace("fBottom", face);

}

}