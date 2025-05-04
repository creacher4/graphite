#include "AssetManager.h"
#include "DeviceManager.h"
#include "utils/Logger.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <assimp/Importer.hpp>
#include <assimp/mesh.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <stdexcept>

void AssetManager::SetDeviceManager(
    DeviceManager *deviceManager)
{
    m_DeviceManager = deviceManager;
}

// const AssetManager::MeshResource *AssetManager::GetMesh(const std::string &id)
// {
//     auto it = m_Meshes.find(id);
//     return (it != m_Meshes.end()) ? &it->second : nullptr;
// }

void AssetManager::Shutdown()
{
    m_Meshes.clear();
    LOG_INFO("AssetManager: Meshes cleared");
}

bool AssetManager::LoadTexture(
    const AssetID &path)
{
    if (m_Textures.count(path))
        return true;

    int w, h, c;
    auto *data = LoadImageDataFromFile(path, w, h, c);
    if (!data)
    {
        LOG_CRITICAL("Failed to load texture data from file: {}", path.string());
        throw std::runtime_error("Failed to load texture: " + path.string());
    }

    TextureResource res;
    if (!CreateTextureAndSRV(w, h, DXGI_FORMAT_R8G8B8A8_UNORM, data, res))
    {
        LOG_CRITICAL("Failed to create texture and SRV");
        throw std::runtime_error("Failed to create texture and SRV: " + path.string());
    }

    m_Textures[path] = std::move(res);
    return true;
}

bool AssetManager::LoadModel(
    const AssetID &path)
{
    if (m_Meshes.count(path))
        return true;

    Assimp::Importer importer;
    unsigned flags =
        aiProcess_Triangulate | aiProcess_CalcTangentSpace | aiProcess_JoinIdenticalVertices | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_ValidateDataStructure | aiProcess_FlipWindingOrder;
    const aiScene *scene = importer.ReadFile(
        path.string(),
        flags);

    if (!scene || !scene->HasMeshes())
    {
        LOG_CRITICAL("AssetManager Failed to load model: {}", path.string());
        throw std::runtime_error("Failed to load model: " + path.string());
    }

    aiMesh *mesh = scene->mMeshes[0]; // take first mesh for simplicity
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;

    if (!ProcessAssimpMesh(mesh, vertices, indices))
    {
        LOG_CRITICAL("AssetManager Failed to process mesh: {}", path.string());
        throw std::runtime_error("Failed to process mesh: " + path.string());
    }

    MeshResource mr;
    if (!CreateMeshResourceBuffers(vertices, indices, mr))
    {
        LOG_CRITICAL("AssetManager Failed to create mesh resource buffers: {}", path.string());
        throw std::runtime_error("Failed to create mesh resource buffers: " + path.string());
    }

    m_Meshes[path] = std::move(mr);
    LOG_INFO("Model loaded succesfully");
    return true;
}

bool AssetManager::AddMaterial(
    const AssetID &id,
    const Material &material)
{
    m_Materials[id] = material;
    return true;
}

const Material *AssetManager::GetMaterial(
    const AssetID &id) const
{
    auto it = m_Materials.find(id);
    return it != m_Materials.end() ? &it->second : nullptr;
}

const AssetManager::MeshResource *AssetManager::GetMesh(
    const AssetID &id) const
{
    auto it = m_Meshes.find(id);
    return (it != m_Meshes.end()) ? &it->second : nullptr;
}

const AssetManager::TextureResource *AssetManager::GetTexture(
    const AssetID &id) const
{
    auto it = m_Textures.find(id);
    return (it != m_Textures.end()) ? &it->second : nullptr;
}

bool AssetManager::ProcessAssimpMesh(
    aiMesh *mesh,
    std::vector<Vertex> &outVertices,
    std::vector<uint32_t> &outIndices) const
{
    if (!mesh->HasPositions())
        return false;

    outVertices.reserve(mesh->mNumVertices);
    for (unsigned i = 0; i < mesh->mNumVertices; ++i)
    {
        Vertex v{};

        v.Position = {mesh->mVertices[i].x,
                      mesh->mVertices[i].y,
                      mesh->mVertices[i].z};

        if (mesh->HasNormals())
        {
            v.Normal = {mesh->mNormals[i].x,
                        mesh->mNormals[i].y,
                        mesh->mNormals[i].z};
        }

        if (mesh->HasTextureCoords(0))
        {
            v.TexCoord = {mesh->mTextureCoords[0][i].x,
                          mesh->mTextureCoords[0][i].y};
        }

        if (mesh->HasTangentsAndBitangents())
        {
            v.Tangent = {mesh->mTangents[i].x,
                         mesh->mTangents[i].y,
                         mesh->mTangents[i].z};
        }

        outVertices.push_back(v);
    }

    outIndices.reserve(mesh->mNumFaces * 3);
    for (unsigned f = 0; f < mesh->mNumFaces; ++f)
    {
        const auto &face = mesh->mFaces[f];
        outIndices.push_back(face.mIndices[0]);
        outIndices.push_back(face.mIndices[1]);
        outIndices.push_back(face.mIndices[2]);
    }

    return true;
}

