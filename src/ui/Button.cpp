#include "ui/Button.hpp"

Button::Button(
    ResourceManager& resources,
    const std::string& normalPath,
    const std::string& hoverPath,
    sf::Vector2f start)
    : normal(resources.texture(normalPath)),
      hover(resources.texture(hoverPath)),
      position(start) {}

void Button::setPosition(sf::Vector2f newPosition) {
    position = newPosition;
}

bool Button::contains(sf::Vector2i point) const {
    const sf::Texture* texture = normal ? normal : hover;
    if (!texture) {
        return false;
    }
    const sf::Vector2u size = texture->getSize();
    return point.x >= position.x && point.y >= position.y
        && point.x <= position.x + static_cast<float>(size.x)
        && point.y <= position.y + static_cast<float>(size.y);
}

void Button::setHover(sf::Vector2i point) {
    hovered = contains(point);
}

void Button::draw(sf::RenderTarget& target) const {
    const sf::Texture* texture = hovered && hover ? hover : normal;
    if (!texture) {
        return;
    }
    sf::Sprite sprite(*texture);
    sprite.setPosition(position);
    target.draw(sprite);
}
