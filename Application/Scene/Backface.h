#pragma once
#include "Entity.h"

namespace Nome::Scene
{

class CBackface : public CEntity
    {
    DEFINE_INPUT(float, ColorR) { }
    DEFINE_INPUT(float, ColorG) { }
    DEFINE_INPUT(float, ColorB) { }

    public:
        DECLARE_META_CLASS(CBackface, CEntity);

    using CEntity::CEntity;
    };

}