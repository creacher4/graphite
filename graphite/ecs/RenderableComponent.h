#pragma once

// #include <string>
#include <filesystem>

using AssetID = std::filesystem::path;

struct RenderableComponent
{
    // std::string primitiveID;

    AssetID meshID;
    AssetID materialID;
};
