#pragma once
#include <unordered_map>
#include <inttypes.h>
#include <glm/glm.hpp>
#include <variant>
#include <optional>
#include <string>
#include <engine/graphics/gui/text.hpp>

enum class Position : uint8_t {
    Fixed, // Place exactly at (x, y)
    Absolute, // Place exactly at (x, y) relative to parent
    Relative // Give parent full control over placement
};

enum class Sizing : uint8_t {
    Fit, Grow, Constraint
};
class Size {
private:
    float constraint = 0.0f;
    Sizing sizing = Sizing::Constraint;
public:
    Size();
    Size(float value);
    Size(Sizing sizing);
    ~Size();

    void setConstraint(float value);
    void setSizing(Sizing sizing);

    std::optional<float> getConstraint() const;
    Sizing getSizing() const;
};
struct Edges {
    float left = 0.0f;
    float bottom = 0.0f;
    float right = 0.0f;
    float top = 0.0f;
};

class Color {
private:
    glm::u8vec4 color = glm::u8vec4(255);
public:
    Color();
    Color(const glm::u8vec3 &color);
    Color(const glm::u8vec4 &color);
    Color(const glm::vec3 &color);
    Color(const glm::vec4 &color);
    Color(uint8_t red, uint8_t green, uint8_t blue);
    Color(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha);
    Color(uint32_t hex);
    // TODO: Add more constructors (e.g., from HSL, HSV, etc.)
    ~Color();

    void set(const glm::u8vec3 &color);
    void set(const glm::u8vec4 &color);
    void set(const glm::vec3 &color);
    void set(const glm::vec4 &color);
    void set(uint8_t red, uint8_t green, uint8_t blue);
    void set(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha);
    void set(uint32_t hex);

    const glm::u8vec4 &get() const;
};

enum class Layout : uint8_t {
    Flex, /* Grid */
};
enum class LayoutDirection : uint8_t {
    Row, Column
};
enum class Align : uint8_t {
    Start, Center, End
};

struct Texture;
class Style {
private:
    enum class Property {
        Position,
        X, Y, Width, Height,
        TextColor, BackgroundColor, BackgroundImage,
        Padding, Margin, Gap,
        Layout, LayoutDirection,
        ContentAlignX, ContentAlignY,
        TextAlignX, TextAlignY,
    };
    std::unordered_map<Property, std::variant<float, Color, Texture*, Edges, Size, Position, Layout, LayoutDirection, Align>> properties;
public:
    Style();
    ~Style();

    // void setPosition(Position position);
    void setX(float x);
    void setY(float y);
    void setWidth(Size width);
    void setHeight(Size height);
    void setBackgroundColor(Color color);
    void setTextColor(Color color);
    void setBackgroundImage(Texture &texture);
    void setPadding(const Edges &padding);
    void setMargin(const Edges &margin);
    void setGap(float gap);
    void setPosition(Position position);
    // void setLayout(Layout layout);
    void setLayoutDirection(LayoutDirection direction);
    void setContentAlignX(Align align);
    void setContentAlignY(Align align);
    void setTextAlignX(Align align);
    void setTextAlignY(Align align);

    // TODO: Look if std::optional is even needed here
    std::optional<float> getX() const;
    std::optional<float> getY() const;
    std::optional<Size> getWidth() const;
    std::optional<Size> getHeight() const;
    std::optional<Color> getBackgroundColor() const;
    std::optional<Color> getTextColor() const;
    std::optional<Texture*> getBackgroundImage() const;
    std::optional<Edges> getPadding() const;
    std::optional<Edges> getMargin() const;
    std::optional<float> getGap() const;
    std::optional<Position> getPosition() const;
    // std::optional<Layout> getLayout() const;
    std::optional<LayoutDirection> getLayoutDirection() const;
    std::optional<Align> getContentAlignX() const;
    std::optional<Align> getContentAlignY() const;
    std::optional<Align> getTextAlignX() const;
    std::optional<Align> getTextAlignY() const;
};

class GuiElementComponent {
public:
    struct Text {
        std::wstring content = std::wstring();
        TextMesh textMesh = TextMesh();
        Font *font = nullptr;
        float fontSize = 12.0f;
        glm::vec2 computedPosition = glm::vec2();
        glm::vec2 computedSize = glm::vec2();
    };
private:
    Style *groupStyle = nullptr;
    Text *text = nullptr;
    bool dirty = true;
public:
    Style style = Style();
    glm::vec2 computedPosition = glm::vec2(), computedSize = glm::vec2();

    GuiElementComponent();
    explicit GuiElementComponent(Style &style);
    ~GuiElementComponent();

    void setGroupStyle(Style &style);
    const Style *getGroupStyle() const;

    void setText(const std::wstring &content);
    void setFont(Font &font);
    void setFontSize(float size);

    const Text *getText() const;
    Text *getMutableText();

    void markClean();
    bool isDirty() const;
};