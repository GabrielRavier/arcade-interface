#pragma once

#include "IDisplayModule.hpp"
#include <unordered_map>

// Note: Typically the texture cache would be implemented using an unordered_map
class ICore {
public:
    virtual void setFramerate(unsigned framerate) = 0;
};
