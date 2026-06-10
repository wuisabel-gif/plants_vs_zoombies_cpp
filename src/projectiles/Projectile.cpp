#include "projectiles/Projectile.hpp"

Projectile::Projectile(ResourceManager& resources, int row, sf::Vector2f start)
    : peaTexture(resources.texture("assets/biology/botany/bullet/ProjectilePea.png")),
      hitTexture(resources.texture("assets/biology/botany/bullet/BulletHit.png")),
      lane(row),
      position(start) {}

void Projectile::update(float deltaSeconds) {
    if (flying) {
        position.x += 600.0f * deltaSeconds;
    } else {
        hitTimer += deltaSeconds;
    }
}

void Projectile::draw(sf::RenderTarget& target) const {
    const sf::Texture* texture = flying ? peaTexture : hitTexture;
    if (texture) {
        sf::Sprite sprite(*texture);
        const sf::Vector2u size = texture->getSize();
        sprite.setPosition({position.x - size.x * 0.5f, position.y - size.y * 0.5f});
        target.draw(sprite);
        return;
    }

    sf::CircleShape fallback(8.0f);
    fallback.setOrigin({8.0f, 8.0f});
    fallback.setPosition(position);
    fallback.setFillColor(sf::Color(120, 220, 70));
    target.draw(fallback);
}

void Projectile::hit() {
    flying = false;
    hitTimer = 0.0f;
}

bool Projectile::shouldRemove() const {
    return position.x > 840.0f || (!flying && hitTimer >= 0.10f);
}

bool Projectile::isFlying() const {
    return flying;
}

int Projectile::row() const {
    return lane;
}

int Projectile::damage() const {
    return 1;
}

sf::FloatRect Projectile::bounds() const {
    return {{position.x - 10.0f, position.y - 10.0f}, {20.0f, 20.0f}};
}
