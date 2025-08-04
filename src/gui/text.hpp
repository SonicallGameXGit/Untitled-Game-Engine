#pragma once
#include <GL/glew.h>
#include <msdf-atlas-gen/msdf-atlas-gen.h>
#include <glm/glm.hpp>
#include <unordered_map>
#include <array>
#include <vector>
#include <string>
#include <optional>
#include <filesystem>

#include "../framework/graphics/mesh.hpp"
#include "../framework/graphics/texture.hpp"

struct Glyph {
    glm::vec4 boundsLBRT;
    glm::vec4 uvLBRT;

    float advance;
};

class FreeType {
private:
    msdfgen::FreetypeHandle *handle = nullptr;
public:
    FreeType();
    ~FreeType();
    
    msdfgen::FreetypeHandle *getHandle() const;
};

struct TextVertex {
    float x, y, u, v;
};
class Font {
private:
    Texture texture = Texture();
    std::unordered_map<wchar_t, Glyph> glyphs = std::unordered_map<wchar_t, Glyph>();

    float fontHeight = 0.0f;
public:
    static constexpr float PIXEL_RANGE = 1.0f;

    Font(const FreeType &freeType, const std::string &filename, float fontHeight);
    ~Font();

    std::vector<TextVertex> createText(const std::wstring &text) const;

    const Glyph *getGlyph(wchar_t character) const;
    const Texture &getTexture() const;
};

class TextMesh {
private:    
    VertexArray vertexArray = VertexArray();
    VertexBuffer vertexBuffer = VertexBuffer(VertexBufferUsage::Static);
    VertexBuffer instanceBuffer = VertexBuffer(VertexBufferUsage::Static);

    uint32_t numInstances = 0;
public:
    TextMesh();
    ~TextMesh();

    void setText(const std::wstring &text, const Font &font);
    void draw() const;
};