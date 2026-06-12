#include "plants/SunFlower.hpp"

SunFlower::SunFlower(ResourceManager& resources, int column, int row, sf::Vector2f drawPosition)
    : Plant(column, row, drawPosition),
      idle(resources, "assets/biology/botany/SunFlower", "SunFlower_", 18) {
    health = 5.0f;
}

Plant::Action SunFlower::update(float deltaSeconds, bool, std::vector<Projectile>&) {
    idle.update(deltaSeconds);
    sunTimer += deltaSeconds;
    if (sunTimer >= 10.0f) {
        sunTimer = 0.0f;
        Action action;
        action.sun = 25;
        return action;
    }
    return {};
}

void SunFlower::draw(sf::RenderTarget& target) const {
    idle.draw(target, position);
}
