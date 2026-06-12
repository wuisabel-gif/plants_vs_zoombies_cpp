#pragma once

#include "resources/ResourceManager.hpp"

#include <SFML/Graphics.hpp>

class Projectile {
public:
    enum class Type {
        Pea,
        Snow
    };

    Projectile(ResourceManager& resources, int row, sf::Vector2f position, Type type = Type::Pea);

    void update(float deltaSeconds);
    void draw(sf::RenderTarget& target) const;
    void hit();

    bool shouldRemove() const;
    bool isFlying() const;
    bool slowsTarget() const;
    int row() const;
    int damage() const;
    sf::FloatRect bounds() const;

private:
    const sf::Texture* peaTexture = nullptr;
    const sf::Texture* snowTexture = nullptr;
    const sf::Texture* hitTexture = nullptr;
    int lane = 0;
    sf::Vector2f position;
    Type type = Type::Pea;
    float hitTimer = 0.0f;
    bool flying = true;
};
