#include "renderer.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <engine/ecs/components/mesh.hpp>
#include <engine/ecs/components/transform.hpp>
#include <engine/ecs/components/world.hpp>
#include <engine/ecs/components/gui.hpp>
#include <engine/graphics/gui/text.hpp>

static void drawWorld(const ShaderProgram &shader, const Camera &camera, const MeshComponent &mesh, const glm::mat4 &projectionViewMatrix, const glm::mat4 &modelMatrix) {
    shader.setMat4("u_ProjectionViewMatrix", projectionViewMatrix);
    shader.setMat4("u_ModelMatrix", modelMatrix);
    shader.setVec3("u_ViewPosition", camera.position);
    shader.setVec4("u_Color", mesh.color);

    const Texture *texture = mesh.getTexture();
    if (texture != nullptr) {
        texture->bind(0);
        shader.setBool("u_HasTexture", true);
    } else {
        shader.setBool("u_HasTexture", false);
    }

    mesh.getMesh()->draw();
}
static void drawSprite(const VertexArray &vertexArray, const ShaderProgram &shader, const SpriteComponent &sprite, const glm::mat4 &projectionViewMatrix, const glm::mat4 &modelMatrix) {
    shader.setMat4("u_ProjectionViewMatrix", projectionViewMatrix);
    shader.setMat4("u_ModelMatrix", modelMatrix);
    shader.setVec4("u_Color", sprite.color);

    if (sprite.texture != nullptr) {
        sprite.texture->bind(0);
        shader.setBool("u_HasTexture", true);
    } else {
        shader.setBool("u_HasTexture", false);
    }

    vertexArray.drawArrays(Topology::TriangleFan);
}
static void drawGuiElement(const VertexArray &vertexArray, const ShaderProgram &shader, const GuiElementComponent &element, const glm::mat4 &projectionViewMatrix, const glm::vec2 &position, const glm::vec2 &size) {
    const Style *groupStyle = element.getGroupStyle();
    shader.setMat4("u_ProjectionViewMatrix", projectionViewMatrix);
    shader.setVec2("u_Position", position);
    shader.setVec2("u_Size", size);

    std::optional<Color> backgroundColor = element.style.getBackgroundColor();
    if (!backgroundColor.has_value() && groupStyle != nullptr) {
        backgroundColor = groupStyle->getBackgroundColor();
    }
    Texture *texture = element.style.getBackgroundImage().value_or(
        groupStyle == nullptr ? nullptr : groupStyle->getBackgroundImage().value_or(nullptr)
    );
    if (texture == nullptr) {
        if (backgroundColor.has_value()) {
            if (backgroundColor->get().a == 0) {
                return;
            }
        } else {
            return;
        }
    }
    shader.setVec4("u_Color", glm::vec4(backgroundColor.value_or(Color(255, 255, 255)).get()) / 255.0f);

    if (texture != nullptr) {
        texture->bind(0);
        shader.setBool("u_HasTexture", true);
    } else {
        shader.setBool("u_HasTexture", false);
    }

    vertexArray.drawArrays(Topology::TriangleFan);
}
static void drawText(const ShaderProgram &shader, const TextComponent &text, const glm::mat4 &projectionViewMatrix, const glm::mat4 &modelMatrix) {
    const Font *font = text.getFont();
    if (font == nullptr) {
        return;
    }

    shader.setMat4("u_ProjectionViewMatrix", projectionViewMatrix);
    shader.setMat4("u_ModelMatrix", modelMatrix);
    shader.setVec4("u_Color", text.color);

    font->getTexture().bind(0);
    text.getMesh().draw();
}

Renderer::Renderer() {
    this->worldShader.bind();
    this->worldShader.setInt("u_ColorSampler", 0);

    this->spriteShader.bind();
    this->spriteShader.setInt("u_ColorSampler", 0);

    this->textShader.bind();
    this->textShader.setInt("u_ColorSampler", 0);
    this->textShader.setFloat("u_PixelRange", Font::PIXEL_RANGE);

    this->spriteVertexArray.bindVertexBuffer(this->spriteVertexBuffer, {
        {
            .type = VertexAttributeType::Float,
            .size = VertexAttributeSize::Vec2,
        }
    });
}
Renderer::~Renderer() {}

