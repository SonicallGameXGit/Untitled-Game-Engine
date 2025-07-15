#pragma once
#include <SDL3/SDL.h>
#include <GL/glew.h>

class Window {
private:
    SDL_Window *handle = nullptr;
    SDL_GLContext context = {};

    bool running = false;
public:
    Window(const char *title, int width, int height, bool resizable);
    ~Window();

    void swapBuffers() const;
    void pollEvents();
    
    bool isRunning() const;
};