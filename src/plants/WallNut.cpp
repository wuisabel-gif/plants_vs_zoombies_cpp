#include "plants/WallNut.hpp"

WallNut::WallNut(ResourceManager& resources, int column, int row, sf::Vector2f drawPosition)
    : Plant(column, row, drawPosition),
      healthy(resources, "assets/biology/botany/WallNut", "WallNut_", 16),
      cracked(resources, "assets/biology/botany/Wallnut_cracked1", "Wallnut_cracked1_", 10),
      broken(resources, "assets/biology/botany/Wallnut_cracked2", "Wallnut_cracked2_", 15) {
    health = 30.0f;
}

Plant::Action WallNut::update(float deltaSeconds, bool, std::vector<Projectile>&) {
    healthy.update(deltaSeconds);
    cracked.update(deltaSeconds);
    broken.update(deltaSeconds);
    return {};
}

void WallNut::draw(sf::RenderTarget& target) const {
    if (health > 20.0f) {
        healthy.draw(target, position);
    } else if (health > 10.0f) {
        cracked.draw(target, position);
    } else {
        broken.draw(target, position);
    }
}
