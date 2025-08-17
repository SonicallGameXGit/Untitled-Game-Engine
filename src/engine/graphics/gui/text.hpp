#pragma once
#include <unordered_map>
#include <string>
#include <glm/glm.hpp>
#include <msdf-atlas-gen/msdf-atlas-gen.h>
#include <framework/graphics/buffers.hpp>
#include <framework/graphics/texture.hpp>

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

class Font {
private:
    Texture texture = Texture();
    std::unordered_map<wchar_t, Glyph> glyphs = std::unordered_map<wchar_t, Glyph>();
public:
    static constexpr float FONT_HEIGHT = 8.0f; // I don't fucking care what user thinks, 8.0 is always enough for 4.5/5 quality of the atlas and speed of generation
    static constexpr float PIXEL_RANGE = 2.0f; // Idk what it does, so I'll just keep it at 2.0f. If someone knows why tf I need to have it more or less than just 1.0f, please tell me :)

    Font(const FreeType &freeType, const std::string &filename);
    ~Font();

    float getTextWidth(const std::wstring &text) const;

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