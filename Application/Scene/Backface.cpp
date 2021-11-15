#include "Backface.h"

namespace Nome::Scene
{

DEFINE_META_OBJECT(CBackface)
{
    BindNamedArgument(&CBackface::ColorR, "color", 0, 0);
    BindNamedArgument(&CBackface::ColorG, "color", 0, 1);
    BindNamedArgument(&CBackface::ColorB, "color", 0, 2);
}

}