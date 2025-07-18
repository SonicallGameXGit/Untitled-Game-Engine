#pragma once
#include <SDL3_image/SDL_image.h>
#include <GL/glew.h>
#include "../util/debug.hpp"

enum class TextureFilter : GLint {
    NEAREST = GL_NEAREST,
    LINEAR = GL_LINEAR,
};
enum class TextureWrap : GLint {
    REPEAT = GL_REPEAT,
    MIRRORED_REPEAT = GL_MIRRORED_REPEAT,
    CLAMP_TO_EDGE = GL_CLAMP_TO_EDGE,
    CLAMP_TO_BORDER = GL_CLAMP_TO_BORDER,
};

class Texture {
private:
    GLuint id = 0;
    Texture(GLuint id);
public:
    ~Texture();
    
    static Texture fromFile(const char *path, TextureFilter filter, TextureWrap wrap);
    static Texture fromPixelsRGBA(void *data, GLsizei width, GLsizei height, TextureFilter filter, TextureWrap wrap);

    void bind(uint32_t slot) const;

    GLuint getId() const;
};
class TextureView {
private:
    GLuint id = 0;
public:
    TextureView();
    TextureView(const Texture &source);
    ~TextureView();

    void bind(uint32_t slot) const;
    GLuint getId() const;
};