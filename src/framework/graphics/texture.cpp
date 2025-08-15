#include "texture.hpp"

static uint32_t boundTexture = 0;

static void destroyTexture(uint32_t texture) {
    if (texture == 0) {
        return;
    }
    if (boundTexture == texture) {
        boundTexture = 0;
    }

    glDeleteTextures(1, &texture);
}

Texture::Texture() {}
Texture::Texture(uint32_t id, uint32_t width, uint32_t height) : id(id), width(width), height(height) {}
Texture::~Texture() {
    destroyTexture(this->id);
}

void Texture::bind(uint32_t slot) const {
    if (boundTexture == this->id) {
        return;
    }

    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, this->id);

    boundTexture = this->id;
}

uint32_t Texture::getId() const {
    return this->id;
}

uint32_t Texture::getWidth() const {
    return this->width;
}
uint32_t Texture::getHeight() const {
    return this->height;
}

bool Texture::isValid() const {
    return this->id != 0;
}

Texture::Texture(Texture &&other) noexcept : id(other.id), width(other.width), height(other.height) {
    other.id = 0;
    other.width = 0;
    other.height = 0;
}
Texture &Texture::operator=(Texture &&other) noexcept {
    if (this != &other) {
        destroyTexture(this->id);

        this->id = other.id;
        this->width = other.width;
        this->height = other.height;

        other.id = 0;
        other.width = 0;
        other.height = 0;
    }

    return *this;
}

Texture Texture::fromFile(const char *path, const Texture::Properties &properties) {
    SDL_Surface *surface = IMG_Load(path);
    if (surface == nullptr) {
        throwFatal("IMG_Load", "Failed to load texture from file. Surface is nullptr.");
        return Texture();
    }

    if (!SDL_FlipSurface(surface, SDL_FLIP_VERTICAL)) {
        throwFatal("SDL_FlipSurface", "Failed to flip surface.");
        SDL_DestroySurface(surface);
        return Texture();
    }

    SDL_PixelFormat sdlPixelFormat;
    TextureFormat format = properties.format;
    switch (format) {
        case TextureFormat::RGBA:
            sdlPixelFormat = SDL_PIXELFORMAT_RGBA32;
            break;
        case TextureFormat::RGB:
            sdlPixelFormat = SDL_PIXELFORMAT_RGB24;
            break;
        default:
            sdlPixelFormat = SDL_PIXELFORMAT_RGB24;
            format = TextureFormat::RGB;
            break;
    }

    if (surface->format != sdlPixelFormat) {
        SDL_Surface *converted = SDL_ConvertSurface(surface, sdlPixelFormat);
        SDL_DestroySurface(surface);
        surface = converted;
    }

    if (surface == nullptr) {
        throwFatal("SDL_ConvertSurface", "Failed to convert texture format.");
        return Texture();
    }

    uint32_t texture = 0;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    int bytesPerPixel = 0;
    switch (properties.format) {
        case TextureFormat::R:    bytesPerPixel = 1; break;
        case TextureFormat::RG:   bytesPerPixel = 2; break;
        case TextureFormat::RGB:  bytesPerPixel = 3; break;
        case TextureFormat::RGBA: bytesPerPixel = 4; break;
        default:                  bytesPerPixel = 4; break;
    }
    if ((surface->w * bytesPerPixel) % 4 != 0) {
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    }
    uint32_t width = static_cast<uint32_t>(surface->w), height = static_cast<uint32_t>(surface->h);
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        static_cast<GLint>(properties.internalFormat),
        static_cast<GLsizei>(width),
        static_cast<GLsizei>(height),
        0,
        static_cast<GLenum>(format),
        static_cast<GLenum>(properties.type),
        surface->pixels
    );
    SDL_DestroySurface(surface);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, static_cast<GLint>(properties.filter));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, static_cast<GLint>(properties.filter));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, static_cast<GLint>(properties.wrap));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, static_cast<GLint>(properties.wrap));

    // TODO: Generate mipmaps if filter one of MIPMAP options
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    return Texture(texture, width, height);
}
Texture Texture::fromPixels(const void *data, uint32_t width, uint32_t height, const Texture::Properties &properties) {
    uint32_t texture = 0;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    int bytesPerPixel = 0;
    switch (properties.format) {
        case TextureFormat::R:    bytesPerPixel = 1; break;
        case TextureFormat::RG:   bytesPerPixel = 2; break;
        case TextureFormat::RGB:  bytesPerPixel = 3; break;
        case TextureFormat::RGBA: bytesPerPixel = 4; break;
        default:                  bytesPerPixel = 4; break;
    }
    if ((width * bytesPerPixel) % 4 != 0) {
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    }

    glTexImage2D(GL_TEXTURE_2D, 0, static_cast<GLint>(properties.internalFormat), width, height, 0, static_cast<GLenum>(properties.format), static_cast<GLenum>(properties.type), data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, static_cast<GLint>(properties.filter));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, static_cast<GLint>(properties.filter));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, static_cast<GLint>(properties.wrap));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, static_cast<GLint>(properties.wrap));

    // TODO: Generate mipmaps if filter one of MIPMAP options
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    return Texture(texture, width, height);
}