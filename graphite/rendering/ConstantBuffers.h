#pragma once

#include <glm/glm.hpp>

struct alignas(16) PerFrameData
{
    glm::mat4 viewMatrix;
    glm::mat4 projectionMatrix;
};

struct alignas(16) PerObjectData
{
    glm::mat4 worldMatrix;
};
