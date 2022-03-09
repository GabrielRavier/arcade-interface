#pragma once

#include "IDisplayModule.hpp"
#include <unordered_map>

// Note: Typically the texture cache would be implemented using an unordered_map
class ICore {
public:
    // This *loads* a texture. you MUST only ever call this function once for any specific texture, and you MUST give a unique ID for it, which you will be able to get the texture with using getTexture. pngFilename MUST be a path to an .png
    virtual void loadTexture(IDisplayModule::Texture::Id id, const std::string &pngFilename, char character, IDisplayModule::Color color, std::size_t width, std::size_t height) = 0;

    // This gets one of the textures loaded with loadTexture. You MUST call this function with an ID that has been used in a call to loadTexture.
    virtual IDisplayModule::Texture &getTexture(IDisplayModule::Texture::Id id) = 0;

    virtual void setFramerate(unsigned framerate) = 0;
};
