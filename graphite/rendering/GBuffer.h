#pragma once

#include "cfg/Config.h"
#include <d3d11.h>
#include <wrl/client.h>
#include <vector>
#include <array>

namespace CC = Config::ClearColors;

class GBuffer
{
public:
    GBuffer() = default;
    ~GBuffer() = default;

    // prevent copy and assignment
    GBuffer(const GBuffer &) = delete;
    GBuffer &operator=(const GBuffer &) = delete;

    bool Init(ID3D11Device *device, UINT width, UINT height);
    void Bind(ID3D11DeviceContext *context);
    void Clear(ID3D11DeviceContext *context);
    void Resize(ID3D11Device *device, UINT width, UINT height);

    ID3D11ShaderResourceView *GetAlbedoSRV() const { return m_srvAlbedo.Get(); }
    ID3D11ShaderResourceView *GetNormalSRV() const { return m_srvNormal.Get(); }
    ID3D11ShaderResourceView *GetOrmSRV() const { return m_srvORM.Get(); }
    ID3D11ShaderResourceView *GetDepthSRV() const { return m_srvDepth.Get(); }

private:
    void Release();

    UINT m_Width = 0;
    UINT m_Height = 0;

    // albedo
    Microsoft::WRL::ComPtr<ID3D11Texture2D> m_texAlbedo;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_rtvAlbedo;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_srvAlbedo;

    // normal
    Microsoft::WRL::ComPtr<ID3D11Texture2D> m_texNormal;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_rtvNormal;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_srvNormal;

    // ORM
    Microsoft::WRL::ComPtr<ID3D11Texture2D> m_texORM;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_rtvORM;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_srvORM;

    // depth
    Microsoft::WRL::ComPtr<ID3D11Texture2D> m_texDepth;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_depthDSV;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_srvDepth;

    std::vector<ID3D11RenderTargetView *> m_renderTargetViews;
};
