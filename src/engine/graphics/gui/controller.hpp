#pragma once
#include <framework/graphics/window.hpp>
#include <framework/graphics/shader.hpp>
#include <framework/graphics/buffers.hpp>
#include <engine/ecs/components/gui.hpp>
#include <engine/world/world.hpp>
#include <glm/glm.hpp>

struct GuiController {
private:
    glm::vec2 fitGuiElements(World &world, Entity entity, GuiElementComponent &element) const;
    void growGuiElements(World &world, Entity entity, GuiElementComponent &element, const glm::vec2 &availableSpace) const;
    void positionGuiElements(World &world, Entity entity, GuiElementComponent &element) const;
    void drawGuiElements(
        const Window &window,
        World &world,
        Entity entity,
        GuiElementComponent &element,
        const VertexArray &spriteVertexArray,
        const ShaderProgram &guiShader, const ShaderProgram &textShader,
        const glm::vec2 &clipPosition, const glm::vec2 &clipSize,
        const glm::vec2 &viewport,
        const glm::mat4 &projectionViewMatrix
    ) const;
    void drawGuiElement(
        const VertexArray &spriteVertexArray,
        const ShaderProgram &guiShader, const ShaderProgram &textShader,
        const GuiElementComponent &element,
        const glm::vec2 &clipPosition, const glm::vec2 &clipSize,
        const glm::vec2 &viewport,
        const glm::mat4 &projectionViewMatrix
    ) const;
public:
    void render(const Window &window, World &world, const VertexArray &spriteVertexArray, const ShaderProgram &guiShader, const ShaderProgram &textShader) const;
};