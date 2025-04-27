#include "AssetManager.h"
#include "DeviceManager.h"
#include "rendering/Vertex.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
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

// const AssetManager::MeshResource *AssetManager::GetMesh(const std::string &id)
// {
//     auto it = m_Meshes.find(id);
//     return (it != m_Meshes.end()) ? &it->second : nullptr;
// }

void AssetManager::Shutdown()
{
    m_Meshes.clear();
    OutputDebugStringA("[AssetManager] Shutdown complete.\n");
}

bool AssetManager::LoadTexture(const AssetID &path)
{
    if (m_Textures.count(path))
        return true;

    int w, h, comp;
    unsigned char *data = stbi_load(path.string().c_str(), &w, &h, &comp, 4);
    if (!data)
    {
        OutputDebugStringA("[AssetManager] Failed to load texture.\n");
        OutputDebugStringA(path.string().c_str());
        OutputDebugStringA("\n");
        return false;
    }

    D3D11_TEXTURE2D_DESC desc{};
    desc.Width = w;
    desc.Height = h;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.SampleDesc.Count = 1;
    desc.SampleDesc.Quality = 0;
    desc.Usage = D3D11_USAGE_IMMUTABLE;
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    desc.CPUAccessFlags = 0;
    desc.MiscFlags = 0;

    D3D11_SUBRESOURCE_DATA sd{data, UINT(4 * w), 0};
    auto dev = m_DeviceManager->GetDevice();
    Microsoft::WRL::ComPtr<ID3D11Texture2D> tex;
    if (FAILED(dev->CreateTexture2D(&desc, &sd, tex.GetAddressOf())))
    {
        stbi_image_free(data);
        OutputDebugStringA("[AssetManager] Failed to create texture.\n");
        return false;
    }
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv;
    dev->CreateShaderResourceView(tex.Get(), nullptr, srv.GetAddressOf());

    m_Textures[path] = {std::move(tex), std::move(srv), UINT(w), UINT(h)};
    stbi_image_free(data);
    return true;
}

bool AssetManager::LoadModel(const AssetID &path)
{
    if (m_Meshes.count(path))
        return true;

    Assimp::Importer importer;
    unsigned flags =
        aiProcess_Triangulate | aiProcess_CalcTangentSpace | aiProcess_JoinIdenticalVertices | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_ValidateDataStructure | aiProcess_FlipWindingOrder;
    const aiScene *scene = importer.ReadFile(
        path.string(),
        flags);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::string error = importer.GetErrorString();
        OutputDebugStringA("[AssetManager] Failed to load model: ");
        OutputDebugStringA(error.c_str());
        OutputDebugStringA("\n");
        return false;
    }

    if (!scene || !scene->HasMeshes())
    {
        OutputDebugStringA("[AssetManager] Failed to load model.\n");
        return false;
    }

    aiMesh *mesh = scene->mMeshes[0]; // take first mesh for simplicity
    if (!mesh->HasPositions())
        return false;
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;

    for (unsigned int i = 0; i < mesh->mNumVertices; ++i)
    {
        Vertex v;
        v.Position = {mesh->mVertices[i].x,
                      mesh->mVertices[i].y,
                      mesh->mVertices[i].z};
        v.Normal = mesh->HasNormals()
                       ? glm::vec3(mesh->mNormals[i].x,
                                   mesh->mNormals[i].y,
                                   mesh->mNormals[i].z)
                       : glm::vec3{0, 0, 0};
        v.TexCoord = mesh->HasTextureCoords(0)
                         ? glm::vec2(mesh->mTextureCoords[0][i].x,
                                     mesh->mTextureCoords[0][i].y)
                         : glm::vec2{0.0f};
        v.Tangent = mesh->HasTangentsAndBitangents()
                        ? glm::vec3(mesh->mTangents[i].x,
                                    mesh->mTangents[i].y,
                                    mesh->mTangents[i].z)
                        : glm::vec3{1, 0, 0};
        vertices.push_back(v);
    }

    for (unsigned f = 0; f < mesh->mNumFaces; ++f)
    {
        auto &face = mesh->mFaces[f];
        indices.push_back(face.mIndices[0]);
        indices.push_back(face.mIndices[1]);
        indices.push_back(face.mIndices[2]);
    }

    MeshResource mr;
    mr.indexCount = (UINT)indices.size();

    // vertex buffer
    D3D11_BUFFER_DESC vbDesc{};
    vbDesc.ByteWidth = sizeof(Vertex) * (UINT)vertices.size();
    vbDesc.Usage = D3D11_USAGE_IMMUTABLE;
    vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

    D3D11_SUBRESOURCE_DATA vbData{};
    vbData.pSysMem = vertices.data();
    if (FAILED(m_DeviceManager->GetDevice()->CreateBuffer(&vbDesc, &vbData, mr.vertexBuffer.GetAddressOf())))
    {
        OutputDebugStringA("[AssetManager] Vertex buffer creation failed.\n");
        return false;
    }

    // index buffer
    D3D11_BUFFER_DESC ib{sizeof(uint32_t) * (UINT)indices.size(), D3D11_USAGE_IMMUTABLE, D3D11_BIND_INDEX_BUFFER};
    D3D11_SUBRESOURCE_DATA ibData{indices.data(), 0, 0};
    if (FAILED(m_DeviceManager->GetDevice()->CreateBuffer(&ib, &ibData, mr.indexBuffer.GetAddressOf())))
    {
        OutputDebugStringA("[AssetManager] Index buffer creation failed.\n");
        return false;
    }

    m_Meshes[path] = std::move(mr);
    OutputDebugStringA("[AssetManager] Model loaded successfully.\n");
    return true;
}

bool AssetManager::AddMaterial(const AssetID &id, const Material &material)
{
    m_Materials[id] = material;
    return true;
}

const Material *AssetManager::GetMaterial(const AssetID &id) const
{
    auto it = m_Materials.find(id);
    return it != m_Materials.end() ? &it->second : nullptr;
}

const AssetManager::MeshResource *AssetManager::GetMesh(const AssetID &id) const
{
    auto it = m_Meshes.find(id);
    return (it != m_Meshes.end()) ? &it->second : nullptr;
}

const AssetManager::TextureResource *AssetManager::GetTexture(const AssetID &id) const
{
    auto it = m_Textures.find(id);
    return (it != m_Textures.end()) ? &it->second : nullptr;
}