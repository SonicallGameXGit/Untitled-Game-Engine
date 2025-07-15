#pragma once
#include <SDL3/SDL.h>
#include <string>

static void throwFatal(const char *title, const std::string &message) {
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, title, message.c_str(), nullptr);
    SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, (message + "\nSDL error: (" + SDL_GetError() + ')').c_str());
}