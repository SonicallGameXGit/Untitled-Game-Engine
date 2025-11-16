#include "gui.hpp"
#define GET_PROPERTY(key, value) if (this->properties.find(Property::key) != this->properties.end()) { \
    return std::get<value>(this->properties.at(Property::key)); \
} \
return std::nullopt

GuiElementComponent::GuiElementComponent() {}
GuiElementComponent::GuiElementComponent(Style &groupStyle) : groupStyle(&groupStyle) {}
GuiElementComponent::~GuiElementComponent() {
    if (this->text != nullptr) {
        delete this->text;
    }
}
void GuiElementComponent::setGroupStyle(Style &style) { this->groupStyle = &style; }
const Style *GuiElementComponent::getGroupStyle() const { return this->groupStyle; }

Size::Size() : constraint(0.0f), sizing(Sizing::Constraint) {}
Size::Size(float constraint) : constraint(constraint), sizing(Sizing::Constraint) {}
Size::Size(Sizing sizing) : constraint(0.0f), sizing(sizing) {}
Size::~Size() {}

void Size::setConstraint(float value) {
    this->constraint = value;
    this->sizing = Sizing::Constraint;
}
void Size::setSizing(Sizing sizing) { this->sizing = sizing; }
std::optional<float> Size::getConstraint() const {
    return this->sizing == Sizing::Constraint ? std::make_optional(this->constraint) : std::nullopt;
}
Sizing Size::getSizing() const { return this->sizing; }

Color::Color() : color(255) {}
Color::Color(const glm::u8vec3 &color) : color(glm::u8vec4(color, 255)) {}
Color::Color(const glm::u8vec4 &color) : color(color) {}
Color::Color(uint8_t red, uint8_t green, uint8_t blue) : color(glm::u8vec4(red, green, blue, 255)) {}
Color::Color(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha) : color(glm::u8vec4(red, green, blue, alpha)) {}
Color::Color(const glm::vec3 &color) : color(glm::u8vec4(
    static_cast<uint8_t>(glm::clamp(color.r * 255.0f, 0.0f, 255.0f)),
    static_cast<uint8_t>(glm::clamp(color.g * 255.0f, 0.0f, 255.0f)),
    static_cast<uint8_t>(glm::clamp(color.b * 255.0f, 0.0f, 255.0f)),
    255
)) {}
Color::Color(const glm::vec4 &color) : color(glm::u8vec4(
    static_cast<uint8_t>(glm::clamp(color.r * 255.0f, 0.0f, 255.0f)),
    static_cast<uint8_t>(glm::clamp(color.g * 255.0f, 0.0f, 255.0f)),
    static_cast<uint8_t>(glm::clamp(color.b * 255.0f, 0.0f, 255.0f)),
    static_cast<uint8_t>(glm::clamp(color.a * 255.0f, 0.0f, 255.0f))
)) {}
Color::Color(uint32_t hex) : color(glm::u8vec4(
    (hex >> 24) & 0xFF,
    (hex >> 16) & 0xFF,
    (hex >> 8) & 0xFF,
    hex & 0xFF
)) {}
Color::~Color() {}

void Color::set(const glm::u8vec3 &color) { this->color = glm::u8vec4(color, 255); }
void Color::set(const glm::u8vec4 &color) { this->color = color; }
void Color::set(uint8_t red, uint8_t green, uint8_t blue) { this->color = glm::u8vec4(red, green, blue, 255); }
void Color::set(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha) { this->color = glm::u8vec4(red, green, blue, alpha); }
void Color::set(const glm::vec3 &color) {
    this->color = glm::u8vec4(
        static_cast<uint8_t>(glm::clamp(color.r * 255.0f, 0.0f, 255.0f)),
        static_cast<uint8_t>(glm::clamp(color.g * 255.0f, 0.0f, 255.0f)),
        static_cast<uint8_t>(glm::clamp(color.b * 255.0f, 0.0f, 255.0f)),
        255
    );
}
void Color::set(const glm::vec4 &color) {
    this->color = glm::u8vec4(
        static_cast<uint8_t>(glm::clamp(color.r * 255.0f, 0.0f, 255.0f)),
        static_cast<uint8_t>(glm::clamp(color.g * 255.0f, 0.0f, 255.0f)),
        static_cast<uint8_t>(glm::clamp(color.b * 255.0f, 0.0f, 255.0f)),
        static_cast<uint8_t>(glm::clamp(color.a * 255.0f, 0.0f, 255.0f))
    );
}
void Color::set(uint32_t hex) {
    this->color = glm::u8vec4(
        (hex >> 24) & 0xFF,
        (hex >> 16) & 0xFF,
        (hex >> 8) & 0xFF,
        hex & 0xFF
    );
}
const glm::u8vec4 &Color::get() const { return this->color; }

Style::Style() {}
Style::~Style() {}

