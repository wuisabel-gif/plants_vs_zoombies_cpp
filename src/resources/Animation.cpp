#include "resources/Animation.hpp"

Animation::Animation(
    ResourceManager& resources,
    const std::string& folder,
    const std::string& prefix,
    int frames) {
    textures.reserve(static_cast<std::size_t>(frames));
    for (int i = 0; i < frames; ++i) {
        textures.push_back(resources.texture(folder + "/" + prefix + std::to_string(i) + ".png"));
    }
}

void Animation::update(float deltaSeconds) {
    if (textures.empty() || finished) {
        return;
    }

    elapsed += deltaSeconds;
    if (elapsed < frameSeconds) {
        return;
    }

    elapsed = 0.0f;
    ++current;
    if (current >= static_cast<int>(textures.size())) {
        if (once) {
            current = static_cast<int>(textures.size()) - 1;
            finished = true;
        } else {
            current = 0;
        }
    }
}

void Animation::draw(sf::RenderTarget& target, sf::Vector2f position) const {
    if (textures.empty() || !textures[static_cast<std::size_t>(current)]) {
        return;
    }

    sf::Sprite sprite(*textures[static_cast<std::size_t>(current)]);
    sprite.setPosition(position);
    target.draw(sprite);
}

void Animation::playOnce() {
    once = true;
    finished = false;
    current = 0;
    elapsed = 0.0f;
}

void Animation::setFrameSeconds(float seconds) {
    frameSeconds = seconds;
}

bool Animation::isFinished() const {
    return finished;
}

sf::Vector2u Animation::size() const {
    for (const sf::Texture* texture : textures) {
        if (texture) {
            return texture->getSize();
        }
    }
    return {0, 0};
}
