#pragma once

#include "ICore.hpp"
#include "IDisplayModule.hpp"
#include <memory>

class IGameModule {
public:
    virtual ~IGameModule() = default;

    // This should initialize the state of the game module. It MUST be called before doing anything else with the game module
    virtual void init(ICore *coreHandle) = 0;

    // Should update the state of all the actors in the game such that the game is ready to draw the next frame.
    virtual void update() = 0;

    // This should draw the state of the game onto the screen
    virtual void draw() = 0;
};

// Note: This should return a pointer to some IGameModuleImpl (please do not make that an actual class name), which should then be deleted when we're done with the module
extern "C" std::unique_ptr<IGameModule> gEpitechArcadeGetGameModuleHandle();
