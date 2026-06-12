#pragma once

#include "plants/Plant.hpp"

class Peashooter : public Plant {
public:
    Peashooter(ResourceManager& resources, int column, int row, sf::Vector2f drawPosition);

    Action update(float deltaSeconds, bool shouldFire, std::vector<Projectile>& projectiles) override;
    void draw(sf::RenderTarget& target) const override;

private:
    ResourceManager& resources;
    Animation idle;
    float fireTimer = 0.0f;
};
