# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),  
and this project adheres to [Semantic Versioning](https://semver.org/).

---

## [Unreleased]

---

## [0.2.0] – 2025-04-25

### Added

- Deferred Rendering Pipeline:
  - Implemented `GBuffer` class managing Albedo (`RGBA8`), Normal (`RGBA16F`), and ORM (`RGBA8`) render targets, plus a Depth buffer (`D24S8`)
  - GBuffer includes creation of Textures, Render Target Views (RTVs), Depth Stencil View (DSV), and Shader Resource Views (SRVs)
  - Added `GBuffer::Bind` and `GBuffer::Clear` methods for pipeline setup
- Shader System:
  - Introduced HLSL shaders: `GeometryVS.hlsl` for vertex transformation and `GeometryPS.hlsl` for outputting to G-Buffer targets
  - Added `ShaderUtils` helper for compiling HLSL shaders using `D3DCompileFromFile` with debug/release flags
  - Renderer now compiles and creates `ID3D11VertexShader` and `ID3D11PixelShader` objects
- Geometry Pass:
  - Implemented `Renderer::GeometryPass` function to handle drawing scene geometry
  - `GeometryPass` iterates ECS entities with `TransformComponent` and `MeshComponent`
  - Binds appropriate pipeline states (Shaders, IA Layout, Rasterizer, Depth State)
  - Binds vertex/index buffers from `ResourceManager` based on `MeshComponent`
  - Issues `DrawIndexed` calls to render geometry into the G-Buffer
- GPU Data Transfer:
  - Added `ConstantBuffers.h` defining `PerFrameData` (View, Projection) and `PerObjectData` (World matrix) structs with `alignas(16)`
  - Renderer creates and manages dynamic `ID3D11Buffer` constant buffers (`m_cbPerFrame`, `m_cbPerObject`)
  - Implemented `Renderer::UpdatePerFrameConstants` to update frame data via `Map`/`Unmap`
  - `GeometryPass` calculates world matrices from `TransformComponent` and updates the per-object constant buffer
- Pipeline State Management:
  - Added explicit creation and use of `ID3D11RasterizerState` (Solid fill, No culling)
  - Added explicit creation and use of `ID3D11DepthStencilState` (Depth test/write enabled, `LESS` comparison)
  - Created `ID3D11InputLayout` based on `Vertex` struct (`POSITION`, `NORMAL`, `TEXCOORD`) to link vertex buffers to the vertex shader stage
- Camera Setup:
  - Added static View (`glm::lookAt`) and Projection (`glm::perspective`) matrices in `Engine`
  - Camera matrices are passed to the renderer via `UpdatePerFrameConstants`
- Debugging UI:
  - Integrated Dear ImGui library (with Win32 and DirectX 11 backends)
  - Added G-Buffer Viewer window in `Renderer::EndFrame` using ImGui::Image to display G-Buffer SRVs
- Core System Support:
  - Propagated window dimensions (`width`, `height`) from `PlatformWindow` through `Application`/`Engine` to `Renderer::Init` for G-Buffer creation
  - `Renderer` now exposes `GetDevice()` for external buffer creation (used by `ResourceManager`)
  - Added ECS `TransformComponent` and `MeshComponent` for scene definition
  - Implemented cube primitive mesh generation in `ResourceManager`
  - Added basic `README.md`
  - Added ECS debug logging via `OutputDebugStringA`, viewed through [DebugView](https://learn.microsoft.com/en-us/sysinternals/downloads/debugview)
  - Added `build.bat`, `remake.bat` and `run.bat` for convenience in building via CMake.

### Changed

- Fundamentally reworked `Renderer` to manage the deferred rendering pipeline instead of just clearing/presenting the back buffer
- Updated `Engine::Render` call sequence to `UpdatePerFrameConstants` -> `BeginFrame` -> `GeometryPass` -> `EndFrame`
- Frame Lifecycle:
  - `Renderer::BeginFrame` now binds and clears the G-Buffer targets
  - `Renderer::EndFrame` now binds the back buffer RTV (`nullptr` DSV), renders ImGui UI, and presents the swap chain
- `PlatformWindow::WndProc` now forwards input messages to ImGui via `ImGui_ImplWin32_WndProcHandler`
- Changed `Vertex` struct members from `glm::vec3`/`vec2` to raw `float[3]` / `float[2]` arrays (ensured input layout format still matches)
- Refactored ECS iteration in `Engine::Update` (and `GeometryPass`) to use `entt::view.each()`
- `Engine` initialization order adjusted (`Renderer` before `ResourceManager`)
- Removed test `NameComponent`

### Fixed

- Fixed issue where geometry failed to render to G-Buffer targets. Corrected matrix multiplication order in `GeometryVS.hlsl` from `mul(vector, matrix)` to `mul(matrix, vector)` to properly handle column-major matrices from GLM.
- Fixed crash caused by attempting to use uninitialized GBuffer render target views (`nullptr`) during `ClearRenderTargetView` calls.
- Fixed `TransformComponent` GLM include issue.
- Fixed ECS loop iteration error caused by incorrect use of `entt::view` in range-based for loop.

---

## [0.1.0] – 2025-04-23

### Added

- `Application` class and `PlatformWindow` abstraction (Win32).
- `Engine` class with basic `Init`, `Update`, `Render` structure.
- Initial `Renderer` setup for DX11 device/swapchain, backbuffer RTV, clear/present.
- Basic ECS setup using EnTT library (`ECSRegistry`, initial `NameComponent`).
- `InputManager` stub class.
- Initial ECS entity test with per-frame logging.
- Basic CMake project setup with DX11 linking.
- Initial window resizing support via DXGI scaling (no explicit buffer reallocation).

### Changed

- Adopted RAII using `Microsoft::WRL::ComPtr` for all DirectX COM objects, replacing manual `Release` calls.
- Ensured safe use of `.GetAddressOf()` for `ComPtr` initialization.

### Fixed

- Resolved linker error for `D3D11CreateDeviceAndSwapChain`.
- Corrected incomplete type usage errors with `std::unique_ptr`.
- Fixed incorrect `ComPtr` argument passing to `ClearRenderTargetView` and `OMSetRenderTargets`.
