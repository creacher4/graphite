#pragma once

#include <array>
#include <filesystem>

namespace Config
{

    namespace Window
    {
        static constexpr int WIDTH = 1280;
        static constexpr int HEIGHT = 720;
        static constexpr const wchar_t *TITLE = L"Graphite";
    } // namespace Window

    namespace ShaderPaths
    {
        static constexpr const wchar_t *GEOMETRY_VS = L"shaders/GeometryVS.hlsl";
        static constexpr const wchar_t *GEOMETRY_PS = L"shaders/GeometryPS.hlsl";
        static constexpr const wchar_t *LIGHTING_VS = L"shaders/LightingVS.hlsl";
        static constexpr const wchar_t *LIGHTING_PS = L"shaders/LightingPS.hlsl";
    } // namespace ShaderPaths

    namespace SceneAssets
    {
        // marble bust
        // static const std::filesystem::path MODEL_PATH = "assets/models/marble_bust/marble_bust_01_8k.gltf";
        // static const std::filesystem::path MATERIAL_ID = "assets/models/marble_bust/marble_bust_01_8k";
        // static const std::filesystem::path ALBEDO_PATH = "assets/models/marble_bust/marble_bust_albedo.png";
        // static const std::filesystem::path NORMAL_PATH = "assets/models/marble_bust/marble_bust_normals.png";
        // static const std::filesystem::path ORM_PATH = "assets/models/marble_bust/marble_bust_orm.png";

        // boulder 1
        static const std::filesystem::path MODEL_PATH = "assets/models/boulder/boulder_01_8k.gltf";
        static const std::filesystem::path MATERIAL_ID = "assets/models/boulder/boulder_01_8k";
        static const std::filesystem::path ALBEDO_PATH = "assets/models/boulder/boulder_01_albedo.png";
        static const std::filesystem::path NORMAL_PATH = "assets/models/boulder/boulder_01_normals.png";
        static const std::filesystem::path ORM_PATH = "assets/models/boulder/boulder_01_orm.png";

        // coastland rocks
        // static const std::filesystem::path MODEL_PATH = "assets/models/coastland_rocks/coast_land_rocks_03_8k.gltf";
        // static const std::filesystem::path MATERIAL_ID = "assets/models/coastland_rocks/coast_land_rocks_03_8k";
        // static const std::filesystem::path ALBEDO_PATH = "assets/models/coastland_rocks/coastland_rocks_albedo.png";
        // static const std::filesystem::path NORMAL_PATH = "assets/models/coastland_rocks/coastland_rocks_normals.png";
        // static const std::filesystem::path ORM_PATH = "assets/models/coastland_rocks/coastland_rocks_orm.png";

    } // namespace SceneAssets

    namespace ClearColors
    {
        inline constexpr std::array<float, 4> GBUFFER_ALBEDO = {0.2f, 0.2f, 0.2f, 1.0f};
        inline constexpr std::array<float, 4> GBUFFER_NORMAL = {0.5f, 0.5f, 1.0f, 1.0f};
        inline constexpr std::array<float, 4> GBUFFER_ORM = {0.8f, 0.8f, 0.0f, 1.0f};
        inline constexpr std::array<float, 4> BACKBUFFER_CLEAR = {0.2f, 0.2f, 0.2f, 1.0f};
    } // namespace ClearColors

} // namespace Config