static void precomputeWorldMatrices(World &world, std::optional<Entity> parent) {
    for (auto [_, value] : world.getChildren(parent)) {
        if (world.hasComponents<Transform3DComponent>(value)) {
            Transform3DComponent &transform = world.getMutableComponent<Transform3DComponent>(value);
            if (parent.has_value() && world.hasComponents<Transform3DComponent>(parent.value())) {
                transform.worldModelMatrix = world.getMutableComponent<Transform3DComponent>(parent.value()).worldModelMatrix * transform.getModelMatrix();
            } else {
                transform.worldModelMatrix = transform.getModelMatrix();
            }
        } else if (world.hasComponents<Transform2DComponent>(value)) {
            Transform2DComponent &transform = world.getMutableComponent<Transform2DComponent>(value);
            float scale = 1.0f;
            if (world.hasComponents<TextComponent>(value)) {
                scale = world.getComponent<TextComponent>(value).textHeight;
            }
            if (parent.has_value() && world.hasComponents<Transform2DComponent>(parent.value())) {
                transform.worldModelMatrix = world.getMutableComponent<Transform2DComponent>(parent.value()).worldModelMatrix * glm::scale(transform.getModelMatrix(), glm::vec3(scale, scale, 1.0f));
            } else {
                transform.worldModelMatrix = glm::scale(transform.getModelMatrix(), glm::vec3(scale, scale, 1.0f));
            }
        }

        precomputeWorldMatrices(world, value);
    }
}

