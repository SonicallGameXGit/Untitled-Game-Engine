#pragma once
#include <SDL3/SDL.h>
#include <GL/glew.h>

class Window {
private:
    SDL_Window *handle = nullptr;
    SDL_GLContext context = {};

    int width = 0, height = 0;
    bool running = false;
public:
    Window(const char *title, int width, int height, bool resizable, bool vsync);
    ~Window();

    void swapBuffers() const;
    void pollEvents();

    int getWidth() const;
    int getHeight() const;
    
    bool isRunning() const;
};