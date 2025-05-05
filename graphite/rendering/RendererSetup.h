#pragma once

#include <d3d11.h>
#include <wrl/client.h>

using Microsoft::WRL::ComPtr;

namespace RendererSetup
{
    // create common D3D11 objects
    void InitStateObjects(
        ID3D11Device *device,
        ComPtr<ID3D11RasterizerState> &outDefaultRS,
        ComPtr<ID3D11RasterizerState> &outWireNoCullRS,
        ComPtr<ID3D11DepthStencilState> &outDepthDS,
        ComPtr<ID3D11SamplerState> &outSampler);

    // compile geometry shaders and create input layout
    // vsblob and psblob are optional out params
    void InitGeometryShadersAndLayout(
        ID3D11Device *device,
        HWND hwnd,
        ComPtr<ID3D11VertexShader> &outVS,
        ComPtr<ID3D11PixelShader> &outPS,
        ComPtr<ID3D11InputLayout> &outInputLayout);

    // compile lighting shaders
    void InitLightingShaders(
        ID3D11Device *device,
        ComPtr<ID3D11VertexShader> &outVS,
        ComPtr<ID3D11PixelShader> &outPS);
}