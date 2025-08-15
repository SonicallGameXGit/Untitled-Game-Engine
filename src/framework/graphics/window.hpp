#pragma once
#include <SDL3/SDL.h>

class Window {
private:
    SDL_Window *handle = nullptr;
    SDL_GLContext context = {};

    int width = 0, height = 0;
    bool running = false;
public:
    struct Config {
        const char *title = "Window";
        int width = 800, height = 600;
        bool resizable = true, vsync = false;
    };

    Window(const Config& config);
    ~Window();

    void swapBuffers() const;
    void pollEvents();

    int getWidth() const;
    int getHeight() const;

    float getHorizontalAspect() const;
    float getVerticalAspect() const;
    
    bool isRunning() const;
};