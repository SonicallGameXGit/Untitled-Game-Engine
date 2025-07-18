#include "window.hpp"
#include "../util/debug.hpp"

Window::Window(const char *title, int width, int height, bool resizable) : width(width), height(height) {
    if (!SDL_Init(SDL_INIT_EVENTS | SDL_INIT_VIDEO)) {
        throwFatal("SDL_Init", "Failed to initialize SDL.");
        return;
    }

    this->handle = SDL_CreateWindow(title, this->width, this->height, SDL_WINDOW_OPENGL | (resizable ? SDL_WINDOW_RESIZABLE : 0));
    if (this->handle == nullptr) {
        throwFatal("SDL_CreateWindow", "Failed to create window.");
        return;
    }

    this->context = SDL_GL_CreateContext(this->handle);
    if (this->context == nullptr) {
        throwFatal("SDL_GL_CreateContext", "Failed to create GL context.");
        return;
    }
    if (glewInit() != GLEW_OK) {
        throwFatal("glewInit", "Failed to init GLEW.");
        return;
    }

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
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_EVENT_QUIT: {
                this->running = false;
                break;
            }
            case SDL_EVENT_WINDOW_RESIZED: {
                this->width = event.window.data1;
                this->height = event.window.data2;
                glViewport(0, 0, this->width, this->height);

                break;
            }
            default: break;
        }
    }
}

int Window::getWidth() const {
    return this->width;
}
int Window::getHeight() const {
    return this->height;
}

bool Window::isRunning() const {
    return this->running;
}