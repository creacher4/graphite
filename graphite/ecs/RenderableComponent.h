#pragma once

// #include <string>
#include <filesystem>

using AssetID = std::filesystem::path;

struct RenderableComponent
{
    // std::string primitiveID;

    // AssetID meshID;
    AssetID modelID;
    size_t subMeshIndex = 0; // submesh index in the model
    AssetID materialID;
};
