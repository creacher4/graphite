#include "ResourceManager.h"
#include "rendering/Vertex.h"
#include <vector>
#include <stdexcept>

bool ResourceManager::InitPrimitiveMeshes(ID3D11Device *device)
{
    Mesh cubeMesh;

    std::vector<Vertex> vertices = {
        // front
        {{-0.5f, -0.5f, -0.5f}, {0, 0, -1}, {0, 1}},
        {{0.5f, -0.5f, -0.5f}, {0, 0, -1}, {1, 1}},
        {{0.5f, 0.5f, -0.5f}, {0, 0, -1}, {1, 0}},
        {{-0.5f, 0.5f, -0.5f}, {0, 0, -1}, {0, 0}},
        // back
        {{-0.5f, -0.5f, 0.5f}, {0, 0, 1}, {1, 1}},
        {{-0.5f, 0.5f, 0.5f}, {0, 0, 1}, {1, 0}},
        {{0.5f, 0.5f, 0.5f}, {0, 0, 1}, {0, 0}},
        {{0.5f, -0.5f, 0.5f}, {0, 0, 1}, {0, 1}},
        // left
        {{-0.5f, -0.5f, 0.5f}, {-1, 0, 0}, {0, 1}},
        {{-0.5f, -0.5f, -0.5f}, {-1, 0, 0}, {1, 1}},
        {{-0.5f, 0.5f, -0.5f}, {-1, 0, 0}, {1, 0}},
        {{-0.5f, 0.5f, 0.5f}, {-1, 0, 0}, {0, 0}},
        // right
        {{0.5f, -0.5f, -0.5f}, {1, 0, 0}, {0, 1}},
        {{0.5f, -0.5f, 0.5f}, {1, 0, 0}, {1, 1}},
        {{0.5f, 0.5f, 0.5f}, {1, 0, 0}, {1, 0}},
        {{0.5f, 0.5f, -0.5f}, {1, 0, 0}, {0, 0}},
        // top
        {{-0.5f, 0.5f, -0.5f}, {0, 1, 0}, {0, 1}},
        {{0.5f, 0.5f, -0.5f}, {0, 1, 0}, {1, 1}},
        {{0.5f, 0.5f, 0.5f}, {0, 1, 0}, {1, 0}},
        {{-0.5f, 0.5f, 0.5f}, {0, 1, 0}, {0, 0}},
        // bottom
        {{-0.5f, -0.5f, 0.5f}, {0, -1, 0}, {0, 1}},
        {{0.5f, -0.5f, 0.5f}, {0, -1, 0}, {1, 1}},
        {{0.5f, -0.5f, -0.5f}, {0, -1, 0}, {1, 0}},
        {{-0.5f, -0.5f, -0.5f}, {0, -1, 0}, {0, 0}},
    };

    std::vector<uint32_t> indices = {
        0, 1, 2, 0, 2, 3,       // front
        4, 5, 6, 4, 6, 7,       // back
        8, 9, 10, 8, 10, 11,    // left
        12, 13, 14, 12, 14, 15, // right
        16, 17, 18, 16, 18, 19, // top
        20, 21, 22, 20, 22, 23  // bottom
    };

    cubeMesh.indexCount = static_cast<uint32_t>(indices.size());

    // create vertex buffer
    D3D11_BUFFER_DESC vbDesc = {};
    vbDesc.Usage = D3D11_USAGE_IMMUTABLE;
    vbDesc.ByteWidth = static_cast<UINT>(sizeof(Vertex) * vertices.size());
    vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

    D3D11_SUBRESOURCE_DATA vbData = {};
    vbData.pSysMem = vertices.data();

    HRESULT hr = device->CreateBuffer(&vbDesc, &vbData, cubeMesh.vertexBuffer.GetAddressOf());
    if (FAILED(hr))
    {
        throw std::runtime_error("Failed to create vertex buffer for cube mesh.");
    }

    // create index buffer
    D3D11_BUFFER_DESC ibDesc = {};
    ibDesc.Usage = D3D11_USAGE_IMMUTABLE;
    ibDesc.ByteWidth = UINT(indices.size() * sizeof(uint32_t));
    ibDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

    D3D11_SUBRESOURCE_DATA ibData = {};
    ibData.pSysMem = indices.data();

    hr = device->CreateBuffer(&ibDesc, &ibData, cubeMesh.indexBuffer.GetAddressOf());
    if (FAILED(hr))
    {
        throw std::runtime_error("Failed to create index buffer for cube mesh.");
    }

    m_PrimitiveMeshes["Cube"] = std::move(cubeMesh);

    return true;
}

Mesh *ResourceManager::GetPrimitiveMesh(const std::string &id)
{
    auto it = m_PrimitiveMeshes.find(id);
    return (it != m_PrimitiveMeshes.end()) ? &it->second : nullptr;
}
