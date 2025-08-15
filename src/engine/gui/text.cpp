#include "text.hpp"

#include <string>
#include <SDL3_image/SDL_image.h>
#include "../../framework/util/debug.hpp"

FreeType::FreeType() {
    this->handle = msdfgen::initializeFreetype();
    if (this->handle == nullptr) {
        throwFatal("msdf_atlas::initializeFreetype", "Failed to initialize MSDFGen.");
        return;
    }
}
FreeType::~FreeType() {
    msdfgen::deinitializeFreetype(this->handle);
}

msdfgen::FreetypeHandle *FreeType::getHandle() const {
    return this->handle;
}

Font::Font(const FreeType &freeType, const std::string &filename) {
    msdfgen::FontHandle *font = msdfgen::loadFont(freeType.getHandle(), filename.c_str());
    if (font == nullptr) {
        throwFatal("msdfgen::loadFont", std::string("Failed to load font: ") + filename);
        return;
    }
    std::vector<msdf_atlas::GlyphGeometry> glyphs = std::vector<msdf_atlas::GlyphGeometry>();
    msdf_atlas::FontGeometry geometry = msdf_atlas::FontGeometry(&glyphs);

    // TODO: Load only ASCII charset at the start and if some text requires more characters, load them later using MSDF-Atlas-Gen's dynamic atlas system
    msdf_atlas::Charset charset = msdf_atlas::Charset();
    for (uint32_t cp = 0x0020; cp <= 0xFFFF; ++cp) {
        if (cp >= 0xD800 && cp <= 0xDFFF) continue;
        charset.add(cp);
    }

    int glyphsLoaded = geometry.loadCharset(font, Font::FONT_HEIGHT, charset);
    if (glyphsLoaded <= 0) {
        msdfgen::destroyFont(font);
        throwFatal("msdf_atlas::FontGeometry::loadCharset", "Not enough glyphs loaded! " + std::to_string(glyphsLoaded) + " of " + std::to_string(charset.size()) + ". Font: " + filename);
        return;
    }

    const double maxCornerAngle = 3.0;
    for (msdf_atlas::GlyphGeometry &glyph : glyphs) {
        glyph.edgeColoring(&msdfgen::edgeColoringInkTrap, maxCornerAngle, 0);
    }
    
    msdf_atlas::TightAtlasPacker packer = msdf_atlas::TightAtlasPacker();
    packer.setDimensionsConstraint(msdf_atlas::DimensionsConstraint::SQUARE);
    packer.setPixelRange(static_cast<double>(Font::PIXEL_RANGE));
    packer.setMiterLimit(1.0);
    packer.setScale(Font::FONT_HEIGHT);
    packer.setSpacing(2);

    const int packerSuccess = 0;
    if (packer.pack(glyphs.data(), glyphsLoaded) != packerSuccess) {
        msdfgen::destroyFont(font);
        throwFatal("msdf_atlas::TightAtlasPacker::pack", std::string("Failed to pack all glyphs! Font: ") + filename);
        return;
    }

    int width = 0, height = 0;
    packer.getDimensions(width, height);
    packer.setUnitRange(msdfgen::Range(0.0, 1.0));

    auto generator = msdf_atlas::ImmediateAtlasGenerator<float, 3, msdf_atlas::msdfGenerator, msdf_atlas::BitmapAtlasStorage<msdfgen::byte, 3>>(width, height);

    msdf_atlas::GeneratorAttributes attributes = msdf_atlas::GeneratorAttributes();
    attributes.config.overlapSupport = true;
    attributes.scanlinePass = true;

    generator.setAttributes(attributes);
    generator.setThreadCount(4);
    generator.generate(glyphs.data(), glyphs.size());

    msdfgen::BitmapConstRef<msdfgen::byte, 3> bitmap = generator.atlasStorage();
    this->texture = Texture::fromPixels(bitmap.pixels, bitmap.width, bitmap.height, {
        .internalFormat = TextureInternalFormat::RGB8,
        .format = TextureFormat::RGB,
        .type = TextureType::UnsignedByte,
        .filter = TextureFilter::Linear,
        .wrap = TextureWrap::Repeat
    });

    uint32_t bitmapWidth = this->texture.getWidth(), bitmapHeight = this->texture.getHeight();

    for (const msdf_atlas::GlyphGeometry &glyph : glyphs) {
        msdfgen::unicode_t unicode = glyph.getCodepoint();

        double boundsL = 0.0, boundsB = 0.0, boundsR = 0.0, boundsT = 0.0;
        glyph.getQuadPlaneBounds(boundsL, boundsB, boundsR, boundsT);

        double uvL = 0, uvB = 0, uvR = 0, uvT = 0;
        glyph.getQuadAtlasBounds(uvL, uvB, uvR, uvT);

        this->glyphs[unicode] = Glyph {
            .boundsLBRT = glm::vec4(
                static_cast<float>(boundsL / Font::FONT_HEIGHT),
                static_cast<float>(boundsB / Font::FONT_HEIGHT),
                static_cast<float>(boundsR / Font::FONT_HEIGHT),
                static_cast<float>(boundsT / Font::FONT_HEIGHT)
            ),
            .uvLBRT = glm::vec4(
                static_cast<float>(uvL) / static_cast<float>(bitmapWidth),
                static_cast<float>(uvB) / static_cast<float>(bitmapHeight),
                static_cast<float>(uvR) / static_cast<float>(bitmapWidth),
                static_cast<float>(uvT) / static_cast<float>(bitmapHeight)
            ),
            .advance = static_cast<float>(glyph.getAdvance() / Font::FONT_HEIGHT),
        };
    }

    msdfgen::destroyFont(font);
}
Font::~Font() {}

