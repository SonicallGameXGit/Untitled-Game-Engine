#pragma once
#include <engine/graphics/gui/text.hpp>
#include <framework/graphics/texture.hpp>

class TextComponent {
private:
    TextMesh textMesh = TextMesh();
    std::wstring text = std::wstring();
    Font *font = nullptr;
public:
    glm::vec4 color = glm::vec4(1.0f);

    TextComponent(Font &font);
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
    SpriteComponent(Texture &texture);
    SpriteComponent(Texture &texture, const glm::vec4 &color);
    SpriteComponent(const glm::vec4 &color);
    ~SpriteComponent();
};