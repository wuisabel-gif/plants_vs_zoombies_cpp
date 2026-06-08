#include "resources/ResourceManager.hpp"

#include <array>
#include <filesystem>

const sf::Texture* ResourceManager::texture(const std::string& path) {
    auto found = textures.find(path);
    if (found != textures.end()) {
        return &found->second;
    }

    sf::Texture loaded;
    if (!loaded.loadFromFile(path)) {
        return nullptr;
    }

    auto [inserted, _] = textures.emplace(path, std::move(loaded));
    return &inserted->second;
}

const sf::Font* ResourceManager::font() {
    if (!triedFont) {
        triedFont = true;
        const std::array<const char*, 4> paths = {
            "/System/Library/Fonts/Supplemental/Arial.ttf",
            "/System/Library/Fonts/Helvetica.ttc",
            "/Library/Fonts/Arial.ttf",
            "assets/fonts/game.ttf"
        };

        for (const char* path : paths) {
            if (std::filesystem::exists(path) && gameFont.openFromFile(path)) {
                hasFont = true;
                break;
            }
        }
    }

    return hasFont ? &gameFont : nullptr;
}
