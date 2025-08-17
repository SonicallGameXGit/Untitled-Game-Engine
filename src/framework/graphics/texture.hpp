#pragma once
#include <SDL3_image/SDL_image.h>
#include <glad/glad.h>
#include <framework/util/debug.hpp>

enum class TextureFilter : uint32_t {
    Nearest = GL_NEAREST,
    Linear  = GL_LINEAR,
};
enum class TextureWrap : uint32_t {
    Repeat          = GL_REPEAT,
    MirroredRepeat  = GL_MIRRORED_REPEAT,
    ClampToEdge     = GL_CLAMP_TO_EDGE,
    ClampToBorder   = GL_CLAMP_TO_BORDER,
};
enum class TextureInternalFormat : uint32_t {
    R8      = GL_R8,
    RG8     = GL_RG8,
    RGB8    = GL_RGB8,
    RGBA8   = GL_RGBA8,
    R32F    = GL_R32F,
    RG32F   = GL_RG32F,
    RGB32F  = GL_RGB32F,
    RGBA32F = GL_RGBA32F,
};
enum class TextureFormat : uint32_t {
    R    = GL_RED,
    RG   = GL_RG,
    RGB  = GL_RGB,
    RGBA = GL_RGBA,
};
enum class TextureType : uint32_t {
    UnsignedByte = GL_UNSIGNED_BYTE,
    Float       = GL_FLOAT
};

class Texture {
private:
    uint32_t id = 0, width = 0, height = 0;

    Texture(uint32_t id, uint32_t width, uint32_t height);
    Texture(const Texture&) = delete;
    Texture &operator=(const Texture&) = delete;
public:
    Texture(Texture &&other) noexcept;
    Texture &operator=(Texture &&other) noexcept;

    struct Properties {
        TextureInternalFormat internalFormat = TextureInternalFormat::RGBA8;
        TextureFormat format = TextureFormat::RGBA;
        TextureType type = TextureType::UnsignedByte;
        TextureFilter filter = TextureFilter::Linear;
        TextureWrap wrap = TextureWrap::Repeat;
    };

    Texture();
    ~Texture();

    void bind(uint32_t slot) const;

    uint32_t getWidth() const;
    uint32_t getHeight() const;

    uint32_t getId() const;
    bool isValid() const;
    
    static Texture fromFile(const char *path, const Texture::Properties &properties);
    static Texture fromPixels(const void *data, uint32_t width, uint32_t height, const Texture::Properties &properties);
};