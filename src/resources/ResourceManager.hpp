#pragma once

#include <SFML/Graphics.hpp>
#include <map>
#include <string>

class ResourceManager {
public:
    const sf::Texture* texture(const std::string& path);
    const sf::Font* font();

private:
    std::map<std::string, sf::Texture> textures;
    sf::Font gameFont;
    bool triedFont = false;
    bool hasFont = false;
};
