#pragma once

#include "plants/Plant.hpp"

class WallNut : public Plant {
public:
    WallNut(ResourceManager& resources, int column, int row, sf::Vector2f drawPosition);

    Action update(float deltaSeconds, bool shouldFire, std::vector<Projectile>& projectiles) override;
    void draw(sf::RenderTarget& target) const override;

private:
    Animation healthy;
    Animation cracked;
    Animation broken;
};
