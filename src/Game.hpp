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

    static constexpr int columns = 9;
    static constexpr int rows = 5;
    static constexpr float gridLeft = 50.0f;
    static constexpr float gridTop = 80.0f;
    static constexpr float cellWidth = 81.0f;
    static constexpr float cellHeight = 99.0f;

    using PlantGrid = std::array<std::array<std::unique_ptr<Plant>, rows>, columns>;

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
    void spawnZombie();
    bool isCardPorted(int card) const;
    int cardPrice(int card) const;

    sf::RenderWindow window;
    ResourceManager resources;
    Scene scene = Scene::Start;
    sf::Clock clock;
    sf::Vector2i mouse = {0, 0};

    Button startButton;
    Button quitButton;
    Button gameOverButton;
    PlantGrid plants;
    std::vector<Projectile> projectiles;
    std::vector<SunToken> sunTokens;
    std::vector<std::unique_ptr<Zombie>> zombies;

    int sun = 500;
    int selectedCard = -1;
    std::array<float, 6> cardCooldowns = {};
    int currentLevel = 1;
    int maxLevel = 4;
    int group = 1;
    int spawnedZombies = 0;
    int defeatedZombies = 0;
    int firstWaveTotal = 8;
    float spawnDelay = 10.0f;
    float spawnTimer = 0.0f;
};
