#pragma once

#include <glm/vec3.hpp>
#include <glm/vec2.hpp>

struct Vertex
{
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoord;
    glm::vec3 Tangent;
};
// sizeof(Vertex) = 44 bytes
