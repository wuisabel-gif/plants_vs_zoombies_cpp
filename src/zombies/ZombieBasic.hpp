#pragma once

#include "zombies/Zombie.hpp"

class ZombieBasic : public Zombie {
public:
    ZombieBasic(ResourceManager& resources, int row);

    void update(float deltaSeconds, Plant* target) override;
    void draw(sf::RenderTarget& target) const override;
    void setDie() override;

private:
    Animation walk;
    Animation eat;
    Animation dieBody;
    Animation dieHead;
};
