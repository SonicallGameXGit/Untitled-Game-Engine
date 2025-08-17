#pragma once

#include <framework/util/debug.hpp>
#include <framework/graphics/window.hpp>
#include <framework/graphics/shader.hpp>
#include <framework/graphics/buffers.hpp>
#include <framework/graphics/texture.hpp>

#include <engine/world/camera.hpp>
#include <engine/world/mesh.hpp>

#include <engine/graphics/gui/text.hpp>

#include <engine/ecs/ecs.hpp>
#include <engine/ecs/components/transform.hpp>
#include <engine/ecs/components/mesh.hpp>
#include <engine/ecs/components/gui.hpp>

#include <engine/graphics/renderer.hpp>
#include <engine/util/defines.hpp>

enum class Bits {
    DOBRI_BIT = 0b00000001,
    ZLIY_BIT  = 0b00000010, // On Zliy, ne pishite, a to zhopa budit
};

constexpr Bits bit = Bits::DOBRI_BIT;

bool hasCriticalErrors() {
    if (static_cast<int>(bit) & static_cast<int>(Bits::ZLIY_BIT)) {
        SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "ZLIY!!!!");
        return true;
    }

    return false;
}