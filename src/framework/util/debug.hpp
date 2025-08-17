#pragma once
#include <SDL3/SDL.h>
#include <string>

namespace Debug {
    void throwFatal(const char *title, const std::string &message);
    
    void beginTimeMeasure();
    void endTimeMeasure();
    void resetTimeMeasure();
};