#include "texture.hpp"

static GLuint boundTexture = 0;

Texture::Texture(GLuint id) : id(id) {}
Texture::~Texture() {
    glDeleteTextures(1, &this->id);
}

Texture Texture::fromFile(const char *path, TextureFilter filter, TextureWrap wrap) {
    SDL_Surface *surface = IMG_Load(path);
    if (surface == nullptr) {
        throwFatal("IMG_Load", "Failed to load texture from file. Surface is nullptr.");
        return Texture(0);
    }
    if (surface->format != SDL_PIXELFORMAT_RGBA32) {
        SDL_Surface *previousSurface = surface;
        surface = SDL_ConvertSurface(surface, SDL_PIXELFORMAT_RGBA32);
        SDL_DestroySurface(previousSurface);
    }

    if (surface == nullptr || !SDL_FlipSurface(surface, SDL_FLIP_VERTICAL)) {
        throwFatal("SDL_ConvertSurface", "Failed to convert texture format and flip it.");
        SDL_DestroySurface(surface);
        return Texture(0);
    }

    GLuint texture = 0;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);
    SDL_DestroySurface(surface);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, static_cast<GLint>(filter));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, static_cast<GLint>(filter));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, static_cast<GLint>(wrap));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, static_cast<GLint>(wrap));

    return Texture(texture);
}
Texture Texture::fromPixelsRGBA(void *data, GLsizei width, GLsizei height, TextureFilter filter, TextureWrap wrap) {
    GLuint texture = 0;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, static_cast<GLint>(filter));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, static_cast<GLint>(filter));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, static_cast<GLint>(wrap));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, static_cast<GLint>(wrap));

    return Texture(texture);
}

void Texture::bind(uint32_t slot) const {
    if (boundTexture == this->id) {
        return;
    }

    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, this->id);

    boundTexture = this->id;
}

GLuint Texture::getId() const {
    return this->id;
}

TextureView::TextureView() {}
TextureView::TextureView(const Texture &texture) {
    this->id = texture.getId();
}
TextureView::~TextureView() {}

void TextureView::bind(uint32_t slot) const {
    if (boundTexture == this->id) {
        return;
    }
    
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, this->id);

    boundTexture = this->id;
}

GLuint TextureView::getId() const {
    return this->id;
}