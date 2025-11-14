#pragma once
#include <SDL3/SDL.h>
#include <unordered_map>
#include <glm/glm.hpp>

class Window {
private:
    SDL_Window *handle = nullptr;
    SDL_GLContext context = {};

    glm::ivec2 size = glm::ivec2();
    bool running = false, viewportChanged = false;
    std::unordered_map<SDL_Keycode, uint64_t> keystates = std::unordered_map<SDL_Keycode, uint64_t>();
    std::unordered_map<Uint8, uint64_t> mousestates = std::unordered_map<Uint8, uint64_t>();
    glm::vec2 mousePosition = glm::vec2(), lastMousePosition = glm::vec2(), mouseDelta = glm::vec2();
    glm::vec2 scrollDelta = glm::vec2();

    size_t frameCounter = 0;
public:
    struct Config {
        const char *title = "Window";
        int width = 800, height = 600;
        bool resizable = true, vsync = false;
    };

    explicit Window(const Config& config);
    ~Window();

    void swapBuffers() const;
    void pollEvents();

    int getWidth() const;
    int getHeight() const;
    const glm::ivec2 &getSize() const;

    float getHorizontalAspect() const;
    float getVerticalAspect() const;
    
    bool isRunning() const;
    bool hasViewportChanged() const;

    bool isKeyPressed(SDL_Keycode key) const;
    bool isKeyJustPressed(SDL_Keycode key) const;
    bool isMousePressed(Uint8 button) const;
    bool isMouseJustPressed(Uint8 button) const;

    float getMouseX() const;
    float getMouseY() const;
    const glm::vec2 &getMousePosition() const;

    float getMouseDeltaX() const;
    float getMouseDeltaY() const;
    const glm::vec2 &getMouseDelta() const;

    float getScrollDeltaX() const;
    float getScrollDeltaY() const;
    const glm::vec2 &getScrollDelta() const;
};