#include "zombies/Zombie.hpp"

Zombie::Zombie(int row, sf::Vector2f position)
    : lane(row), pos(position) {}

void Zombie::damage(int amount) {
    blood -= amount;
    if (blood <= 0 && state != State::Dying && state != State::Removed) {
        setDie();
    }
}

bool Zombie::isAlive() const {
    return blood > 0 && state != State::Dying && state != State::Removed;
}

bool Zombie::shouldRemove() const {
    return state == State::Removed;
}

int Zombie::row() const {
    return lane;
}

sf::Vector2f Zombie::position() const {
    return pos;
}

sf::FloatRect Zombie::bounds() const {
    return {{pos.x, pos.y}, {81.0f, 99.0f}};
}