static glm::vec2 fitGuiElements(World &world, Entity entity, GuiElementComponent &element) {
    const Style *myGroupStyle = element.getGroupStyle();
    Edges myPadding = element.style.getPadding().value_or(
        myGroupStyle == nullptr ? Edges {} : myGroupStyle->getPadding().value_or(
            Edges {}
        )
    );
    Edges myMargin = element.style.getMargin().value_or(
        myGroupStyle == nullptr ? Edges {} : myGroupStyle->getMargin().value_or(
            Edges {}
        )
    );

    std::optional<Size> myWidth = element.style.getWidth();
    if (!myWidth.has_value()) {
        myWidth = myGroupStyle == nullptr ? std::nullopt : myGroupStyle->getWidth();
    }
    std::optional<Size> myHeight = element.style.getHeight();
    if (!myHeight.has_value()) {
        myHeight = myGroupStyle == nullptr ? std::nullopt : myGroupStyle->getHeight();
    }

    if (myWidth.has_value() && myWidth->getConstraint().has_value()) {
        element.computedSize.x = myWidth->getConstraint().value();
    } else {
        element.computedSize.x = 0.0f;
    }
    if (myHeight.has_value() && myHeight->getConstraint().has_value()) {
        element.computedSize.y = myHeight->getConstraint().value();
    } else {
        element.computedSize.y = 0.0f;
    }

    LayoutDirection layoutDirection = element.style.getLayoutDirection().value_or(
        myGroupStyle == nullptr ? LayoutDirection::Row : myGroupStyle->getLayoutDirection().value_or(
            LayoutDirection::Row
        )
    );

    int32_t numElements = 0;
    for (auto [_, child] : world.getChildren(entity)) {
        if (!world.hasComponents<GuiElementComponent>(child)) {
            continue;
        }
        GuiElementComponent &childElement = world.getMutableComponent<GuiElementComponent>(child);
        Position childPosition = childElement.style.getPosition().value_or(
            childElement.getGroupStyle() == nullptr ? Position::Relative : childElement.getGroupStyle()->getPosition().value_or(
                Position::Relative
            )
        );
        glm::vec2 childUsedSpace = fitGuiElements(world, child, childElement);
        if (childPosition != Position::Relative) {
            continue;
        }

        if (layoutDirection == LayoutDirection::Row) {
            if (!myWidth.has_value() || !myWidth->getConstraint().has_value()) {
                element.computedSize.x += childUsedSpace.x;
            }
            if (!myHeight.has_value() || !myHeight->getConstraint().has_value()) {
                element.computedSize.y = std::max(element.computedSize.y, childUsedSpace.y);
            }
        } else {
            if (!myHeight.has_value() || !myHeight->getConstraint().has_value()) {
                element.computedSize.y += childUsedSpace.y;
            }
            if (!myWidth.has_value() || !myWidth->getConstraint().has_value()) {
                element.computedSize.x = std::max(element.computedSize.x, childUsedSpace.x);
            }
        }

        numElements++;
    }

    float myGap = element.style.getGap().value_or(
        myGroupStyle == nullptr ? 0.0f : myGroupStyle->getGap().value_or(0.0f)
    );
    if (layoutDirection == LayoutDirection::Row) {
        element.computedSize.x += myGap * static_cast<float>(std::max(0, numElements - 1));
    } else {
        element.computedSize.y += myGap * static_cast<float>(std::max(0, numElements - 1));
    }
    element.computedSize.x += myPadding.left + myPadding.right;
    element.computedSize.y += myPadding.top + myPadding.bottom;
    return glm::vec2(element.computedSize.x + myMargin.left + myMargin.right, element.computedSize.y + myMargin.top + myMargin.bottom);
}
static void growGuiElements(World &world, Entity entity, GuiElementComponent &element, const glm::vec2 &availableSpace) {
    Position myPosition = element.style.getPosition().value_or(
        element.getGroupStyle() == nullptr ? Position::Relative : element.getGroupStyle()->getPosition().value_or(
            Position::Relative
        )
    );
    std::optional<Size> myWidth = element.style.getWidth();
    std::optional<Size> myHeight = element.style.getHeight();
    if (!myWidth.has_value()) {
        myWidth = element.getGroupStyle() == nullptr ? std::nullopt : element.getGroupStyle()->getWidth();
    }
    if (!myHeight.has_value()) {
        myHeight = element.getGroupStyle() == nullptr ? std::nullopt : element.getGroupStyle()->getHeight();
    }

    if (myPosition == Position::Relative) {
        Edges myMargin = element.style.getMargin().value_or(
            element.getGroupStyle() == nullptr ? Edges {} : element.getGroupStyle()->getMargin().value_or(
                Edges {}
            )
        );
        if (myWidth.has_value() && myWidth->getSizing() == Sizing::Grow) {
            element.computedSize.x = availableSpace.x - myMargin.left - myMargin.right;
        }
        if (myHeight.has_value() && myHeight->getSizing() == Sizing::Grow) {
            element.computedSize.y = availableSpace.y - myMargin.top - myMargin.bottom;
        }
    }

    Edges myPadding = element.style.getPadding().value_or(
        element.getGroupStyle() == nullptr ? Edges {} : element.getGroupStyle()->getPadding().value_or(
            Edges {}
        )
    );

    LayoutDirection layoutDirection = element.style.getLayoutDirection().value_or(
        element.getGroupStyle() == nullptr ? LayoutDirection::Row : element.getGroupStyle()->getLayoutDirection().value_or(
            LayoutDirection::Row
        )
    );

    glm::vec2 myAvailableSpace = glm::vec2(
        element.computedSize.x - myPadding.left - myPadding.right,
        element.computedSize.y - myPadding.top - myPadding.bottom
    );
    int32_t numSizedElements = 0, numGrowElements = 0;
    float accumulatedSized = 0.0f;
    for (auto [_, child] : world.getChildren(entity)) {
        if (!world.hasComponents<GuiElementComponent>(child)) {
            continue;
        }
        GuiElementComponent &childElement = world.getMutableComponent<GuiElementComponent>(child);
        Position childPosition = childElement.style.getPosition().value_or(
            childElement.getGroupStyle() == nullptr ? Position::Relative : childElement.getGroupStyle()->getPosition().value_or(
                Position::Relative
            )
        );
        switch (childPosition) {
            case Position::Fixed: continue;
            case Position::Absolute: continue;
            case Position::Relative: break;
            default: break;
        }
        Size childWidth = childElement.style.getWidth().value_or(
            childElement.getGroupStyle() == nullptr ? Size() : childElement.getGroupStyle()->getWidth().value_or(Size())
        );
        Size childHeight = childElement.style.getHeight().value_or(
            childElement.getGroupStyle() == nullptr ? Size() : childElement.getGroupStyle()->getHeight().value_or(Size())
        );

        if (layoutDirection == LayoutDirection::Row) {
            if (childWidth.getSizing() == Sizing::Grow) {
                numGrowElements++;
            } else {
                accumulatedSized += childElement.computedSize.x;
                numSizedElements++;
            }
        } else {
            if (childHeight.getSizing() == Sizing::Grow) {
                numGrowElements++;
            } else {
                accumulatedSized += childElement.computedSize.y;
                numSizedElements++;
            }
        }
    }

    float myGap = element.style.getGap().value_or(
        element.getGroupStyle() == nullptr ? 0.0f : element.getGroupStyle()->getGap().value_or(0.0f)
    );
    float totalGap = myGap * static_cast<float>(std::max(0, numGrowElements + numSizedElements - 1));
    float perGrowSize = layoutDirection == LayoutDirection::Row ? myAvailableSpace.x : myAvailableSpace.y;
    if (numGrowElements > 0) {
        perGrowSize = (perGrowSize - accumulatedSized - totalGap) / static_cast<float>(numGrowElements);
    }
    for (auto [_, child] : world.getChildren(entity)) {
        if (!world.hasComponents<GuiElementComponent>(child)) {
            continue;
        }
        growGuiElements(world, child, world.getMutableComponent<GuiElementComponent>(child), layoutDirection == LayoutDirection::Row ? glm::vec2(perGrowSize, myAvailableSpace.y) : glm::vec2(myAvailableSpace.x, perGrowSize));
    }
}
static void positionGuiElements(World &world, Entity entity, GuiElementComponent &element) {
    // WARN: Expecting every element is a horizontal flex container
    const Style *myGroupStyle = element.getGroupStyle();
    Edges myPadding = element.style.getPadding().value_or(
        myGroupStyle == nullptr ? Edges {} : myGroupStyle->getPadding().value_or(
            Edges {}
        )
    );
    Edges myMargin = element.style.getMargin().value_or(
        myGroupStyle == nullptr ? Edges {} : myGroupStyle->getMargin().value_or(
            Edges {}
        )
    );

    Position myPosition = element.style.getPosition().value_or(
        myGroupStyle == nullptr ? Position::Relative : myGroupStyle->getPosition().value_or(
            Position::Relative
        )
    );

    Align myContentAlignX = element.style.getContentAlignX().value_or(
        myGroupStyle == nullptr ? Align::Start : myGroupStyle->getContentAlignX().value_or(
            Align::Start
        )
    );
    Align myContentAlignY = element.style.getContentAlignY().value_or(
        myGroupStyle == nullptr ? Align::Start : myGroupStyle->getContentAlignY().value_or(
            Align::Start
        )
    );

    switch (myPosition) {
        case Position::Fixed: {
            element.computedPosition.x = element.style.getX().value_or(
                myGroupStyle == nullptr ? 0.0f : myGroupStyle->getX().value_or(0.0f)
            );
            element.computedPosition.y = element.style.getY().value_or(
                myGroupStyle == nullptr ? 0.0f : myGroupStyle->getY().value_or(0.0f)
            );
            break;
        }
        case Position::Absolute: {
            element.computedPosition.x += element.style.getX().value_or(
                myGroupStyle == nullptr ? 0.0f : myGroupStyle->getX().value_or(0.0f)
            );
            element.computedPosition.y += element.style.getY().value_or(
                myGroupStyle == nullptr ? 0.0f : myGroupStyle->getY().value_or(0.0f)
            );
            break;
        }
        case Position::Relative: {
            element.computedPosition.x += myMargin.left;
            element.computedPosition.y += myMargin.bottom;
            break;
        }
        default: break;
    };

    LayoutDirection layoutDirection = element.style.getLayoutDirection().value_or(
        myGroupStyle == nullptr ? LayoutDirection::Row : myGroupStyle->getLayoutDirection().value_or(
            LayoutDirection::Row
        )
    );

    float sourceOffset = layoutDirection == LayoutDirection::Row ? element.computedPosition.x + myPadding.left : (element.computedPosition.y + myPadding.bottom);
    float offset = sourceOffset;
    float myGap = element.style.getGap().value_or(
        myGroupStyle == nullptr ? 0.0f : myGroupStyle->getGap().value_or(0.0f)
    );

    const auto &childrenView = world.getChildren(entity);
    auto children = std::vector<std::tuple<Entity, EntityIdentifier, GuiElementComponent*>>();

    float totalChildrenWidth = 0.0f, totalChildrenHeight = 0.0f;
    for (auto [_, child] : childrenView) {
        if (!world.hasComponents<GuiElementComponent>(child)) {
            continue;
        }
        GuiElementComponent *childElement = &world.getMutableComponent<GuiElementComponent>(child);
        children.emplace_back(child, world.getComponent<EntityIdentifier>(child), childElement);

        if (layoutDirection == LayoutDirection::Row) {
            totalChildrenWidth += childElement->computedSize.x;
            totalChildrenHeight = std::max(totalChildrenHeight, childElement->computedSize.y);
        } else {
            totalChildrenHeight += childElement->computedSize.y;
            totalChildrenWidth = std::max(totalChildrenWidth, childElement->computedSize.x);
        }
    }
    if (layoutDirection == LayoutDirection::Row) {
        totalChildrenWidth += myGap * static_cast<float>(std::max(0, static_cast<int32_t>(children.size()) - 1));
    } else {
        totalChildrenHeight += myGap * static_cast<float>(std::max(0, static_cast<int32_t>(children.size()) - 1));
    }

    std::sort(children.begin(), children.end(), [](const auto &a, const auto &b) {
        return std::get<1>(a).index < std::get<1>(b).index;
    });

    for (auto [child, _, childElement] : children) {
        Position childPosition = childElement->style.getPosition().value_or(
            childElement->getGroupStyle() == nullptr ? Position::Relative : childElement->getGroupStyle()->getPosition().value_or(
                Position::Relative
            )
        );
        if (layoutDirection == LayoutDirection::Row) {
            if (childPosition == Position::Absolute) {
                childElement->computedPosition.x = sourceOffset;
            } else {
                childElement->computedPosition.x = offset;
            }
            childElement->computedPosition.y = element.computedPosition.y + myPadding.bottom;
        } else {
            if (childPosition == Position::Absolute) {
                childElement->computedPosition.y = sourceOffset;
            } else {
                childElement->computedPosition.y = offset;
            }
            childElement->computedPosition.x = element.computedPosition.x + myPadding.left;
        }

        if (childPosition == Position::Relative) {
            switch (myContentAlignX) {
                case Align::Start: break;
                case Align::Center: {
                    childElement->computedPosition.x += (element.computedSize.x - (layoutDirection == LayoutDirection::Row ? totalChildrenWidth : childElement->computedSize.x)) * 0.5f - myPadding.left - myPadding.right;
                    break; 
                }
                case Align::End: {
                    childElement->computedPosition.x += element.computedSize.x - (layoutDirection == LayoutDirection::Row ? totalChildrenWidth : childElement->computedSize.x) - myPadding.right - myPadding.left;
                    break;
                }
                default: break;
            };
            switch (myContentAlignY) {
                case Align::Start: break;
                case Align::Center: {
                    childElement->computedPosition.y += (element.computedSize.y - (layoutDirection == LayoutDirection::Row ? childElement->computedSize.y : totalChildrenHeight)) * 0.5f - myPadding.bottom - myPadding.top;
                    break; 
                }
                case Align::End: {
                    childElement->computedPosition.y += element.computedSize.y - (layoutDirection == LayoutDirection::Row ? childElement->computedSize.y : totalChildrenHeight) - myPadding.bottom - myPadding.top;
                    break;
                }
                default: break;
            };
        }

        positionGuiElements(world, child, *childElement);
        if (childPosition != Position::Relative) {
            continue;
        }

        offset += (layoutDirection == LayoutDirection::Row ? childElement->computedSize.x : childElement->computedSize.y) + myGap;
    }
}

