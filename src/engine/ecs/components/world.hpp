#pragma once
#include <engine/graphics/gui/text.hpp>
#include <framework/graphics/texture.hpp>

class TextComponent {
private:
    TextMesh textMesh = TextMesh();
    Font *font = nullptr;
    std::wstring text = std::wstring();
public:
    float textHeight = 1.0f;
    glm::vec4 color = glm::vec4(1.0f);

    explicit TextComponent(Font &font);
    TextComponent(Font &font, const std::wstring &text);
    TextComponent(Font &font, const std::wstring &text, const glm::vec4 &color);
    ~TextComponent();

    void setFont(Font &font);
    void setText(const std::wstring &text);

    const Font *getFont() const;
    const std::wstring &getText() const;
    const TextMesh &getMesh() const;
};
struct SpriteComponent {
    Texture *texture = nullptr;
    glm::vec4 color = glm::vec4(1.0f);

    SpriteComponent();
    explicit SpriteComponent(Texture &texture);
    SpriteComponent(Texture &texture, const glm::vec4 &color);
    explicit SpriteComponent(const glm::vec4 &color);
    ~SpriteComponent();
};