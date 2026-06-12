#pragma once

#include "zombies/Zombie.hpp"

class ZombieConehead : public Zombie {
public:
    ZombieConehead(ResourceManager& resources, int row);

    void update(float deltaSeconds, Plant* target) override;
    void draw(sf::RenderTarget& target) const override;
    void setDie() override;

private:
    Animation walk;
    Animation eat;
    Animation dieBody;
    Animation dieHead;
};
