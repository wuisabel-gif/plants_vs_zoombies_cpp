#include "zombies/ZombieBasic.hpp"

namespace {
constexpr float gridLeft = 50.0f;
constexpr float gridTop = 80.0f;
constexpr float cellWidth = 81.0f;
constexpr float cellHeight = 99.0f;
}

ZombieBasic::ZombieBasic(ResourceManager& resources, int row)
    : Zombie(row, {gridLeft + cellWidth * 9.0f, gridTop + cellHeight * row}),
      walk(resources, "assets/biology/zombie/Zombie1", "Frame", 22),
      eat(resources, "assets/biology/zombie/ZombieAttack", "Frame", 20),
      dieBody(resources, "assets/biology/zombie/ZombieDie", "Frame", 14),
      dieHead(resources, "assets/biology/zombie/ZombieHead", "Frame", 11) {
    blood = 7;
}

void ZombieBasic::update(float deltaSeconds, Plant* target) {
    if (state == State::Walking) {
        walk.update(deltaSeconds);
        pos.x -= speed * deltaSeconds;
        if (target) {
            state = State::Eating;
        }
    } else if (state == State::Eating) {
        eat.update(deltaSeconds);
        if (target) {
            target->damage(attack * deltaSeconds);
        } else {
            state = State::Walking;
        }
    } else if (state == State::Dying) {
        dieBody.update(deltaSeconds);
        dieHead.update(deltaSeconds);
        if (dieBody.isFinished()) {
            state = State::Removed;
        }
    }
}

void ZombieBasic::draw(sf::RenderTarget& target) const {
    const sf::Vector2f drawPos(pos.x - 75.0f, pos.y - 60.0f);
    if (state == State::Walking) {
        walk.draw(target, drawPos);
    } else if (state == State::Eating) {
        eat.draw(target, drawPos);
    } else if (state == State::Dying) {
        dieHead.draw(target, drawPos);
        dieBody.draw(target, drawPos);
    }

    sf::CircleShape shadow(35.0f);
    shadow.setScale({1.0f, 0.28f});
    shadow.setPosition({pos.x + 10.0f, pos.y + 65.0f});
    shadow.setFillColor(sf::Color(0, 0, 0, 90));
    target.draw(shadow);
}

void ZombieBasic::setDie() {
    state = State::Dying;
    dieHead.playOnce();
    dieBody.playOnce();
}
