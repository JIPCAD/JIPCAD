#include "Icosphere.h"

//#undef M_PI

namespace Nome::Scene
{

DEFINE_META_OBJECT(CIcosphere)
{
   BindPositionalArgument(&CIcosphere::Scale, 1, 0);
   BindPositionalArgument(&CIcosphere::SubdivisionSteps, 1, 1);
   //BindNamedArgument(&CIcosphere::bInvert, "invert", 1, 2);
}

void CIcosphere::UpdateEntity()
{
    if (!IsDirty())
        return;

    Super::UpdateEntity();
    
    float scale = Scale.GetValue(1.0f);
    float subdivSteps = SubdivisionSteps.GetValue(1.0f);
    scale = 1.0f;

    float phi =  (1 + 5 ^ (1 / 2)) / 2.0;
    std::vector<Vertex*> handles;
    /*
    int i = 1;
    handles.push_back(AddVertex("v" + std::to_string(i), { 1*scale, phi*scale, 0 }));
    i++;
    handles.push_back(AddVertex("v" + std::to_string(i), { -1*scale, phi*scale, 0 }));
    i++;
    handles.push_back(AddVertex("v" + std::to_string(i), { 1*scale, -phi*scale, 0 }));
    i++;
    handles.push_back(AddVertex("v" + std::to_string(i), { -1*scale, -phi*scale, 0 }));
    i++;
    handles.push_back(AddVertex("v" + std::to_string(i), { 0*scale, 1*scale, phi*scale }));
    i++;
    handles.push_back(AddVertex("v" + std::to_string(i), { 0 * scale, -1 * scale, phi * scale }));
    i++;
    handles.push_back(AddVertex("v" + std::to_string(i), { 0, 1 * scale, -phi * scale }));
    i++;
    handles.push_back(AddVertex("v" + std::to_string(i), { 0 * scale, -1 * scale, -phi * scale }));
    i++;
    handles.push_back(AddVertex("v" + std::to_string(i), { phi * scale, 0, 1 * scale }));
    i++;
    handles.push_back(AddVertex("v" + std::to_string(i), { -phi * scale, 0, 1 * scale }));
    i++;
    handles.push_back(AddVertex("v" + std::to_string(i), { phi * scale, 0, -1 * scale }));
    i++;
    handles.push_back(AddVertex("v" + std::to_string(i), { -phi * scale, 0, -1 * scale }));
    AddLineStrip("icosphere", handles);
    std::cout << "huh" << "\n";
    std::vector<std::array<std::string, 3>> faces = {
        { "v1", "v9", "v5" },  { "v1", "v5", "v2" },  { "v1", "v2", "v10" }, { "v1", "v10", "v6" },
        { "v1", "v6", "v9" },  { "v2", "v5", "v12" }, { "v5", "v9", "v8" },  { "v9", "v6", "v7" },
        { "v6", "v10", "v4" }, { "v10", "v2", "v3" }, { "v12", "v5", "v8" }, { "v8", "v9", "v7" },
        { "v7", "v6", "v4" },  { "v4", "v10", "v3" }, { "v3", "v2", "v12" }, { "v12", "v8", "v3" },
        { "v8", "v7", "v4" },  { "v7", "v3", "v9" },  { "v3", "v4", "v12" }, { "v9", "v7", "v12" }
    };
    for (int j = 0; j < 17; j++)
    {
        try
        {
            std::vector<std::string> face;
            face.push_back(faces[j][0]);
            face.push_back(faces[j][1]);
            face.push_back(faces[j][2]);
            std::cout << "hi: " << j << "\n";
            AddFace("f" + j, face);
        }
        catch (const std::runtime_error& e)
        {
            std::cout << "failed at: " << j << "\n";
        }
    }
    
    */
}

}
