#include "RendererSetup.h"
#include "utils/Logger.h"
#include "ShaderUtils.h"
#include "cfg/Config.h"

namespace ShaderPaths = Config::ShaderPaths;

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

        HRESULT hr = device->CreateRasterizerState(&rd, outDefaultRS.GetAddressOf());

        if (FAILED(hr))
        {
            LOG_CRITICAL("Failed to create default rasterizer state");
            throw std::runtime_error("Failed to create default rasterizer state");
        }

        // wire‐no‐cull
        rd.FillMode = D3D11_FILL_WIREFRAME;
        rd.CullMode = D3D11_CULL_NONE;

        hr = device->CreateRasterizerState(&rd, outWireNoCullRS.GetAddressOf());

        if (FAILED(hr))
        {
            LOG_CRITICAL("Failed to create wireframe rasterizer state");
            throw std::runtime_error("Failed to create wireframe rasterizer state");
        }

        // depth stencil
        D3D11_DEPTH_STENCIL_DESC dsd = {};
        dsd.DepthEnable = TRUE;
        dsd.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
        dsd.DepthFunc = D3D11_COMPARISON_LESS;

        hr = device->CreateDepthStencilState(&dsd, outDepthDS.GetAddressOf());

        if (FAILED(hr))
        {
            LOG_CRITICAL("Failed to create depth stencil state");
            throw std::runtime_error("Failed to create depth stencil state");
        }

        // sampler
        D3D11_SAMPLER_DESC sd = {};
        sd.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
        sd.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
        sd.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
        sd.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;

        hr = device->CreateSamplerState(&sd, outSampler.GetAddressOf());

        if (FAILED(hr))
        {
            LOG_CRITICAL("Failed to create sampler state");
            throw std::runtime_error("Failed to create sampler state");
        }
    }

    void InitGeometryShadersAndLayout(
        ID3D11Device *device,
        HWND hwnd,
        ComPtr<ID3D11VertexShader> &outVS,
        ComPtr<ID3D11PixelShader> &outPS,
        ComPtr<ID3D11InputLayout> &outInputLayout)
    {
        ComPtr<ID3DBlob> vsBlob, psBlob;
        if (!CompileShaderFromFile(ShaderPaths::GEOMETRY_VS, "main", "vs_5_0", vsBlob))
            throw std::runtime_error("GeometryVS compilation failed");

        HRESULT hr = device->CreateVertexShader(
            vsBlob->GetBufferPointer(),
            vsBlob->GetBufferSize(),
            nullptr,
            outVS.GetAddressOf());

        if (FAILED(hr))
        {
            LOG_ERROR("Failed to create vertex shader. HRESULT: {}", hr);
            throw std::runtime_error("Failed to create vertex shader");
        }

        if (!CompileShaderFromFile(ShaderPaths::GEOMETRY_PS, "main", "ps_5_0", psBlob))
            throw std::runtime_error("GeometryPS compilation failed");

        hr = device->CreatePixelShader(
            psBlob->GetBufferPointer(),
            psBlob->GetBufferSize(),
            nullptr,
            outPS.GetAddressOf());

        if (FAILED(hr))
        {
            LOG_ERROR("Failed to create pixel shader. HRESULT: {}", hr);
            throw std::runtime_error("Failed to create pixel shader");
        }

        // input layout
        D3D11_INPUT_ELEMENT_DESC layoutDesc[] = {
            {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
            {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
            {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0},
            {"TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0}};

        hr = device->CreateInputLayout(
            layoutDesc,
            _countof(layoutDesc),
            vsBlob->GetBufferPointer(),
            vsBlob->GetBufferSize(),
            outInputLayout.GetAddressOf());

        if (FAILED(hr))
        {
            LOG_ERROR("Failed to create input layout. HRESULT: {}", hr);
            throw std::runtime_error("Failed to create input layout");
        }
    }

    void InitLightingShaders(
        ID3D11Device *device,
        ComPtr<ID3D11VertexShader> &outVS,
        ComPtr<ID3D11PixelShader> &outPS)
    {
        ComPtr<ID3DBlob> vsBlob, psBlob;
        if (!CompileShaderFromFile(ShaderPaths::LIGHTING_VS, "main", "vs_5_0", vsBlob))
            throw std::runtime_error("LightingVS compilation failed");

        HRESULT hr = device->CreateVertexShader(
            vsBlob->GetBufferPointer(),
            vsBlob->GetBufferSize(),
            nullptr,
            outVS.GetAddressOf());
        if (FAILED(hr))
        {
            LOG_ERROR("Failed to create vertex shader. HRESULT: {}", hr);
            throw std::runtime_error("Failed to create vertex shader");
        }

        if (!CompileShaderFromFile(ShaderPaths::LIGHTING_PS, "main", "ps_5_0", psBlob))
            throw std::runtime_error("LightingPS compilation failed");

        hr = device->CreatePixelShader(
            psBlob->GetBufferPointer(),
            psBlob->GetBufferSize(),
            nullptr,
            outPS.GetAddressOf());

        if (FAILED(hr))
        {
            LOG_ERROR("Failed to create pixel shader. HRESULT: {}", hr);
            throw std::runtime_error("Failed to create pixel shader");
        }
    }
}
