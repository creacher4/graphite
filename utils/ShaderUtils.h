#pragma once

#include <d3d11.h>
#include <d3dcompiler.h>
#include <wrl/client.h>
#include <string>

bool CompileShaderFromFile(
    const std::wstring &filename,
    const std::string &entryPoint,
    const std::string &target,
    Microsoft::WRL::ComPtr<ID3DBlob> &outBlob);
