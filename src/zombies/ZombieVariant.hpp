#pragma once

#include "zombies/Zombie.hpp"

class ZombieVariant : public Zombie {
public:
    enum class Kind {
        Fast,
        Tough
    };

    ZombieVariant(ResourceManager& resources, int row, Kind kind);

    void update(float deltaSeconds, Plant* target) override;
    void draw(sf::RenderTarget& target) const override;
    void setDie() override;

private:
    Animation walk;
    Animation eat;
    Animation dieBody;
    Animation dieHead;
    Kind kind;
};
