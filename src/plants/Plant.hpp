#pragma once

#include "projectiles/Projectile.hpp"
#include "resources/Animation.hpp"

#include <SFML/Graphics.hpp>
#include <vector>

class Plant {
public:
    struct Action {
        int sun = 0;
        bool removeSelf = false;
        bool explode = false;
        sf::Vector2f explosionCenter = {};
        float explosionRadius = 0.0f;
    };

    Plant(int column, int row, sf::Vector2f drawPosition);
    virtual ~Plant() = default;

    virtual Action update(float deltaSeconds, bool shouldFire, std::vector<Projectile>& projectiles) = 0;
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
