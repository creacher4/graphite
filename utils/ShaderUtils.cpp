#include "ShaderUtils.h"
#include "Logger.h"
#include <stdexcept>
#include <Windows.h>

bool CompileShaderFromFile(
    const std::wstring &filename,
    const std::string &entryPoint,
    const std::string &target,
    Microsoft::WRL::ComPtr<ID3DBlob> &outBlob)
{
    DWORD compileFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined(_DEBUG)
    compileFlags |= D3DCOMPILE_DEBUG;
    compileFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

    Microsoft::WRL::ComPtr<ID3DBlob> errorBlob;
    HRESULT hr = D3DCompileFromFile(
        filename.c_str(),
        nullptr,
        D3D_COMPILE_STANDARD_FILE_INCLUDE,
        entryPoint.c_str(),
        target.c_str(),
        compileFlags,
        0,
        &outBlob,
        &errorBlob);

    if (FAILED(hr))
    {
        if (errorBlob)
        {
            LOG_ERROR("Shader compilation error: {}", static_cast<const char *>(errorBlob->GetBufferPointer()));
        }
        return false;
    }

    return true;
}
