#pragma once

#include "ICore.hpp"
#include "IDisplayModule.hpp"

class IGameModule {
public:
	// This should initialize the state of the game module. It MUST be called before doing anything else with the game module
	virtual void init(ICore *coreHandle, IDisplayModule *displayHandle) = 0;

	// Should update the state of all the actors in the game such that the game is ready to draw the next frame. Note that it may be possible this is called multiple times before a call to draw(), as lag may result in drawing being redundant (if we're 1 second behind, why bother drawing every frame in the middle ? Just call update 60 times)
	virtual void update() = 0;

	// This should draw the state of the game onto the screen
	virtual void draw() = 0;
};

// Note: This should be a pointer to some IGameModuleImpl (please do not make this an actual class name)
extern "C" IGameModule *gEpitechArcadeGameModuleHandle;
