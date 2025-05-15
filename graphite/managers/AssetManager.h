#pragma once

#include <map>
#include <vector>
#include <string>
#include <wrl/client.h>
#include <d3d11.h>
#include <Windows.h>
#include <assimp/mesh.h>
#include "DeviceManager.h"
#include "rendering/Material.h"
#include "rendering/Vertex.h"
#include "core/CommonTypes.h"

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

    struct ModelResource
    {
        std::vector<MeshResource> meshes;
    };

    AssetManager() = default;
    ~AssetManager() = default;

    void SetDeviceManager(DeviceManager *deviceManager);

    bool LoadTexture(const AssetID &path);
    bool LoadModel(const AssetID &path);
    bool AddMaterial(const AssetID &id, const Material &material);

    const TextureResource *GetTexture(const AssetID &id) const;
    const ModelResource *GetModel(const AssetID &id) const;
    const Material *GetMaterial(const AssetID &id) const;

    void
    Shutdown();

private:
    DeviceManager *m_DeviceManager = nullptr;

    // stores all loaded model.
    // each model can have multiple meshes
    std::map<AssetID, ModelResource> m_Models;
    std::map<AssetID, TextureResource> m_Textures;
    std::map<AssetID, Material> m_Materials;

    // helpers
    bool ProcessAssimpMesh(
        aiMesh *mesh,
        std::vector<Vertex> &outVertices,
        std::vector<uint32_t> &outIndices) const;

    bool CreateMeshResourceBuffers(
        const std::vector<Vertex> &vertices,
        const std::vector<uint32_t> &indices,
        MeshResource &outResource) const;

    unsigned char *LoadImageDataFromFile(
        const AssetID &path,
        int &outWidth,
        int &outHeight,
        int &outChannels) const;

    bool CreateTextureAndSRV(
        int width,
        int height,
        DXGI_FORMAT format,
        unsigned char *pixelData,
        TextureResource &outResource) const;
};