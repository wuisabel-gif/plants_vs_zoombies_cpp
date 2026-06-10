#pragma once

#include "projectiles/Projectile.hpp"
#include "resources/Animation.hpp"

#include <SFML/Graphics.hpp>
#include <vector>

class Plant {
public:
    Plant(int column, int row, sf::Vector2f drawPosition);
    virtual ~Plant() = default;

    virtual int update(float deltaSeconds, bool shouldFire, std::vector<Projectile>& projectiles) = 0;
    virtual void draw(sf::RenderTarget& target) const = 0;

    void damage(float amount);
    bool isAlive() const;
    int row() const;
    int column() const;
    sf::Vector2f sunSpawnPosition() const;
    sf::FloatRect bounds() const;

protected:
    int gridColumn;
    int gridRow;
    sf::Vector2f position;
    float health = 5.0f;
};
