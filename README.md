# Untitled-Game-Engine
TODO: Find a name for the engine

For those who build it on Windows:
 - pls, use GCC (MinGW) to render uncommon symbols and just make the app faster and lighter than using MSVC or similar.
 - the compiler will probably fail on trying to find the freetype library that doesn't exist on Windows, so use vcpkg to install it and don't forget to specify -DCMAKE_TOOLCHAIN_FILE