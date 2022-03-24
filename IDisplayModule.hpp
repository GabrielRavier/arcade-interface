#pragma once

#include <string>
#include <memory>
#include <cstdint>

// Things that MUST be passed on when a display module is loaded
// - Every single texture from the previous display module must be reloaded
// - pixelsPerCell must be set to the value from the old display module
class IDisplayModule {
public:
    virtual ~IDisplayModule() = default;

    // Usually used to represent positions
    struct Vector2u {
        std::uint32_t x;
        std::uint32_t y;
    };

    // All the colors a cell can be. Can't really do more than this considering this is the minimum set that ncurses implements, and adding anymore would make this a ridiculous mess
    // Note that "none" means no color at all (i.e. the character or background of a sprite are not drawn, for example)
    enum class Color {
        black,
        red,
        green,
        yellow,
        blue,
        magenta,
        cyan,
        white,
        none,
    };

    // This MUST not be 0. It is the width and height of a call in the game
    // Yes, I know this is stupid and this could just be a variable, but we apparently have to be "pure" and that means no variables at all in interfaces apparently
    virtual void setPixelsPerCell(std::uint32_t pixelsPerCell) = 0;
    virtual std::uint32_t getPixelsPerCell() = 0;

    // RawTexture is a class containing a texture.
    // If loading a .png, a texture contains a width and a height, which SHOULD correspond to the width and height of the image in the .png file. Note that with a backend that supports using images in sprite (usually graphics), usually only the filename parameters will be used, whereas on a backend that does not (usually text) usually only the rest of the parameters will be used. Note that pointers to RawTexture become invalid after destroying a graphics library and MUST be destroyed before doing so
    // If loading a .ttf, the resulting texture will be the given character, with the given color and the given width used as the font size.
    // Note that it is genereally preferrable that one use a fixed-size font
    class RawTexture {
    public:
        virtual ~RawTexture() = 0;
    };

    // This MUST only ever be called by ICore, which MUST properly handle the case where graphics libraries are reloaded during a game session (and yes the subject requires this)
    // The filename MUST either end in .png or .ttf, and point to files of the correct format.
    // If the filename ends in .png, the texture will be loaded from there
    // If the filename ends in .ttf, the texture will be a letter loaded from that TTF file
    virtual std::unique_ptr<IDisplayModule::RawTexture> loadTexture(const std::string &filename, char character, IDisplayModule::Color characterColor, IDisplayModule::Color backgroundColor, std::size_t width, std::size_t height) = 0;

    // This opens the window with the wanted window size. The size is in pixels. It MUST be called before trying to render or display anything. 
    virtual void openWindow(IDisplayModule::Vector2u pixelsWantedWindowSize) = 0;

    // These are probably all the buttons you need. I would personally recommand that they map onto WASD (for left right up down), the arrow keys (for abxy), q and e (for l and r) and c and v (for start and select) to have a layout as close to an actual controller, but if you want you can do something else (...probably your core should tell people what the inputs are in the menu...)
    // The F1 to F7 buttons MUST NOT be used by the game as they WILL be used by the core to handle the following inputs (preferrably in the same order), which are MANDATED by the subject:
    // - Previous graphics library
    // - Next graphics library
    // - Previous game
    // - Next game
    // - Restart the game
    // - Go back to the menu
    // - Exit
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
        Select,
        F1,
        F2,
        F3,
        F4,
        F5,
        F6,
        F7,
    };

    // isButtonPressed returns whether the button has started getting held on this frame (i.e. if the button was also held on the previous frame it will return false)
    // If you're wondering why there's no interface to get the currently held buttons (i.e. without regards to whether they just started getting held), just try to implement that with ncurses :p. Anyway, if you go look at how the games we're supposed to do, you'll see they're easily implemented without this
    virtual bool isButtonPressed(IDisplayModule::Button button) = 0;

    struct MouseButtonReleaseEvent {
        enum class Type {
            None,
            Left,
            Right,
        } type;
        IDisplayModule::Vector2u cellPosition;
    };
    // If someone released a mouse button on this frame, this will return a MouseButtonReleaseEvent with information on which button was released and where
    virtual IDisplayModule::MouseButtonReleaseEvent getMouseButtonReleaseEvent() = 0;

    // Did the user just try to close the game in a way that only the display can see ? (e.g. sf::Event::Closed, SDL_QUIT, etc.)
    // Note: the core should handle this the same way as pressing F7, i.e. immediately exit
    virtual bool isClosing() = 0;

    // You MUST call this before calling getTextInput, and should preferably do nothing else related to input than calling getTextInput until you call endTextInput
    virtual void startTextInput() = 0;

    // This MUST never be called while not in between a call to startTextInput and a call to endTextInput
    // This returns text read from the user, as a string.
    // Usage of backspace WILL be returned as a \b character.
    // Usage of enter WILL be returned as a \n character.
    virtual std::string getTextInput() = 0;

    // This should never be called before a call to startTextInput and no calls to getTextInput should occur after this before another call to startTextInput
    virtual void endTextInput() = 0;

    struct Sprite {
        IDisplayModule::Vector2u rawPixelPosition;
        IDisplayModule::RawTexture *texture;
    };

    // This replaces the entire current contents of the screen with the chosen background color. This could also be done by just displaying a big texture to do this, but this is simpler and avoids having to carry around a png file just for this.
    virtual void clearScreen(IDisplayModule::Color color) = 0;

    // This renders a sprite, although it WILL NOT be displayed onto the screen until display() is called.
    // Note that in text mode, a sprite will ALWAYS take at least one cell, whichever cell is at the center of the sprite. In text mode, should a sprite overlap more than one cell, its color WILL NOT be displayed onto any other cell. PLEASE DO NOT make graphics that take much more than a cell to display...
    // Note that if multiple sprites are rendered onto one another, the order in which they are layered corresponds to the order in which they were rendered (i.e. when two sprites overlap, whichever was rendered last is rendered on top of the other)
    virtual void renderSprite(IDisplayModule::Sprite sprite) = 0;

    // This MUST be called to do the actual displaying (i.e. actually puts the pixels that have been drawn onto the screen). It does no sleeping, as the core is responsible for that.
    virtual void display() = 0;

    // This MUST be called by the core after the end of each frame (i.e. AFTER SLEEPING so as to avoid input lag) so as to allow the display module to poll events and things like that
    virtual void update() = 0;
};

// This is here so that you don't get an "undefined symbol: _ZN14IDisplayModule10RawTextureD2Ev" error when importing your shared libraries. This is actually allowed, you can in fact have pure virtual interfaces that are still implemented. The purpose of leaving it as a pure virtual interface is so that IDisplayModule::RawTexture cannot be instantiated by itself and has to be implemented.
inline IDisplayModule::RawTexture::~RawTexture()
{
}

// Note: This should return a pointer to some IDisplayModuleImpl (please do not make that an actual class name), which should then be deleted when we're done with the module
extern "C" std::unique_ptr<IDisplayModule> gEpitechArcadeGetDisplayModuleHandle();
