#pragma once

#include "plants/Plant.hpp"

class SunFlower : public Plant {
public:
    SunFlower(ResourceManager& resources, int column, int row, sf::Vector2f drawPosition);

    int update(float deltaSeconds, bool shouldFire, std::vector<Projectile>& projectiles) override;
    void draw(sf::RenderTarget& target) const override;

private:
    Animation idle;
    float sunTimer = 0.0f;
};
