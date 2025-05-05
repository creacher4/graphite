#include "RendererSetup.h"
#include "utils/Logger.h"
#include "ShaderUtils.h"

// shader file paths
static constexpr const wchar_t *GEOMETRY_VS_FILE = L"shaders/GeometryVS.hlsl";
static constexpr const wchar_t *GEOMETRY_PS_FILE = L"shaders/GeometryPS.hlsl";
static constexpr const wchar_t *LIGHTING_VS_FILE = L"shaders/LightingVS.hlsl";
static constexpr const wchar_t *LIGHTING_PS_FILE = L"shaders/LightingPS.hlsl";

namespace RendererSetup
{
    void InitStateObjects(ID3D11Device *device,
                          ComPtr<ID3D11RasterizerState> &outDefaultRS,
                          ComPtr<ID3D11RasterizerState> &outWireNoCullRS,
                          ComPtr<ID3D11DepthStencilState> &outDepthDS,
                          ComPtr<ID3D11SamplerState> &outSampler)
    {
        // default rasterizer
        D3D11_RASTERIZER_DESC rd = {};
        rd.FillMode = D3D11_FILL_SOLID;
        rd.CullMode = D3D11_CULL_BACK;
        rd.DepthClipEnable = TRUE;
        device->CreateRasterizerState(&rd, outDefaultRS.GetAddressOf());

        // wire‐no‐cull
        rd.FillMode = D3D11_FILL_WIREFRAME;
        rd.CullMode = D3D11_CULL_NONE;
        device->CreateRasterizerState(&rd, outWireNoCullRS.GetAddressOf());

        // depth stencil
        D3D11_DEPTH_STENCIL_DESC dsd = {};
        dsd.DepthEnable = TRUE;
        dsd.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
        dsd.DepthFunc = D3D11_COMPARISON_LESS;
        device->CreateDepthStencilState(&dsd, outDepthDS.GetAddressOf());

        // sampler
        D3D11_SAMPLER_DESC sd = {};
        sd.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
        sd.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
        sd.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
        sd.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
        device->CreateSamplerState(&sd, outSampler.GetAddressOf());
    }

    void InitGeometryShadersAndLayout(
        ID3D11Device *device,
        HWND hwnd,
        ComPtr<ID3D11VertexShader> &outVS,
        ComPtr<ID3D11PixelShader> &outPS,
        ComPtr<ID3D11InputLayout> &outInputLayout)
    {
        ComPtr<ID3DBlob> vsBlob, psBlob;
        if (!CompileShaderFromFile(GEOMETRY_VS_FILE, "main", "vs_5_0", vsBlob))
            throw std::runtime_error("GeometryVS compilation failed");
        device->CreateVertexShader(
            vsBlob->GetBufferPointer(),
            vsBlob->GetBufferSize(),
            nullptr,
            outVS.GetAddressOf());

        if (!CompileShaderFromFile(GEOMETRY_PS_FILE, "main", "ps_5_0", psBlob))
            throw std::runtime_error("GeometryPS compilation failed");
        device->CreatePixelShader(
            psBlob->GetBufferPointer(),
            psBlob->GetBufferSize(),
            nullptr,
            outPS.GetAddressOf());

        // input layout
        D3D11_INPUT_ELEMENT_DESC layoutDesc[] = {
            {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
            {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
            {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0},
            {"TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0}};
        device->CreateInputLayout(
            layoutDesc,
            _countof(layoutDesc),
            vsBlob->GetBufferPointer(),
            vsBlob->GetBufferSize(),
            outInputLayout.GetAddressOf());
    }

    void InitLightingShaders(
        ID3D11Device *device,
        ComPtr<ID3D11VertexShader> &outVS,
        ComPtr<ID3D11PixelShader> &outPS)
    {
        ComPtr<ID3DBlob> vsBlob, psBlob;
        if (!CompileShaderFromFile(LIGHTING_VS_FILE, "main", "vs_5_0", vsBlob))
            throw std::runtime_error("LightingVS compilation failed");
        device->CreateVertexShader(
            vsBlob->GetBufferPointer(),
            vsBlob->GetBufferSize(),
            nullptr,
            outVS.GetAddressOf());

        if (!CompileShaderFromFile(LIGHTING_PS_FILE, "main", "ps_5_0", psBlob))
            throw std::runtime_error("LightingPS compilation failed");
        device->CreatePixelShader(
            psBlob->GetBufferPointer(),
            psBlob->GetBufferSize(),
            nullptr,
            outPS.GetAddressOf());
    }
}