void Renderer::render(const Window &window, World &world) const {
    // Step -1: Precompute world & GUI matrices for all entities
    // FIXME: Too slow / [Avg: 0.171936ms | Peak: 1.859586ms]
    // Debug::beginTimeMeasure();
    precomputeWorldMatrices(world, std::nullopt);
    {
        glm::vec2 viewport = glm::vec2(static_cast<float>(window.getWidth()), static_cast<float>(window.getHeight()));
        for (auto [_, root] : world.getChildren(std::nullopt)) {
            if (!world.hasComponents<GuiElementComponent>(root)) {
                continue;
            }

            GuiElementComponent &element = world.getMutableComponent<GuiElementComponent>(root);
            element.computedPosition.x = 0.0f;
            element.computedPosition.y = 0.0f;
            element.computedSize.x = 0.0f;
            element.computedSize.y = 0.0f;

            fitGuiElements(world, root, element);
            growGuiElements(world, root, element, viewport);
            positionGuiElements(world, root, element);
        }
    }
    // Debug::endTimeMeasure();

    // Step 0: Initial setup for rendering
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Step 1: Draw all opaque world objects
    {
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glDisable(GL_BLEND);

        glm::mat4 projectionViewMatrix = world.camera.getProjectionViewMatrix();
        auto transparentObjects = std::vector<std::tuple<MeshComponent, Transform3DComponent>>();
        for (auto [_, mesh, transform] : world.getAllEntitiesWith<MeshComponent, Transform3DComponent>().each()) {
            if (mesh.color.a < 1.0f) {
                transparentObjects.emplace_back(mesh, transform);
                continue;
            }

            drawWorld(this->worldShader, world.camera, mesh, projectionViewMatrix, transform.worldModelMatrix);
        }

        glEnable(GL_BLEND);
        // FIXME: Don't just cull the faces of all transparent objects, but try to find way to render polygons from back to front if possible
        // TODO: Add depth sorting for transparent objects
        for (auto &[mesh, transform] : transparentObjects) {
            drawWorld(this->worldShader, world.camera, mesh, world.camera.getProjectionViewMatrix(), transform.worldModelMatrix);
        }
    } // FIXME: Too slow / [Avg: 0.155663ms | Peak: 1.036083ms]

    // Step 2: Draw all 2D sprites (no need to sort them by depth or draw transparent ones separately because they're already sorted by spawn order)
    {
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
        glEnable(GL_BLEND);

        // EnTT doesn't guarantee that entt::entity (aka. Entity) would be the same as its creation order due to optimizations and other stuff.
        // TODO: If possible, optimize it, because that looks cringe
        // TODO: Try to render all sprites by iterating recursively trough hierarchy instead of recieving all of them and sorting
        auto entitiesView = world.getAllEntitiesWith<EntityIdentifier, Transform2DComponent>().each();
        auto entities = std::vector<std::tuple<Entity, EntityIdentifier, const Transform2DComponent*>>();
        entities.reserve(std::distance(entitiesView.begin(), entitiesView.end()));
        for (auto [entity, identifier, transform] : entitiesView) {
            entities.emplace_back(entity, identifier, &transform);
        }
        std::sort(entities.begin(), entities.end(), [](const auto& a, const auto& b) {
            return std::get<1>(a).index < std::get<1>(b).index;
        });

        glm::mat4 projectionViewMatrix = world.camera.getProjectionViewMatrix();
        for (auto [entity, identifier, transform] : entities) {
            if (world.hasComponents<SpriteComponent>(entity)) {
                drawSprite(
                    this->spriteVertexArray,
                    this->spriteShader,
                    world.getComponent<SpriteComponent>(entity),
                    projectionViewMatrix,
                    transform->worldModelMatrix
                );
                continue;
            }
            if (world.hasComponents<TextComponent>(entity)) {
                drawText(
                    this->textShader,
                    world.getComponent<TextComponent>(entity),
                    projectionViewMatrix,
                    transform->worldModelMatrix
                );
                continue;
            }
        }
    }

    // Debug::beginTimeMeasure();
    // Step 3: Draw all GUI elements (no need to sort them by depth or draw transparent ones separately because they're already sorted by spawn order)
    {
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
        glEnable(GL_BLEND);

        // EnTT doesn't guarantee that entt::entity (aka. Entity) would be the same as its creation order due to optimizations and other stuff.
        // TODO: If possible, optimize it, because that looks cringe
        auto entitiesView = world.getAllEntitiesWith<EntityIdentifier, GuiElementComponent>().each();
        auto entities = std::vector<std::tuple<Entity, EntityIdentifier, const GuiElementComponent*>>();
        entities.reserve(std::distance(entitiesView.begin(), entitiesView.end()));
        for (auto [entity, identifier, element] : entitiesView) {
            entities.emplace_back(entity, identifier, &element);
        }
        std::sort(entities.begin(), entities.end(), [](const auto &a, const auto &b) {
            return std::get<1>(a).index < std::get<1>(b).index;
        });

        float width = static_cast<float>(window.getWidth()), height = static_cast<float>(window.getHeight());
        glm::mat4 projectionViewMatrix = glm::ortho(0.0f, width, 0.0f, height);

        for (auto [entity, identifier, element] : entities) {
            drawGuiElement(
                this->spriteVertexArray,
                this->guiShader,
                *element,
                projectionViewMatrix,
                element->computedPosition,
                element->computedSize
            );
        }
    }
    // Debug::endTimeMeasure();
}