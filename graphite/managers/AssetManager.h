#pragma once

#include <map>
#include <string>
#include <wrl/client.h>
#include <d3d11.h>
#include <Windows.h>
#include "DeviceManager.h"

class AssetManager
{
public:
    struct MeshResource
    {
        Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
        Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;
        UINT indexCount = 0;
    };

    AssetManager() = default;
    ~AssetManager() = default;

    void SetDeviceManager(DeviceManager *deviceManager);
    bool InitPrimitiveMeshes();

    const MeshResource *GetMesh(const std::string &id);

    void Shutdown();

private:
    DeviceManager *m_DeviceManager = nullptr;
    std::map<std::string, MeshResource> m_Meshes;
};