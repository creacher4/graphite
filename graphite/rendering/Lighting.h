#pragma once

#include <glm/vec3.hpp>

struct DirectionalLightData
{
    // slot 0
    glm::vec3 dir;
    float pad0;

    // slot 1
    glm::vec3 color;
    float useAlbedo;

    // slot 2
    float useNormals;
    float useAO;
    float pad1;
    float pad2;

    // slot 3
    glm::vec3 viewDir;
    float useRoughness;

    // slot 4
    float useMetallic;
    float useFresnel;
    float useRim;
    float pad3;
};