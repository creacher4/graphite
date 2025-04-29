# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/)
and this project adheres to [Semantic Versioning](https://semver.org/).

## [Unreleased]

### Added

- Modular engine setup helpers: `InitCamera`, `InitSystems` and `InitScene`
- Refactored `AssetManager` to separate mesh processing and texture loading into focused internal helpers for better code clarity and reuse

### Changed

- `Engine::Init` now delegates responsibilities to dedicated helpers for initialization tasks
- `Application::Run()` now returns `int` exit code instead of `void`
- Replaced internal assert checks with explicit runtime exceptions for more controlled error handling

## [0.5.0.2] – 2025-04-28

### Added

- Toggleable no-cull wireframe mode via the Engine Stats window.
- `SetupCamera` helper for centralized camera creation and projection setup.

### Changed

- Camera yaw display normalized to wrap between 0°–360°
- Refactored `Renderer::Init` and `GeometryPass` into smaller helper methods for clarity (no functional changes)

## [0.5.0] – 2025-04-27

### Added

- Assimp-based model loading for `.gltf`, `.fbx` and `.blend` files
- Texture loading pipeline using `stb_image`
- Material system with `Material` structs and `AssetManager` integration
- `RenderableComponent` for ECS-based entity rendering
- Normal mapping support in the deferred rendering pipeline
- Depth view option in the G-Buffer viewer

### Changed

- Renamed `MeshComponent` to `RenderableComponent`
- Default rasterizer state updated to enable back-face culling

### Fixed

- Normal maps now render correctly in the G-Buffer
- Fixed incorrect winding order in imported meshes

## [0.4.2] – 2025-04-27

### Added

- Engine Stats window showing frame metrics (FPS, draw calls, triangle count, camera position, etc.)
- `StatsSystem` to gather and display performance statistics

### Changed

- Updated `Renderer` and `RenderSystem` to integrate with `StatsSystem`.

### Fixed

- Stats window now updates properly each frame

## [0.4.1] – 2025-04-26

### Added

- Camera accessors for FOV and clip plane distances

### Changed

- Cached camera basis vectors for improved performance
- Enforced read-only access to mesh resources through `AssetManager`
- Camera projection now updates automatically on window resize

### Removed

- Deprecated `InputSystem`; input handling is now fully managed by `InputManager`

## [0.4.0] – 2025-04-26

### Added

- `InputManager` for capturing raw Win32 input
- `Camera` class and `CameraController` system for 3D movement and view control
- Delta-time propagation across systems
- Dynamic camera injection into the `RenderSystem`

### Changed

- Eliminated unintended camera roll behavior
- Improved WASD navigation relative to camera orientation

### Fixed

- Clamped camera pitch to prevent flipping
- Blocked camera movement when interacting with UI elements

## [0.3.0] – 2025-04-25

### Added

- Modular `ISystem` and `SystemManager` architecture for systems lifecycle management
- `DeviceManager` for central device/context/swapchain handling
- `AssetManager` for resource loading
- Dependency injection for systems and managers
- Full resize pipeline handling via `ResizeSystem`

### Changed

- Refactored `Engine`, `Renderer`, and `Application` to use new managers and systems

### Removed

- Old `ResourceManager` (merged into `AssetManager`)

### Fixed

- Proper clearing of the back buffer to eliminate ghosting artifacts

## [0.2.0] – 2025-04-25

### Added

- Deferred rendering pipeline with G-Buffer (Albedo, Normal, ORM, Depth)
- Basic HLSL shaders for geometry pass
- Geometry rendering through ECS-based iteration
- GPU constant buffer management for per-frame and per-object data
- Pipeline state objects (Rasterizer, Depth-Stencil states)
- Static camera setup
- Dear ImGui integration for G-Buffer visualization
- ECS components for transforms and meshes
- Primitive mesh generation for testing
- Basic build scripts (`build.bat`, `remake.bat`, `run.bat`)

### Changed

- Overhauled `Renderer` to fully manage the deferred pipeline.
- Restructured engine frame lifecycle around new rendering stages.

### Fixed

- Corrected matrix multiplication order in shaders.
- Prevented crashes from uninitialized GBuffer targets.

## [0.1.0] – 2025-04-23

### Added

- Basic `Application`, `Engine`, `Renderer` and Win32 windowing
- Initial ECS setup using `EnTT`
- Basic Direct3D 11 device/swapchain/backbuffer management
- `InputManager` stub
- Early CMake project setup

### Changed

- Adopted RAII principles with `ComPtr` for DirectX objects

### Fixed

- Resolved linker errors for D3D11 device creation
- Fixed ECS transform component and input forwarding issues
