#pragma once
#include <SDL3_image/SDL_image.h>
#include <GL/glew.h>
#include "../util/debug.hpp"

enum class TextureFilter : GLint {
    NEAREST = GL_NEAREST,
    LINEAR  = GL_LINEAR,
};
enum class TextureWrap : GLint {
    REPEAT          = GL_REPEAT,
    MIRRORED_REPEAT = GL_MIRRORED_REPEAT,
    CLAMP_TO_EDGE   = GL_CLAMP_TO_EDGE,
    CLAMP_TO_BORDER = GL_CLAMP_TO_BORDER,
};
enum class TextureInternalFormat : GLint {
    R8      = GL_R8,
    RG8     = GL_RG8,
    RGB8    = GL_RGB8,
    RGBA8   = GL_RGBA8,
    R32F    = GL_R32F,
    RG32F   = GL_RG32F,
    RGB32F  = GL_RGB32F,
    RGBA32F = GL_RGBA32F,
};
enum class TextureFormat : GLenum {
    R    = GL_RED,
    RG   = GL_RG,
    RGB  = GL_RGB,
    RGBA = GL_RGBA,
};
enum class TextureType : GLenum {
    UNSIGNED_BYTE = GL_UNSIGNED_BYTE,
    FLOAT         = GL_FLOAT
};

class Texture {
private:
    GLuint id = 0;
    uint32_t width = 0, height = 0;

    Texture(GLuint id, uint32_t width, uint32_t height);
public:
    Texture();
    ~Texture();

    Texture(const Texture&) = delete;
    Texture &operator=(const Texture&) = delete;

    Texture(Texture &&other) noexcept;
    Texture &operator=(Texture &&other) noexcept;
    
    static Texture fromFile(const char *path, TextureInternalFormat internalFormat, TextureFormat format, TextureType type, TextureFilter filter, TextureWrap wrap);
    static Texture fromPixels(const void *data, GLsizei width, GLsizei height, TextureInternalFormat internalFormat, TextureFormat format, TextureType type, TextureFilter filter, TextureWrap wrap);

    void bind(uint32_t slot) const;

    uint32_t getWidth() const;
    uint32_t getHeight() const;

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