#include "debug.hpp"

void throwFatal(const char *title, const std::string &message) {
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, title, message.c_str(), nullptr);
    SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, (message + "\nSDL error: (" + SDL_GetError() + ')').c_str());
}

static Uint64 measureTimer = 0;
static double maxMeasureTime = 0.0, lastMeasureTime = 0.0, avgMeasureTime = 0.0;
static size_t measureFrames = 1;

void beginTimeMeasure() {
    measureTimer = SDL_GetTicksNS();
}
void endTimeMeasure() {
    double measureTime = static_cast<double>(SDL_GetTicksNS() - measureTimer) / 1000000.0;
    if (measureTime > maxMeasureTime) {
        maxMeasureTime = measureTime;
    }

    avgMeasureTime += (measureTime - lastMeasureTime) / static_cast<double>(measureFrames);
    lastMeasureTime = measureTime;
    measureFrames++;

    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, ("Measure: " + std::to_string(measureTime) + "ms | Avg: " + std::to_string(avgMeasureTime) + "ms | Peak: " + std::to_string(maxMeasureTime) + "ms").c_str());
}
void resetTimeMeasure() {
    maxMeasureTime = 0.0;
    avgMeasureTime = 0.0;
    measureFrames = 1;
}