#pragma once

#include <string>
#include <memory>
#include <cstdint>

class IDisplayModule {
public:
    // Usually used to represent positions
    struct Vector2u {
        std::uint32_t x;
        std::uint32_t y;
    };

    enum class Color {
        blue,
        yellow,
        pink,
    };

    // This MUST not be 0. It is the width and height of a call in the game, and should be set by the game
    std::uint32_t pixelsPerCell;

    // This function converts a raw position (i.e. a position as measured in pixels) into a cell position (i.e. a position as measured in cells)
    Vector2u rawPixelPositionToCellPosition(Vector2u rawPosition)
    {
        return {rawPosition.x / this->pixelsPerCell, rawPosition.y / this->pixelsPerCell};
    }

    // This function converts a cell position to a raw position
    Vector2u cellPositionToRawPixelPosition(Vector2u cellPosition)
    {
        return {cellPosition.x * this->pixelsPerCell, cellPosition.y * this->pixelsPerCell};
    }

    Vector2u getSquareCenter(Vector2u topLeft, Vector2u size)
    {
        return {topLeft.x + ((topLeft.x - size.x) / 2), topLeft.y + ((topLeft.y - size.y) / 2)};
    }

    // Texture is a class containing a texture. A texture contains a width and a height, which SHOULD correspond to the width and height of the image in the .png file. Note that with a backend that supports using images in sprite (usually graphics), usually only the pngFilename parameters will be used, whereas on a backend that does not (usually text) usually only the rest of the parameters will be used.
    class Texture {
    public:
        using Id = std::uint64_t;
    };
    virtual std::unique_ptr<Texture> loadTexture(IDisplayModule::Texture::Id id, const std::string &pngFilename, char character, IDisplayModule::Color color, std::size_t width, std::size_t height) = 0;

    struct Sprite {
        Vector2u rawPixelPosition;
        Texture *texture;
    };

    // This opens the window with the wanted window size. The size is calculated in cells. It MUST be called before trying to render or display anything
    virtual void openWindow(Vector2u wantedWindowSize) = 0;

    enum class Button {
        Left,
        Right,
        Up,
        Down,
        A,
        B,
        X,
        Y,
        Start,
        Select
    };

    // isButtonPressed returns whether the button has started getting held on this frame (i.e. if the button was also held on the previous frame it will return false)
    virtual bool isButtonPressed(Button button) = 0;

    // isButtonHeld returns whether a button is held in any way (including if it was held on the previous frame)
    virtual bool isButtonHeld(Button button) = 0;

    // You MUST call this before calling getTextInput, and should preferably do nothing else related to input than calling getTextInput until you call endTextInput
    virtual void startTextInput() = 0;

    // This MUST never be called while not in between a call to startTextInput and a call to endTextInput
    virtual std::string getTextInput() = 0;

    // This should never be called before a call to startTextInput and no calls to getTextInput should occur after this before another call to startTextInput
    virtual void endTextInput() = 0;

    // This renders a sprite, although it WILL NOT be displayed onto the screen until display() is called.
    // Note that in text mode, a sprite will ALWAYS take at least one cell, whichever cell overlaps the most with the sprite. In text mode, should a sprite overlap more than one cell, its color WILL be displayed onto every cell for which it overlaps onto more than 50% of its area (although its character WILL NOT be display onto more than one cell).
    virtual void renderSprite(Sprite sprite) = 0;

    // This does the actual displaying (i.e. actually puts the pixels that have been drawn onto the screen). It does no sleeping, as the core is responsible for that. Note: This SHOULD only be called by the core
    virtual void display() = 0;
};

// Note: This should be a pointer to some IDisplayModuleImpl (please do not make this an actual class name)
extern "C" IDisplayModule *gEpitechArcadeDisplayModuleHandle;
