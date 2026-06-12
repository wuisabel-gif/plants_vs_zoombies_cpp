#pragma once

#include "plants/Plant.hpp"
#include "resources/Animation.hpp"

#include <SFML/Graphics.hpp>

class Zombie {
public:
    Zombie(int row, sf::Vector2f position);
    virtual ~Zombie() = default;

    virtual void update(float deltaSeconds, Plant* target) = 0;
    virtual void draw(sf::RenderTarget& target) const = 0;
    virtual void setDie() = 0;

    void damage(int amount);
    void destroy();
    void applySlow();
    float movementSpeed() const;
    bool isAlive() const;
    bool shouldRemove() const;
    int row() const;
    sf::Vector2f position() const;
    sf::FloatRect bounds() const;

protected:
    enum class State {
        Walking,
        Eating,
        Dying,
        Removed
    };

    int lane = 0;
    sf::Vector2f pos;
    int blood = 7;
    float speed = 20.0f;
    float attack = 1.2f;
    float slowTimer = 0.0f;
    State state = State::Walking;
};
