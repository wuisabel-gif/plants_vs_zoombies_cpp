#pragma once

#include "resources/ResourceManager.hpp"

#include <SFML/Graphics.hpp>

class SunToken {
public:
    SunToken(ResourceManager& resources, sf::Vector2f position, sf::Vector2f target);

    void update(float deltaSeconds);
    void draw(sf::RenderTarget& target) const;
    void catchSun();

    bool contains(sf::Vector2i point) const;
    bool isReadyToCollect() const;
    bool shouldRemove() const;

private:
    const sf::Texture* sun1 = nullptr;
    const sf::Texture* sun2 = nullptr;
    const sf::Texture* sun3 = nullptr;
    sf::Vector2f position;
    sf::Vector2f target;
    bool caught = false;
    bool collected = false;
    float age = 0.0f;
};
