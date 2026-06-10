#include "ui/SunToken.hpp"

#include <cmath>

namespace {
float length(sf::Vector2f vector) {
    return std::sqrt(vector.x * vector.x + vector.y * vector.y);
}
}

SunToken::SunToken(ResourceManager& resources, sf::Vector2f start, sf::Vector2f end)
    : sun1(resources.texture("assets/img/Sun1.png")),
      sun2(resources.texture("assets/img/Sun2.png")),
      sun3(resources.texture("assets/img/Sun3.png")),
      position(start),
      target(end) {}

void SunToken::update(float deltaSeconds) {
    age += deltaSeconds;

    if (caught) {
        const sf::Vector2f delta = target - position;
        const float distance = length(delta);
        if (distance <= 20.0f) {
            collected = true;
            return;
        }

        const sf::Vector2f direction = delta / distance;
        position += direction * 800.0f * deltaSeconds;
    } else {
        const sf::Vector2f delta = target - position;
        if (delta.y > 0.0f) {
            position.y += 100.0f * deltaSeconds;
        }
        if (delta.x > 0.0f) {
            position.x += 100.0f * deltaSeconds;
        }
    }
}

void SunToken::draw(sf::RenderTarget& targetSurface) const {
    if (sun3) {
        sf::Sprite sprite(*sun3);
        sprite.setPosition(position);
        targetSurface.draw(sprite);
    }
    if (sun2) {
        sf::Sprite sprite(*sun2);
        sprite.setPosition(position + sf::Vector2f(20.0f, 20.0f));
        targetSurface.draw(sprite);
    }
    if (sun1) {
        sf::Sprite sprite(*sun1);
        sprite.setPosition(position + sf::Vector2f(40.0f, 40.0f));
        targetSurface.draw(sprite);
    }
}

void SunToken::catchSun() {
    caught = true;
    target = {170.0f, 0.0f};
}

bool SunToken::contains(sf::Vector2i point) const {
    return point.x > position.x && point.x < position.x + 117.0f
        && point.y > position.y && point.y < position.y + 117.0f;
}

bool SunToken::isReadyToCollect() const {
    return collected;
}

bool SunToken::shouldRemove() const {
    return collected || (!caught && age > 3.0f);
}
