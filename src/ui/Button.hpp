#pragma once

#include "resources/ResourceManager.hpp"

#include <SFML/Graphics.hpp>
#include <string>

class Button {
public:
    Button() = default;
    Button(
        ResourceManager& resources,
        const std::string& normalPath,
        const std::string& hoverPath,
        sf::Vector2f position);

    void setPosition(sf::Vector2f newPosition);
    bool contains(sf::Vector2i point) const;
    void setHover(sf::Vector2i point);
    void draw(sf::RenderTarget& target) const;

private:
    const sf::Texture* normal = nullptr;
    const sf::Texture* hover = nullptr;
    sf::Vector2f position;
    bool hovered = false;
};
