#pragma once

#include "plants/Plant.hpp"
#include "projectiles/Projectile.hpp"
#include "resources/ResourceManager.hpp"
#include "ui/Button.hpp"
#include "ui/SunToken.hpp"
#include "zombies/Zombie.hpp"

#include <SFML/Graphics.hpp>
#include <array>
#include <memory>
#include <vector>

class Game {
public:
    Game();
    void run();

private:
    enum class Scene {
        Start,
        Adventure,
        PlantsWon,
        ZombiesWon
    };

   
    void handleEvents();
    void handleClick(sf::Vector2i point);
    void update(float deltaSeconds);
    void draw();

    void drawStart();
    void drawAdventure();
    void drawSeedBank();
    void drawCards();
    void drawText(
        const sf::String& text,
        sf::Vector2f position,
        unsigned int size,
        sf::Color color);
    void drawSprite(const std::string& path, sf::Vector2f position);

    void beginAdventure();
    void beginLevel(int level);
    bool insideGrid(sf::Vector2i point) const;
    sf::Vector2i gridFromPixel(sf::Vector2i point) const;
    sf::Vector2f plantPosition(int column, int row) const;
    bool rowHasZombie(int row) const;
    Plant* plantHitBy(Zombie& zombie);
    