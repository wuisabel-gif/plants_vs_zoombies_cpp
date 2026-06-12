#include "plants/Repeater.hpp"

Repeater::Repeater(ResourceManager& resourceManager, int column, int row, sf::Vector2f drawPosition)
    : Plant(column, row, drawPosition),
      resources(resourceManager),
      idle(resourceManager, "assets/biology/botany/Repeater", "Repeater_", 15) {
    health = 5.0f;
}

Plant::Action Repeater::update(float deltaSeconds, bool shouldFire, std::vector<Projectile>& projectiles) {
    idle.update(deltaSeconds);
    fireTimer += deltaSeconds;
    if (shouldFire && fireTimer >= 2.0f) {
        projectiles.emplace_back(resources, gridRow, sf::Vector2f(position.x + 100.0f, position.y + 25.0f));
        projectiles.emplace_back(resources, gridRow, sf::Vector2f(position.x + 50.0f, position.y + 25.0f));
        fireTimer = 0.0f;
    }
    return {};
}

void Repeater::draw(sf::RenderTarget& target) const {
    idle.draw(target, position);
}
