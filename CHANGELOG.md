# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),  
and this project adheres to [Semantic Versioning](https://semver.org/).

## [Unreleased]

### Added

- ~~Placeholder~~

## [0.1.0] – 23/04/25

### Added

- `Application` and `PlatformWindow` (Win32)
- `Engine` with `Init`, `Update`, `Render`
- `Renderer` with DX11 init, backbuffer clear/present
- ECS via EnTT, with `ECSRegistry` and `NameComponent`
- `InputManager` stub
- ECS entity test with per-frame log
- CMake setup with DX11 linking and output path
- Basic window resizing support via DXGI scaling (no buffer reallocation yet)

### Changed

- RAII cleanup using `ComPtr` for all COM objects
- Replaced raw pointers and manual `Release` calls
- DX11 init uses `.GetAddressOf()` safely

### Fixed

- Linker error for `D3D11CreateDeviceAndSwapChain`
- Incomplete type usage in `unique_ptr` declarations
- `ComPtr` misuse in `ClearRenderTargetView` and `OMSetRenderTargets`
