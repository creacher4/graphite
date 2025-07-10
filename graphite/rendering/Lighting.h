#pragma once

#include <glm/vec3.hpp>

struct DirectionalLightData
{
    // slot 0
    glm::vec3 dir;
    float pad0;

    // slot 1
    glm::vec3 color;
    int useAlbedo;

    // slot 2
    int useNormals;
    int useAO;
    int pad1;
    int pad2;

    // slot 3
    glm::vec3 viewDir;
    int useRoughness;

    // slot 4
    int useMetallic;
    int useFresnel;
    int useRim;
    int pad3;
};
static_assert((sizeof(DirectionalLightData) % 16) == 0, "DirectionalLightData size must be 16-byte aligned.");