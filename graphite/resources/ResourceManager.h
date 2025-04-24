#pragma once

#include "rendering/Mesh.h"
#include <string>
#include <map>

class ResourceManager
{
public:
    bool InitPrimitiveMeshes(ID3D11Device *device);
    Mesh *GetPrimitiveMesh(const std::string &id);

private:
    std::map<std::string, Mesh> m_PrimitiveMeshes;
};