bool AssetManager::CreateMeshResourceBuffers(
    const std::vector<Vertex> &vertices,
    const std::vector<uint32_t> &indices,
    MeshResource &outResource) const
{
    auto *device = m_DeviceManager->GetDevice();
    if (!device)
        return false;

    // fill index count
    // using static cast to avoid narrowing conversion preemptively
    outResource.indexCount = static_cast<UINT>(indices.size());

    // vertex buffer
    D3D11_BUFFER_DESC vbDesc{};
    vbDesc.Usage = D3D11_USAGE_IMMUTABLE;
    vbDesc.ByteWidth = UINT(sizeof(Vertex) * vertices.size());
    vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    D3D11_SUBRESOURCE_DATA vbData{vertices.data(), 0, 0};
    if (FAILED(device->CreateBuffer(&vbDesc, &vbData, outResource.vertexBuffer.GetAddressOf())))
    {
        LOG_ERROR("Vertex buffer creation failed");
        return false;
    }

    // index buffer
    D3D11_BUFFER_DESC ibDesc{};
    ibDesc.Usage = D3D11_USAGE_IMMUTABLE;
    ibDesc.ByteWidth = UINT(sizeof(uint32_t) * indices.size());
    ibDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    D3D11_SUBRESOURCE_DATA ibData{indices.data(), 0, 0};
    if (FAILED(device->CreateBuffer(&ibDesc, &ibData, outResource.indexBuffer.GetAddressOf())))
    {
        LOG_ERROR("Index buffer creation failed");
        return false;
    }

    return true;
}

unsigned char *AssetManager::LoadImageDataFromFile(
    const AssetID &path,
    int &outWidth,
    int &outHeight,
    int &outChannels) const
{
    unsigned char *data = stbi_load(
        path.string().c_str(),
        &outWidth,
        &outHeight,
        &outChannels,
        4); // 4 channels (rgba)

    if (!data)
    {
        LOG_ERROR("Failed to load image data from file: {}", path.string());
    }

    return data;
}

bool AssetManager::CreateTextureAndSRV(
    int width,
    int height,
    DXGI_FORMAT format,
    unsigned char *pixelData,
    TextureResource &outResource) const
{
    auto *device = m_DeviceManager->GetDevice();
    if (!device)
    {
        stbi_image_free(pixelData);
        return false;
    }

    // texture description
    D3D11_TEXTURE2D_DESC texDesc{};
    texDesc.Width = width;
    texDesc.Height = height;
    texDesc.MipLevels = 1;
    texDesc.ArraySize = 1;
    texDesc.Format = format;
    texDesc.SampleDesc = {1, 0};
    texDesc.Usage = D3D11_USAGE_IMMUTABLE;
    texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

    D3D11_SUBRESOURCE_DATA initData{};
    initData.pSysMem = pixelData;
    initData.SysMemPitch = width * 4; // 4 bytes per pixel (RGBA)
    initData.SysMemSlicePitch = 0;    // not used for 2D textures

    // create texture
    Microsoft::WRL::ComPtr<ID3D11Texture2D> tex;
    HRESULT hr = device->CreateTexture2D(&texDesc, &initData, tex.GetAddressOf());
    if (FAILED(hr))
    {
        LOG_ERROR("CreateTexture2D failed: {}", hr);
        stbi_image_free(pixelData);
        return false;
    }

    // create shader resource view
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv;
    hr = device->CreateShaderResourceView(
        tex.Get(),
        nullptr,
        srv.GetAddressOf());
    if (FAILED(hr))
    {
        LOG_ERROR("CreateShaderResourceView failed: {}", hr);
        stbi_image_free(pixelData);
        return false;
    }

    // populate the texture resource, then free the pixel data
    outResource.texture = std::move(tex);
    outResource.srv = std::move(srv);
    outResource.width = width;
    outResource.height = height;

    stbi_image_free(pixelData);
    return true;
}