float Font::getTextWidth(const std::wstring &text) const {
    float maxWidth = 0.0f, width = 0.0f;

    for (wchar_t character : text) {
        if (character == L'\n') {
            maxWidth = std::max(maxWidth, width);
            width = 0.0f;
            continue;
        }

        const Glyph *glyph = this->getGlyph(character);
        if (glyph == nullptr) continue;

        width += glyph->advance;
    }

    maxWidth = std::max(maxWidth, width);
    return maxWidth;
}

const Glyph *Font::getGlyph(wchar_t character) const {
    auto iterator = this->glyphs.find(character);
    if (iterator != this->glyphs.end()) {
        return &iterator->second;
    }

    return nullptr;
}
const Texture &Font::getTexture() const {
    return this->texture;
}

std::vector<TextVertex> Font::createText(const std::wstring &text) const {
    std::vector<TextVertex> vertices = std::vector<TextVertex>();
    float penX = 0.0f, penY = -1.0f;

    for (wchar_t character : text) {
        if (character == L'\n') {
            penX = 0.0f;
            penY -= 1.0f;

            continue;
        }

        const Glyph *glyph = this->getGlyph(character);
        if (glyph == nullptr) continue;

        float x0 = penX + glyph->boundsLBRT.x;
        float y0 = penY + glyph->boundsLBRT.y;
        float x1 = penX + glyph->boundsLBRT.z;
        float y1 = penY + glyph->boundsLBRT.w;

        glm::vec2 uv0 = glm::vec2(glyph->uvLBRT.x, glyph->uvLBRT.y);
        glm::vec2 uv1 = glm::vec2(glyph->uvLBRT.z, glyph->uvLBRT.w);

        vertices.emplace_back(x0, y0, uv0.x, uv0.y);
        vertices.emplace_back(x1, y0, uv1.x, uv0.y);
        vertices.emplace_back(x1, y1, uv1.x, uv1.y);

        vertices.emplace_back(x0, y0, uv0.x, uv0.y);
        vertices.emplace_back(x1, y1, uv1.x, uv1.y);
        vertices.emplace_back(x0, y1, uv0.x, uv1.y);

        penX += glyph->advance;
    }

    return vertices;
}

TextMesh::TextMesh() {
    this->vertexBuffer.setData(std::vector<float>{
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f
    });
    this->vertexArray.bindVertexBuffer(this->vertexBuffer, std::vector<VertexAttribute>{
        {
            .type = VertexAttributeType::Float,
            .size = VertexAttributeSize::Vec2,
        }
    });
    this->vertexArray.bindVertexBuffer(this->instanceBuffer, std::vector<VertexAttribute>{
        {
            .type = VertexAttributeType::Float,
            .size = VertexAttributeSize::Vec4,
            .divisor = VertexAttributeDivisor::PerInstance
        },
        {
            .type = VertexAttributeType::Float,
            .size = VertexAttributeSize::Vec4,
            .divisor = VertexAttributeDivisor::PerInstance
        },
    });
}
TextMesh::~TextMesh() {}

struct TextInstance {
    float x, y, w, h;
    float u, v, uw, vh;
};

void TextMesh::setText(const std::wstring &text, const Font &font) {
    std::vector<TextInstance> instances = std::vector<TextInstance>();
    instances.reserve(text.size());

    float penX = 0.0f, penY = -1.0f;
    for (wchar_t character : text) {
        if (character == L'\n') {
            penX = 0.0f;
            penY -= 1.0f;

            continue;
        }

        const Glyph *glyph = font.getGlyph(character);
        if (glyph == nullptr) continue;

        float x0 = penX + glyph->boundsLBRT.x;
        float y0 = penY + glyph->boundsLBRT.y;
        float x1 = penX + glyph->boundsLBRT.z;
        float y1 = penY + glyph->boundsLBRT.w;

        float u0 = glyph->uvLBRT.x;
        float v0 = glyph->uvLBRT.y;
        float u1 = glyph->uvLBRT.z;
        float v1 = glyph->uvLBRT.w;

        instances.emplace_back(x0, y0, x1 - x0, y1 - y0, u0, v0, u1 - u0, v1 - v0);
        penX += glyph->advance;
    }

    this->instanceBuffer.setData(instances);
    this->numInstances = static_cast<uint32_t>(instances.size());
}
void TextMesh::draw() const {
    this->vertexArray.drawArraysInstanced(Topology::TriangleFan, this->numInstances);
}