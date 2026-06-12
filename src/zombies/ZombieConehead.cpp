#include "zombies/ZombieConehead.hpp"

#include <algorithm>

namespace {
constexpr float gridLeft = 50.0f;
constexpr float gridTop = 80.0f;
constexpr float cellWidth = 81.0f;
constexpr float cellHeight = 99.0f;
}

ZombieConehead::ZombieConehead(ResourceManager& resources, int row)
    : Zombie(row, {gridLeft + cellWidth * 9.0f, gridTop + cellHeight * row}),
      walk(resources, "assets/biology/zombie/Zombie4", "Frame", 21),
      eat(resources, "assets/biology/zombie/ConeheadZombieAttack", "Frame", 11),
      dieBody(resources, "assets/biology/zombie/ZombieDie", "Frame", 14),
      dieHead(resources, "assets/biology/zombie/ZombieHead", "Frame", 11) {
    blood = 14;
}

void ZombieConehead::update(float deltaSeconds, Plant* target) {
    slowTimer = std::max(0.0f, slowTimer - deltaSeconds);

    if (state == State::Walking) {
        walk.setFrameSeconds(slowTimer > 0.0f ? 0.15f : 0.07f);
        walk.update(deltaSeconds);
        pos.x -= movementSpeed() * deltaSeconds;
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

void ZombieConehead::draw(sf::RenderTarget& target) const {
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

void ZombieConehead::setDie() {
    state = State::Dying;
    dieHead.playOnce();
    dieBody.playOnce();
}
