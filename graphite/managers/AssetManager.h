#pragma once

#include <map>
#include <vector>
#include <filesystem>
#include <string>
#include <wrl/client.h>
#include <d3d11.h>
#include <Windows.h>
#include "DeviceManager.h"
#include "rendering/Material.h"

using AssetID = std::filesystem::path;

class AssetManager
{
public:
    struct MeshResource
    {
        Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
        Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;
        UINT indexCount = 0;
    };

    struct TextureResource
    {
        Microsoft::WRL::ComPtr<ID3D11Texture2D> texture;
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv;
        UINT width = 0, height = 0;
    };

    AssetManager() = default;
    ~AssetManager() = default;

    void SetDeviceManager(DeviceManager *deviceManager);
    bool InitPrimitiveMeshes();

    bool LoadTexture(const AssetID &path);
    bool LoadModel(const AssetID &path);
    bool AddMaterial(const AssetID &id, const Material &material);

    const MeshResource *GetMesh(const AssetID &id) const;
    const TextureResource *GetTexture(const AssetID &id) const;
    const Material *GetMaterial(const AssetID &id) const;
    // const MeshResource *GetMesh(const std::string &id);

    void
    Shutdown();

private:
    DeviceManager *m_DeviceManager = nullptr;
    std::map<AssetID, MeshResource> m_Meshes;
    std::map<AssetID, TextureResource> m_Textures;
    std::map<AssetID, Material> m_Materials;
};