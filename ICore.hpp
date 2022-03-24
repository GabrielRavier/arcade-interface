#pragma once

#include "IDisplayModule.hpp"
#include <unordered_map>

class ICore {
public:
    virtual ~ICore() = default;

    // Sets the amount of pixels contained in every cell. pixelsPerCell MUST NOT be 0, and this MUST be called before trying to do anything w.r.t. rendering/displaying
    virtual void setPixelsPerCell(std::uint32_t pixelsPerCell) = 0;

    // Sets the framerate (in frames per second). framerate MUST NOT be 0, and SHOULD be a reasonable amount. Note that framerate handling WILL be done by the core. This MUST be called before trying to do anything w.r.t. displaying
    // To implementers: Use clock_nanosleep
    virtual void setFramerate(unsigned framerate) = 0;

    // See IDisplayModule::Vector2u for docs
    using Vector2u = IDisplayModule::Vector2u;

    // See IDisplayModule::Color for docs
    using Color = IDisplayModule::Color;

    // This exists because one needs to be able to dynamically reload the textures when changing the display module.
    // Essentially, this needs to contain all the information to pass to IDisplayModule::loadTexture, and a pointer to the result of that, so that when changing graphics library one can call IDisplayModule::loadTexture with the arguments and update the raw texture pointer.
    // That raw texture pointer should NEVER be visible to the IGameModule, such as to avoid potential leaks of texture pointers which could lead to invalid usage
    // To implementers: std::deque is probably the best way you can implement the texture store (which you need to be able to reload them), as pointers to elements of an std::vector do not stay stable when increasing the size of the vector
    class Texture;
    virtual ICore::Texture *loadTexture(const std::string &filename, char character, ICore::Color characterColor, ICore::Color backgroundColor, std::size_t width, std::size_t height) = 0;

    // See IDisplayModule::openWindow for docs
    virtual void openWindow(ICore::Vector2u pixelsWantedWindowSize) = 0;

    // See IDisplayModule::Button for docs
    using Button = IDisplayModule::Button;

    // See IDisplayModule::isButtonPressed for docs
    virtual bool isButtonPressed(ICore::Button button) = 0;

    using MouseButtonReleaseEvent = IDisplayModule::MouseButtonReleaseEvent;
    // See IDisplayModule::getMouseButtonReleaseEvent for docs
    virtual ICore::MouseButtonReleaseEvent getMouseButtonReleaseEvent() = 0;

    // See IDisplayModule::startTextInput for docs
    virtual void startTextInput() = 0;

    // See IDisplayModule::getTextInput for docs
    virtual std::string getTextInput() = 0;

    // See IDisplayModule::endTextInput for docs
    virtual void endTextInput() = 0;

    struct Sprite {
        ICore::Vector2u pixelPosition;
        ICore::Texture *texture;
    };

    // See IDisplayModule::clearScreen for docs
    virtual void clearScreen(ICore::Color color) = 0;

    // This is equivalent to IDisplayModule::renderSprite, except that it accepts core sprites instead of DisplayModule sprites
    virtual void renderSprite(ICore::Sprite sprite) = 0;

    // This tells the core about the score of a game session that was just finished (i.e. the player got a game over or something like that)
    // Note: This DOES NOT mean that the core should go back to the menu or do anything apart from adding the given score to the score list
    virtual void addNewScore(std::uint32_t score) = 0;
};
