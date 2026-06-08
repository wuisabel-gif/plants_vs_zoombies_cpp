#pragma once

#include "resources/ResourceManager.hpp"

#include <SFML/Graphics.hpp>
#include <string>
#include <vector>

class Animation {
public:
    Animation() = default;
    Animation(ResourceManager& resources, const std::string& folder, const std::string& prefix, int frames);

    void update(float deltaSeconds);
    void draw(sf::RenderTarget& target, sf::Vector2f position) const;
    void playOnce();
    void setFrameSeconds(float seconds);
    bool isFinished() const;
    sf::Vector2u size() const;

private:
    std::vector<const sf::Texture*> textures;
    float frameSeconds = 0.07f;
    float elapsed = 0.0f;
    int current = 0;
    bool once = false;
    bool finished = false;
};
