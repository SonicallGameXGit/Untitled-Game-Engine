#include "gui.hpp"

TextComponent::TextComponent(Font &font) : font(&font) {}
TextComponent::TextComponent(Font &font, const std::wstring &text) : font(&font), text(text) {
    if (!text.empty()) {
        this->textMesh.setText(text, *this->font);
    }
}
TextComponent::TextComponent(Font &font, const std::wstring &text, const glm::vec4 &color) :
    font(&font), text(text), color(color)
{
    if (!text.empty()) {
        this->textMesh.setText(text, *this->font);
    }
}
TextComponent::~TextComponent() {}

void TextComponent::setFont(Font &font) {
    this->font = &font;
    if (!this->text.empty()) {
        this->textMesh.setText(this->text, *this->font);
    } else {
        this->textMesh = TextMesh();
    }
}
void TextComponent::setText(const std::wstring &text) {
    this->text = text;

    if (!text.empty() && this->font != nullptr) {
        this->textMesh.setText(text, *this->font);
    } else {
        this->textMesh = TextMesh();
    }
}

const Font *TextComponent::getFont() const {
    return this->font;
}
const std::wstring &TextComponent::getText() const {
    return this->text;
}
const TextMesh &TextComponent::getMesh() const {
    return this->textMesh;
}

SpriteComponent::SpriteComponent() {}
SpriteComponent::SpriteComponent(Texture &texture) : texture(&texture) {}
SpriteComponent::SpriteComponent(Texture &texture, const glm::vec4 &color) : texture(&texture), color(color) {}
SpriteComponent::SpriteComponent(const glm::vec4 &color) : color(color) {}
SpriteComponent::~SpriteComponent() {}