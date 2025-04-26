#include "AssetManager.h"
#include "DeviceManager.h"
#include "rendering/Vertex.h"
#include <vector>
#include <stdexcept>

void AssetManager::SetDeviceManager(DeviceManager *deviceManager)
{
    m_DeviceManager = deviceManager;
}

bool AssetManager::InitPrimitiveMeshes()
{

    if (!m_DeviceManager || !m_DeviceManager->GetDevice())
    {
        return false;
    }
    auto *device = m_DeviceManager->GetDevice();

    // create cube mesh
    std::vector<Vertex> vertices = {
        {{-0.5f, -0.5f, -0.5f}, {0, 0, -1}, {0, 1}}, {{0.5f, -0.5f, -0.5f}, {0, 0, -1}, {1, 1}}, {{0.5f, 0.5f, -0.5f}, {0, 0, -1}, {1, 0}}, {{-0.5f, 0.5f, -0.5f}, {0, 0, -1}, {0, 0}}, {{-0.5f, -0.5f, 0.5f}, {0, 0, 1}, {1, 1}}, {{-0.5f, 0.5f, 0.5f}, {0, 0, 1}, {1, 0}}, {{0.5f, 0.5f, 0.5f}, {0, 0, 1}, {0, 0}}, {{0.5f, -0.5f, 0.5f}, {0, 0, 1}, {0, 1}}, {{-0.5f, -0.5f, 0.5f}, {-1, 0, 0}, {0, 1}}, {{-0.5f, -0.5f, -0.5f}, {-1, 0, 0}, {1, 1}}, {{-0.5f, 0.5f, -0.5f}, {-1, 0, 0}, {1, 0}}, {{-0.5f, 0.5f, 0.5f}, {-1, 0, 0}, {0, 0}}, {{0.5f, -0.5f, -0.5f}, {1, 0, 0}, {0, 1}}, {{0.5f, -0.5f, 0.5f}, {1, 0, 0}, {1, 1}}, {{0.5f, 0.5f, 0.5f}, {1, 0, 0}, {1, 0}}, {{0.5f, 0.5f, -0.5f}, {1, 0, 0}, {0, 0}}, {{-0.5f, 0.5f, -0.5f}, {0, 1, 0}, {0, 1}}, {{0.5f, 0.5f, -0.5f}, {0, 1, 0}, {1, 1}}, {{0.5f, 0.5f, 0.5f}, {0, 1, 0}, {1, 0}}, {{-0.5f, 0.5f, 0.5f}, {0, 1, 0}, {0, 0}}, {{-0.5f, -0.5f, 0.5f}, {0, -1, 0}, {0, 1}}, {{0.5f, -0.5f, 0.5f}, {0, -1, 0}, {1, 1}}, {{0.5f, -0.5f, -0.5f}, {0, -1, 0}, {1, 0}}, {{-0.5f, -0.5f, -0.5f}, {0, -1, 0}, {0, 0}}};
    std::vector<uint32_t> indices = {
        0, 1, 2, 0, 2, 3, 4, 5, 6, 4, 6, 7,
        8, 9, 10, 8, 10, 11, 12, 13, 14, 12, 14, 15,
        16, 17, 18, 16, 18, 19, 20, 21, 22, 20, 22, 23};

    MeshResource cubeMesh{};
    cubeMesh.indexCount = static_cast<UINT>(indices.size());

    // vertex buffer
    D3D11_BUFFER_DESC vbDesc = {};
    vbDesc.Usage = D3D11_USAGE_IMMUTABLE;
    vbDesc.ByteWidth = sizeof(Vertex) * (UINT)vertices.size();
    vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    D3D11_SUBRESOURCE_DATA vbData{vertices.data()};
    if (FAILED(device->CreateBuffer(&vbDesc, &vbData, cubeMesh.vertexBuffer.GetAddressOf())))
        throw std::runtime_error("AssetManager: vertex buffer creation failed");

    // index buffer
    D3D11_BUFFER_DESC ibDesc = {};
    ibDesc.Usage = D3D11_USAGE_IMMUTABLE;
    ibDesc.ByteWidth = sizeof(uint32_t) * (UINT)indices.size();
    ibDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    D3D11_SUBRESOURCE_DATA ibData{indices.data()};
    if (FAILED(device->CreateBuffer(&ibDesc, &ibData, cubeMesh.indexBuffer.GetAddressOf())))
        throw std::runtime_error("AssetManager: index buffer creation failed");

    m_Meshes["Cube"] = std::move(cubeMesh);
    OutputDebugStringA("[AssetManager] Primitive meshes initialized.\n");
    return true;
}

const AssetManager::MeshResource *AssetManager::GetMesh(const std::string &id)
{
    auto it = m_Meshes.find(id);
    return (it != m_Meshes.end()) ? &it->second : nullptr;
}

void AssetManager::Shutdown()
{
    m_Meshes.clear();
    OutputDebugStringA("[AssetManager] Shutdown complete.\n");
}