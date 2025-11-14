#include "window.hpp"
#include <glad/glad.h>
#include <framework/util/debug.hpp>

Window::Window(const Window::Config &config) : size(config.width, config.height) {
    if (!SDL_Init(SDL_INIT_EVENTS | SDL_INIT_VIDEO)) {
        Debug::throwFatal("SDL_Init", "Failed to initialize SDL.");
        return;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);
    this->handle = SDL_CreateWindow(config.title, this->size.x, this->size.y, SDL_WINDOW_OPENGL | (config.resizable ? SDL_WINDOW_RESIZABLE : 0));
    if (this->handle == nullptr) {
        Debug::throwFatal("SDL_CreateWindow", "Failed to create window.");
        return;
    }
    
    this->context = SDL_GL_CreateContext(this->handle);
    if (this->context == nullptr) {
        Debug::throwFatal("SDL_GL_CreateContext", "Failed to create GL context.");
        return;
    }

    if (!gladLoadGL()) {
        Debug::throwFatal("gladLoadGL", "Failed to load OpenGL functions.");
        return;
    }
    glEnable(GL_MULTISAMPLE);

    SDL_GL_SetSwapInterval(config.vsync);
    this->running = true;
}
Window::~Window() {
    SDL_GL_DestroyContext(this->context);
    SDL_DestroyWindow(this->handle);
    SDL_Quit();
}

void Window::swapBuffers() const {
    SDL_GL_SwapWindow(this->handle);
}
void Window::pollEvents() {
    this->frameCounter++;
    this->mouseDelta = this->mousePosition - this->lastMousePosition;
    this->lastMousePosition = this->mousePosition;
    this->scrollDelta = glm::vec2();
    this->viewportChanged = false;
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_EVENT_QUIT: {
                this->running = false;
                break;
            }
            case SDL_EVENT_WINDOW_RESIZED: {
                this->size.x = event.window.data1;
                this->size.y = event.window.data2;
                glViewport(0, 0, this->size.x, this->size.y);
                this->viewportChanged = true;

                break;
            }
            case SDL_EVENT_KEY_DOWN: {
                if (!event.key.repeat) {
                    this->keystates[event.key.key] = this->frameCounter;
                }
                break;
            }
            case SDL_EVENT_KEY_UP: {
                this->keystates[event.key.key] = 0;
                break;
            }
            case SDL_EVENT_MOUSE_MOTION: {
                this->mousePosition.x = static_cast<float>(event.motion.x);
                this->mousePosition.y = static_cast<float>(event.motion.y);
                break;
            }
            case SDL_EVENT_MOUSE_BUTTON_DOWN: {
                this->mousestates[event.button.button] = this->frameCounter;
                break;
            }
            case SDL_EVENT_MOUSE_BUTTON_UP: {
                this->mousestates[event.button.button] = 0;
                break;
            }
            case SDL_EVENT_MOUSE_WHEEL: {
                this->scrollDelta.x += static_cast<float>(event.wheel.x);
                this->scrollDelta.y += static_cast<float>(event.wheel.y);
                break;
            }
            default: break;
        }
    }
}

int Window::getWidth() const {
    return this->size.x;
}
int Window::getHeight() const {
    return this->size.y;
}
const glm::ivec2 &Window::getSize() const {
    return this->size;
}

float Window::getHorizontalAspect() const {
    return static_cast<float>(this->size.x) / static_cast<float>(this->size.y);
}
float Window::getVerticalAspect() const {
    return static_cast<float>(this->size.y) / static_cast<float>(this->size.x);
}

bool Window::isRunning() const {
    return this->running;
}
bool Window::hasViewportChanged() const {
    return this->viewportChanged;
}

bool Window::isKeyPressed(SDL_Keycode key) const {
    auto it = this->keystates.find(key);
    if (it != this->keystates.end()) {
        return it->second > 0;
    }
    return false;
}
bool Window::isKeyJustPressed(SDL_Keycode key) const {
    auto it = this->keystates.find(key);
    if (it != this->keystates.end()) {
        return it->second == this->frameCounter;
    }
    return false;
}

bool Window::isMousePressed(Uint8 button) const {
    auto it = this->mousestates.find(button);
    if (it != this->mousestates.end()) {
        return it->second > 0;
    }
    return false;
}
bool Window::isMouseJustPressed(Uint8 button) const {
    auto it = this->mousestates.find(button);
    if (it != this->mousestates.end()) {
        return it->second == this->frameCounter;
    }
    return false;
}

float Window::getMouseX() const {
    return this->mousePosition.x;
}
float Window::getMouseY() const {
    return this->mousePosition.y;
}
const glm::vec2 &Window::getMousePosition() const {
    return this->mousePosition;
}

float Window::getMouseDeltaX() const {
    return this->mouseDelta.x;
}
float Window::getMouseDeltaY() const {
    return this->mouseDelta.y;
}
const glm::vec2 &Window::getMouseDelta() const {
    return this->mouseDelta;
}

float Window::getScrollDeltaX() const {
    return this->scrollDelta.x;
}
float Window::getScrollDeltaY() const {
    return this->scrollDelta.y;
}
const glm::vec2 &Window::getScrollDelta() const {
    return this->scrollDelta;
}