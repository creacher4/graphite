#pragma once

#include <d3d11.h>
#include <wrl/client.h>
#include <cstdint>

struct Mesh
{
    Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
    Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;
    uint32_t indexCount = 0;
};