void Style::setX(float x) { this->properties[Property::X] = x; }
void Style::setY(float y) { this->properties[Property::Y] = y; }
void Style::setWidth(Size width) { this->properties[Property::Width] = width; }
void Style::setHeight(Size height) { this->properties[Property::Height] = height; }
void Style::setBackgroundColor(Color color) { this->properties[Property::BackgroundColor] = color; }
void Style::setTextColor(Color color) { this->properties[Property::TextColor] = color; }
void Style::setBackgroundImage(Texture &texture) { this->properties[Property::BackgroundImage] = &texture; }
void Style::setPadding(const Edges &padding) { this->properties[Property::Padding] = padding; }
void Style::setMargin(const Edges &margin) { this->properties[Property::Margin] = margin; }
void Style::setGap(float gap) { this->properties[Property::Gap] = gap; }
void Style::setPosition(Position position) { this->properties[Property::Position] = position; }
// void Style::setLayout(Layout layout) { this->properties[Property::Layout] = layout; }
void Style::setLayoutDirection(LayoutDirection direction) { this->properties[Property::LayoutDirection] = direction; }
void Style::setContentAlignX(Align align) { this->properties[Property::ContentAlignX] = align; }
void Style::setContentAlignY(Align align) { this->properties[Property::ContentAlignY] = align; };
void Style::setTextAlignX(Align align) { this->properties[Property::TextAlignX] = align; }
void Style::setTextAlignY(Align align) { this->properties[Property::TextAlignY] = align; }
void Style::setHoverable(bool hoverable) { this->properties[Property::Hoverable] = hoverable; }

std::optional<float> Style::getX() const { GET_PROPERTY(X, float); }
std::optional<float> Style::getY() const { GET_PROPERTY(Y, float); }
std::optional<Size> Style::getWidth() const { GET_PROPERTY(Width, Size); }
std::optional<Size> Style::getHeight() const { GET_PROPERTY(Height, Size); }
std::optional<Color> Style::getBackgroundColor() const { GET_PROPERTY(BackgroundColor, Color); }
std::optional<Color> Style::getTextColor() const { GET_PROPERTY(TextColor, Color); }
std::optional<Texture*> Style::getBackgroundImage() const { GET_PROPERTY(BackgroundImage, Texture*); }
std::optional<Edges> Style::getPadding() const { GET_PROPERTY(Padding, Edges); }
std::optional<Edges> Style::getMargin() const { GET_PROPERTY(Margin, Edges); }
std::optional<float> Style::getGap() const { GET_PROPERTY(Gap, float); }
std::optional<Position> Style::getPosition() const { GET_PROPERTY(Position, Position); }
// std::optional<Layout> Style::getLayout() const { GET_PROPERTY(Layout, Layout); }
std::optional<LayoutDirection> Style::getLayoutDirection() const { GET_PROPERTY(LayoutDirection, LayoutDirection); }
std::optional<Align> Style::getContentAlignX() const { GET_PROPERTY(ContentAlignX, Align); }
std::optional<Align> Style::getContentAlignY() const { GET_PROPERTY(ContentAlignY, Align); }
std::optional<Align> Style::getTextAlignX() const { GET_PROPERTY(TextAlignX, Align); }
std::optional<Align> Style::getTextAlignY() const { GET_PROPERTY(TextAlignY, Align); }
std::optional<bool> Style::getHoverable() const { GET_PROPERTY(Hoverable, bool); }

void GuiElementComponent::setText(const std::wstring &content) {
    if (this->text == nullptr) {
        this->text = new Text();
    }
    this->text->content = content;
    if (this->text->font != nullptr) {
        this->text->textMesh.setText(content, *this->text->font);
        this->text->computedSize = this->text->font->getTextSize(content) * this->text->fontSize;
        this->dirty = true;
    }
}
void GuiElementComponent::setFont(Font &font) {
    if (this->text == nullptr) {
        this->text = new Text();
    }
    this->text->font = &font;
    this->text->textMesh.setText(this->text->content, font);
    this->text->computedSize = font.getTextSize(this->text->content) * this->text->fontSize;
    this->dirty = true;
}
void GuiElementComponent::setFontSize(float size) {
    if (this->text == nullptr) {
        this->text = new Text();
    }
    this->text->fontSize = size;
    if (this->text->font != nullptr) {
        this->text->textMesh.setText(this->text->content, *this->text->font);
        this->text->computedSize = this->text->font->getTextSize(this->text->content) * this->text->fontSize;
        this->dirty = true;
    }
}

const GuiElementComponent::Text *GuiElementComponent::getText() const {
    return this->text;
}
GuiElementComponent::Text *GuiElementComponent::getMutableText() {
    return this->text;
}

bool GuiElementComponent::isClicked() const {
    return this->clicked;
}
bool GuiElementComponent::isHovered() const {
    return this->hovered;
}