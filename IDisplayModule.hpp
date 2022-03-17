#pragma once

#include <string>
#include <memory>
#include <cstdint>

// Things that MUST be passed on when a display module is loaded
// - Every single texture from the previous display module must be reloaded
// - pixelsPerCell must be set to the value from the old display module
class IDisplayModule {
public:
    // Usually used to represent positions
    struct Vector2u {
        std::uint32_t x;
        std::uint32_t y;
    };

    // All the background colors a cell can be. Can't really do more than this considering this is the minimum set that ncurses implements, and adding anymore would make this a ridiculous mess
    enum class Color {
        black,
        red,
        green,
        yellow,
        blue,
        magenta,
        cyan,
        white,
    };

    // This MUST not be 0. It is the width and height of a call in the game
    // Yes, I know this is stupid and this could just be a variable, but we apparently have to be "pure" and that means no variables at all in interfaces apparently
    void setPixelsPerCell(std::uint32_t pixelsPerCell);
    std::uint32_t getPixelsPerCell();

    // RawTexture is a class containing a texture. A texture contains a width and a height, which SHOULD correspond to the width and height of the image in the .png file. Note that with a backend that supports using images in sprite (usually graphics), usually only the pngFilename parameters will be used, whereas on a backend that does not (usually text) usually only the rest of the parameters will be used. Note that pointers to RawTexture become invalid after destroying a graphics library and MUST be destroyed before doing so
    class RawTexture {
    public:
        virtual ~RawTexture() = 0;
    };

    // This MUST only ever be called by ICore, which MUST properly handle the case where graphics libraries are reloaded during a game session (and yes the subject requires this)
    virtual std::unique_ptr<RawTexture> loadTexture(const std::string &pngFilename, char character, IDisplayModule::Color characterColor, IDisplayModule::Color backgroundColor, std::size_t width, std::size_t height) = 0;

    // This opens the window with the wanted window size. The size is in pixels. It MUST be called before trying to render or display anything. 
    virtual void openWindow(Vector2u pixelsWantedWindowSize) = 0;

    // These are probably all the buttons you need. I would personally recommand that they map onto WASD (for left right up down), the arrow keys (for abxy), q and e (for l and r) and c and v (for start and select) to have a layout as close to an actual controller, but if you want you can do something else 
    enum class Button {
        Left,
        Right,
        Up,
        Down,
        A,
        B,
        X,
        Y,
        L,
        R,
        Start,
        Select
    };

    // isButtonPressed returns whether the button has started getting held on this frame (i.e. if the button was also held on the previous frame it will return false)
    // If you're wondering why there's no interface to get the currently held buttons (i.e. without regards to whether they just started getting held), just try to implement that with ncurses :p. Anyway, if you go look at how the games we're supposed to do, you'll see they're easily implemented without this
    virtual bool isButtonPressed(IDisplayModule::Button button) = 0;

    struct MouseButtonReleaseEvent {
        enum class Type {
            None,
            Left,
            Right,
        };
        Vector2u cellPosition;
    };
    // If someone released a mouse button on this frame, this will return a MouseButtonReleaseEvent with information on which button was released and where
    virtual MouseButtonReleaseEvent getMouseButtonReleaseEvent() = 0;

    // You MUST call this before calling getTextInput, and should preferably do nothing else related to input than calling getTextInput until you call endTextInput
    virtual void startTextInput() = 0;

    // This MUST never be called while not in between a call to startTextInput and a call to endTextInput
    virtual std::string getTextInput() = 0;

    // This should never be called before a call to startTextInput and no calls to getTextInput should occur after this before another call to startTextInput
    virtual void endTextInput() = 0;

    struct Sprite {
        Vector2u rawPixelPosition;
        IDisplayModule::RawTexture *texture;
    };

    // This renders a sprite, although it WILL NOT be displayed onto the screen until display() is called.
    // Note that in text mode, a sprite will ALWAYS take at least one cell, whichever cell overlaps the most with the sprite. In text mode, should a sprite overlap more than one cell, its color WILL be displayed onto every cell for which it overlaps onto more than 50% of its area (although its character WILL NOT be displayed onto more than one cell).
    // Note that if multiple sprites are rendered onto one another, the order in which they are layered corresponds to the order in which they were rendered (i.e. when two sprites overlap, whichever was rendered last is rendered on top of the other)
    virtual void renderSprite(IDisplayModule::Sprite sprite) = 0;

    // This does the actual displaying (i.e. actually puts the pixels that have been drawn onto the screen). It does no sleeping, as the core is responsible for that.
    virtual void display() = 0;
};

// Note: This should return a pointer to some IDisplayModuleImpl (please do not make that an actual class name), which should then be deleted when we're done with the module
extern "C" std::unique_ptr<IDisplayModule> gEpitechArcadeGetDisplayModuleHandle();
