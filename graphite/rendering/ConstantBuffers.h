#pragma once

#include <glm/glm.hpp>

struct alignas(16) PerFrameData
{
    glm::mat4 viewMatrix;
    glm::mat4 projectionMatrix;
};
static_assert((sizeof(PerFrameData) % 16) == 0, "PerFrameData size must be 16-byte aligned.");

struct alignas(16) PerObjectData
{
    glm::mat4 worldMatrix;
};
static_assert((sizeof(PerObjectData) % 16) == 0, "PerObjectData size must be 16-byte aligned.");
