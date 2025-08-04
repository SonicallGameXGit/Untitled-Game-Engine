#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <GL/glew.h>

#include "framework/util/debug.hpp"
#include "framework/graphics/window.hpp"
#include "framework/graphics/shader.hpp"
#include "framework/graphics/mesh.hpp"
#include "framework/graphics/texture.hpp"
#include "gui/text.hpp"

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