#pragma once
#include "core/CommonTypes.h"

struct Material
{
    AssetID albedo;
    AssetID normal;
    AssetID orm;

    float baseColor[4] = {1, 1, 1, 1};
    float roughness = 1.0f;
    float metallic = 0.0f;
};