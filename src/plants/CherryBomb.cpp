#include "plants/CherryBomb.hpp"

CherryBomb::CherryBomb(ResourceManager& resources, int column, int row, sf::Vector2f drawPosition)
    : Plant(column, row, drawPosition),
      boomTexture(resources.texture("assets/img/Boom.png")),
      fuse(resources, "assets/biology/botany/CherryBomb", "CherryBomb_", 7) {
    health = 5.0f;
    fuse.playOnce();
}

Plant::Action CherryBomb::update(float deltaSeconds, bool, std::vector<Projectile>&) {
    fuse.update(deltaSeconds);
    if (!fuse.isFinished()) {
        return {};
    }

    if (!exploded) {
        exploded = true;
        Action action;
        action.explode = true;
        action.explosionCenter = position;
        action.explosionRadius = 300.0f;
        return action;
    }

    removeTimer += deltaSeconds;
    Action action;
    action.removeSelf = removeTimer >= 0.2f;
    return action;
}

void CherryBomb::draw(sf::RenderTarget& target) const {
    fuse.draw(target, position);
    if (!exploded || !boomTexture) {
        return;
    }

    sf::Sprite boom(*boomTexture);
    const sf::Vector2u size = boomTexture->getSize();
    boom.setPosition({
        position.x - static_cast<float>(size.x) / 3.0f,
        position.y - static_cast<float>(size.y) / 3.0f
    });
    target.draw(boom);
}
