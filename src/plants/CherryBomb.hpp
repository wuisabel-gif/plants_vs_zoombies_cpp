#pragma once

#include "plants/Plant.hpp"

class CherryBomb : public Plant {
public:
    CherryBomb(ResourceManager& resources, int column, int row, sf::Vector2f drawPosition);

    Action update(float deltaSeconds, bool shouldFire, std::vector<Projectile>& projectiles) override;
    void draw(sf::RenderTarget& target) const override;

private:
    const sf::Texture* boomTexture = nullptr;
    Animation fuse;
    bool exploded = false;
    float removeTimer = 0.0f;
};
