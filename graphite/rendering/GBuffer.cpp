#include "rendering/GBuffer.h"
#include <stdexcept>
#include "utils/Logger.h"

void GBuffer::Release()
{
    m_texAlbedo.Reset();
    m_rtvAlbedo.Reset();
    m_srvAlbedo.Reset();
    m_texNormal.Reset();
    m_rtvNormal.Reset();
    m_srvNormal.Reset();
    m_texORM.Reset();
    m_rtvORM.Reset();
    m_srvORM.Reset();
    m_texDepth.Reset();
    m_depthDSV.Reset();
    m_srvDepth.Reset();
    m_renderTargetViews.clear();
}

bool GBuffer::Init(ID3D11Device *device, UINT width, UINT height)
{
    Release(); // cleanup if resizing
    m_Width = width;
    m_Height = height;

    HRESULT hr;
    D3D11_TEXTURE2D_DESC desc = {};
    desc.Width = width;
    desc.Height = height;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.SampleDesc.Count = 1;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

    // albedo (RGBA8)
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    device->CreateTexture2D(&desc, nullptr, m_texAlbedo.GetAddressOf());
    hr = device->CreateRenderTargetView(m_texAlbedo.Get(), nullptr, m_rtvAlbedo.GetAddressOf());
    if (FAILED(hr))
    {
        LOG_ERROR("Failed to create albedo render target view");
        return false;
    }
    device->CreateShaderResourceView(m_texAlbedo.Get(), nullptr, m_srvAlbedo.GetAddressOf());

    // normal (RGBA16F)
    desc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
    device->CreateTexture2D(&desc, nullptr, m_texNormal.GetAddressOf());
    hr = device->CreateRenderTargetView(m_texNormal.Get(), nullptr, m_rtvNormal.GetAddressOf());
    if (FAILED(hr))
    {
        LOG_ERROR("Failed to create normal render target view");
        return false;
    }
    device->CreateShaderResourceView(m_texNormal.Get(), nullptr, m_srvNormal.GetAddressOf());

    // ORM (RGBA8)
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    device->CreateTexture2D(&desc, nullptr, m_texORM.GetAddressOf());
    hr = device->CreateRenderTargetView(m_texORM.Get(), nullptr, m_rtvORM.GetAddressOf());
    if (FAILED(hr))
    {
        LOG_ERROR("Failed to create ORM render target view");
        return false;
    }
    device->CreateShaderResourceView(m_texORM.Get(), nullptr, m_srvORM.GetAddressOf());

    // depth (typeless for SRV)
    desc.Format = DXGI_FORMAT_R24G8_TYPELESS;
    desc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
    device->CreateTexture2D(&desc, nullptr, m_texDepth.GetAddressOf());

    D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
    dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    dsvDesc.Texture2D.MipSlice = 0;
    device->CreateDepthStencilView(m_texDepth.Get(), &dsvDesc, m_depthDSV.GetAddressOf());

    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = 1;
    device->CreateShaderResourceView(m_texDepth.Get(), &srvDesc, m_srvDepth.GetAddressOf());

    // MRT array for OMSetRenderTargets
    m_renderTargetViews = {
        m_rtvAlbedo.Get(),
        m_rtvNormal.Get(),
        m_rtvORM.Get()};

    return true;
}

void GBuffer::Bind(ID3D11DeviceContext *context)
{
    context->OMSetRenderTargets(
        static_cast<UINT>(m_renderTargetViews.size()),
        m_renderTargetViews.data(),
        m_depthDSV.Get());

    D3D11_VIEWPORT viewport = {};
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.Width = static_cast<float>(m_Width);
    viewport.Height = static_cast<float>(m_Height);
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;

    context->RSSetViewports(1, &viewport);
}

void GBuffer::Clear(ID3D11DeviceContext *context)
{
    context->ClearRenderTargetView(m_rtvAlbedo.Get(), CC::GBUFFER_ALBEDO.data());
    context->ClearRenderTargetView(m_rtvNormal.Get(), CC::GBUFFER_NORMAL.data());
    context->ClearRenderTargetView(m_rtvORM.Get(), CC::GBUFFER_ORM.data());
    context->ClearDepthStencilView(m_depthDSV.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void GBuffer::Resize(ID3D11Device *device, UINT width, UINT height)
{
    if (!Init(device, width, height))
    {
        LOG_CRITICAL("Failed to resize GBuffer");
    }
}