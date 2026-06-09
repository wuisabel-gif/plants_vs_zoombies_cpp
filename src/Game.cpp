#include "Game.hpp"

#include "plants/Peashooter.hpp"
#include "plants/SunFlower.hpp"
#include "zombies/ZombieBasic.hpp"

#include <algorithm>
#include <array>
#include <cstdlib>

namespace {
constexpr unsigned int windowWidth = 820;
constexpr unsigned int windowHeight = 620;
constexpr std::array<int, 4> levelZombieTotals = {8, 12, 16, 22};
constexpr std::array<float, 4> levelFirstSpawnDelays = {
    6.0f, 5.0f, 4.0f, 3.0f
};
constexpr std::array<float, 4> levelSpawnDelays = {4.0f, 3.4f, 2.8f, 2.2f};
constexpr std::array<int, 6> cardPrices = {50, 100, 50, 150, 175, 200};
constexpr std::array<float, 6> cardCooldownDurations = {
    2.0f, 2.5f, 5.0f, 15.0f, 8.0f, 10.0f
};
constexpr int sunFlowerCard = 0;
constexpr int peashooterCard = 1;
}

Game::Game()
    : window(sf::VideoMode({windowWidth, windowHeight}), "Plants vs. Zombies C++ Port"),
      startButton(resources,
          "assets/ui/start/SelectorScreen_StartAdventure_Button1.png",
          "assets/ui/start/SelectorScreen_StartAdventure_Highlight.png",
          {410.0f, 65.0f}),
      quitButton(resources,
          "assets/ui/start/SelectorScreen_Quit1.png",
          "assets/ui/start/SelectorScreen_Quit2.png",
          {724.0f, 515.0f}),
      gameOverButton(resources,
          "assets/ui/GameOver.png",
          "assets/ui/GameOver_highlight.png",
          {0.0f, 0.0f}) {
    window.setFramerateLimit(60);
}

void Game::run() {
    while (window.isOpen()) {
        const float deltaSeconds = clock.restart().asSeconds();
        handleEvents();
        update(deltaSeconds);
        draw();
    }
}

void Game::handleEvents() {
    while (const auto event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            window.close();
        } else if (const auto* moved = event->getIf<sf::Event::MouseMoved>()) {
            mouse = moved->position;
            startButton.setHover(mouse);
            quitButton.setHover(mouse);
            gameOverButton.setHover(mouse);
            if (scene == Scene::Adventure) {
                for (auto& sunToken : sunTokens) {
                    if (sunToken.contains(mouse)) {
                        sunToken.catchSun();
                    }
                }
            }
        } else if (const auto* pressed = event->getIf<sf::Event::MouseButtonPressed>()) {
            if (pressed->button == sf::Mouse::Button::Left) {
                handleClick(pressed->position);
            }
        } else if (const auto* key = event->getIf<sf::Event::KeyPressed>()) {
            if (key->code == sf::Keyboard::Key::Escape || key->code == sf::Keyboard::Key::Q) {
                window.close();
            }
        }
    }
}

void Game::handleClick(sf::Vector2i point) {
    if (scene == Scene::Start) {
        if (startButton.contains(point)) {
            beginAdventure();
        } else if (quitButton.contains(point)) {
            window.close();
        }
        return;
    }

    if (scene == Scene::PlantsWon) {
        if (currentLevel < maxLevel) {
            beginLevel(currentLevel + 1);
        } else {
            scene = Scene::Start;
        }
        return;
    }

    if (scene == Scene::ZombiesWon) {
        scene = Scene::Start;
        return;
    }

    if (point.y > 0
        && point.y <= 80
        && point.x > 245
        && point.x < 245 + 55 * 6) {
        const int card = (point.x - 245) / 55;
        const auto cardIndex = static_cast<std::size_t>(card);
        if (isCardPorted(card)
            && cardCooldowns[cardIndex] <= 0.0f
            && sun >= cardPrice(card)) {
            selectedCard = card;
        }
        return;
    }

    if (selectedCard != -1 && insideGrid(point)) {
        const sf::Vector2i cell = gridFromPixel(point);
        auto& slot = plants[cell.x][cell.y];
        if (!slot) {
            if (selectedCard == sunFlowerCard) {
                slot = std::make_unique<SunFlower>(
                    resources,
                    cell.x,
                    cell.y,
                    plantPosition(cell.x, cell.y));
            } else if (selectedCard == peashooterCard) {
                slot = std::make_unique<Peashooter>(
                    resources,
                    cell.x,
                    cell.y,
                    plantPosition(cell.x, cell.y));
            }

            sun -= cardPrice(selectedCard);
            const auto cardIndex = static_cast<std::size_t>(selectedCard);
            cardCooldowns[cardIndex] = cardCooldownDurations[cardIndex];
            selectedCard = -1;
        }
    }
}

void Game::update(float deltaSeconds) {
    if (scene != Scene::Adventure) {
        return;
    }

    for (float& cooldown : cardCooldowns) {
        cooldown = std::max(0.0f, cooldown - deltaSeconds);
    }

    spawnTimer += deltaSeconds;
    if (spawnedZombies < firstWaveTotal && spawnTimer >= spawnDelay) {
        spawnZombie();
        spawnTimer = 0.0f;
        spawnDelay = levelSpawnDelays[static_cast<std::size_t>(currentLevel - 1)];
    }

    for (auto& column : plants) {
        for (auto& plant : column) {
            if (plant) {
                const int producedSun = plant->update(
                    deltaSeconds,
                    rowHasZombie(plant->row()),
                    projectiles);
                if (producedSun > 0) {
                    sunTokens.emplace_back(
                        resources,
                        plant->sunSpawnPosition(),
                        plant->sunSpawnPosition());
                }
            }
        }
    }

    for (auto& sunToken : sunTokens) {
        sunToken.update(deltaSeconds);
        if (sunToken.isReadyToCollect()) {
            sun += 25;
        }
    }

    for (auto& projectile : projectiles) {
        projectile.update(deltaSeconds);
        if (!projectile.isFlying()) {
            continue;
        }
        for (auto& zombie : zombies) {
            if (zombie->row() == projectile.row() && zombie->isAlive()
                && projectile.bounds().findIntersection(zombie->bounds()).has_value()) {
                zombie->damage(projectile.damage());
                projectile.hit();
                break;
            }
        }
    }

    for (auto& zombie : zombies) {
        Plant* target = plantHitBy(*zombie);
        zombie->update(deltaSeconds, target);
        if (zombie->position().x < gridLeft && zombie->isAlive()) {
            scene = Scene::ZombiesWon;
        }
    }

    for (auto& column : plants) {
        for (auto& plant : column) {
            if (plant && !plant->isAlive()) {
                plant.reset();
            }
        }
    }

    projectiles.erase(
        std::remove_if(
            projectiles.begin(),
            projectiles.end(),
            [](const Projectile& projectile) {
                return projectile.shouldRemove();
            }),
        projectiles.end());

    sunTokens.erase(
        std::remove_if(
            sunTokens.begin(),
            sunTokens.end(),
            [](const SunToken& sunToken) {
                return sunToken.shouldRemove();
            }),
        sunTokens.end());

    const auto beforeRemovingZombies = zombies.size();
    zombies.erase(
        std::remove_if(
            zombies.begin(),
            zombies.end(),
            [](const std::unique_ptr<Zombie>& zombie) {
                return zombie->shouldRemove();
            }),
        zombies.end());
    defeatedZombies += static_cast<int>(beforeRemovingZombies - zombies.size());

    if (spawnedZombies >= firstWaveTotal && zombies.empty()) {
        scene = Scene::PlantsWon;
    }
}

void Game::draw() {
    window.clear(sf::Color::Black);
    if (scene == Scene::Start) {
        drawStart();
    } else {
        drawAdventure();
    }
    window.display();
}

void Game::drawStart() {
    drawSprite("assets/ui/start/SelectorScreen_BG.jpg", {0.0f, 0.0f});
    drawSprite("assets/ui/start/SelectorScreen_BG_Center.png", {85.0f, 250.0f});
    drawSprite("assets/ui/start/SelectorScreen_BG_Left.png", {0.0f, -80.0f});
    drawSprite("assets/ui/start/SelectorScreen_BG_Right.png", {76.0f, 40.0f});
    drawSprite("assets/ui/start/SelectorScreen_Leaves.png", {0.0f, 538.0f});
    drawSprite("assets/ui/start/SelectorScreen_WoodSign1.png", {23.0f, -5.0f});
    drawSprite("assets/ui/start/SelectorScreen_WoodSign2.png", {23.0f, 130.0f});
    startButton.draw(window);
    drawSprite("assets/ui/start/SelectorScreen_Survival_button.png", {410.0f, 170.0f});
    drawSprite("assets/ui/start/SelectorScreen_Challenges_button.png", {415.0f, 255.0f});
    drawSprite("assets/ui/start/SelectorScreen_Vasebreaker_button.png", {420.0f, 325.0f});
    drawSprite("assets/ui/start/SelectorScreen_Options1.png", {570.0f, 490.0f});
    drawSprite("assets/ui/start/SelectorScreen_Help1.png", {655.0f, 529.0f});
    quitButton.draw(window);
}

void Game::drawAdventure() {
    drawSprite("assets/img/background.jpg", {-200.0f, 0.0f});

    for (const auto& column : plants) {
        for (const auto& plant : column) {
            if (plant) {
                plant->draw(window);
            }
        }
    }

    for (const auto& projectile : projectiles) {
        projectile.draw(window);
    }
    for (const auto& sunToken : sunTokens) {
        sunToken.draw(window);
    }
    for (const auto& zombie : zombies) {
        zombie->draw(window);
    }

    drawSeedBank();

    if (selectedCard != -1 && insideGrid(mouse)) {
        const auto cell = gridFromPixel(mouse);
        if (selectedCard == sunFlowerCard) {
            drawSprite("assets/img/Blurs/SunFlower.png", plantPosition(cell.x, cell.y));
        } else if (selectedCard == peashooterCard) {
            drawSprite("assets/img/Blurs/Peashooter.png", plantPosition(cell.x, cell.y));
        }
    }

    if (scene == Scene::PlantsWon) {
        sf::RectangleShape panel({420.0f, 150.0f});
        panel.setPosition({200.0f, 220.0f});
        panel.setFillColor(sf::Color(242, 226, 155, 235));
        panel.setOutlineThickness(4.0f);
        panel.setOutlineColor(sf::Color(88, 112, 55));
        window.draw(panel);
        if (currentLevel < maxLevel) {
            drawText(
                "Level " + std::to_string(currentLevel) + " Complete!",
                {270.0f, 256.0f},
                32,
                sf::Color(42, 82, 38));
            drawText(
                "Click to start Level " + std::to_string(currentLevel + 1),
                {300.0f, 314.0f},
                22,
                sf::Color(64, 64, 44));
        } else {
            drawText(
                "All Levels Complete!",
                {260.0f, 256.0f},
                32,
                sf::Color(42, 82, 38));
            drawText(
                "Click to return to the menu",
                {278.0f, 314.0f},
                22,
                sf::Color(64, 64, 44));
        }
    }

    if (scene == Scene::ZombiesWon) {
        drawSprite("assets/img/ZombiesWon.png", {250.0f, 160.0f});
        drawSprite("assets/ui/GameOverPanel.png", {232.0f, 160.0f});
        drawSprite("assets/ui/GameOver.png", {280.0f, 335.0f});
    }
}

void Game::drawSeedBank() {
    drawSprite("assets/ui/SeedBank.png", {170.0f, 0.0f});
    drawSprite("assets/ui/ShovelBank.png", {170.0f + 446.0f, 0.0f});
    drawSprite("assets/ui/Shovel.png", {170.0f + 446.0f, 0.0f});
    drawCards();
    drawText(std::to_string(sun), {190.0f, 73.0f}, 18, sf::Color::Black);
    drawText(
        "Level " + std::to_string(currentLevel),
        {640.0f, 555.0f},
        27,
        sf::Color::Black);
    drawText(
        "Level " + std::to_string(currentLevel),
        {650.0f, 550.0f},
        25,
        sf::Color::White);
    drawText(
        std::to_string(defeatedZombies) + "/" + std::to_string(firstWaveTotal),
        {720.0f, 585.0f},
        18,
        sf::Color::White);
}

void Game::drawCards() {
    static const std::array<const char*, 6> readyCards = {
        "assets/ui/Cards/SunFlower0.png",
        "assets/ui/Cards/Peashooter0.png",
        "assets/ui/Cards/WallNut0.png",
        "assets/ui/Cards/CherryBomb0.png",
        "assets/ui/Cards/SnowPea0.png",
        "assets/ui/Cards/Repeater0.png"
    };
    static const std::array<const char*, 6> blockedCards = {
        "assets/ui/Cards/SunFlower1.png",
        "assets/ui/Cards/Peashooter1.png",
        "assets/ui/Cards/WallNut1.png",
        "assets/ui/Cards/CherryBomb1.png",
        "assets/ui/Cards/SnowPea1.png",
        "assets/ui/Cards/Repeater1.png"
    };

    for (int i = 0; i < 6; ++i) {
        const auto cardIndex = static_cast<std::size_t>(i);
        const bool usable = isCardPorted(i)
            && cardCooldowns[cardIndex] <= 0.0f
            && sun >= cardPrices[cardIndex];
        drawSprite(
            usable ? readyCards[i] : blockedCards[i],
            {245.0f + i * 55.0f, 5.0f});
    }
}

void Game::drawText(
    const sf::String& text,
    sf::Vector2f position,
    unsigned int size,
    sf::Color color) {
    const sf::Font* font = resources.font();
    if (!font) {
        return;
    }
    sf::Text label(*font, text, size);
    label.setPosition(position);
    label.setFillColor(color);
    window.draw(label);
}

void Game::drawSprite(const std::string& path, sf::Vector2f position) {
    const sf::Texture* texture = resources.texture(path);
    if (!texture) {
        return;
    }
    sf::Sprite sprite(*texture);
    sprite.setPosition(position);
    window.draw(sprite);
}

void Game::beginAdventure() {
    beginLevel(1);
}

void Game::beginLevel(int level) {
    for (auto& column : plants) {
        for (auto& plant : column) {
            plant.reset();
        }
    }
    projectiles.clear();
    sunTokens.clear();
    zombies.clear();
    currentLevel = std::clamp(level, 1, maxLevel);
    sun = 500 + (currentLevel - 1) * 75;
    selectedCard = -1;
    for (std::size_t i = 0; i < cardCooldowns.size(); ++i) {
        cardCooldowns[i] = cardCooldownDurations[i];
    }
    group = currentLevel;
    spawnedZombies = 0;
    defeatedZombies = 0;
    const auto levelIndex = static_cast<std::size_t>(currentLevel - 1);
    firstWaveTotal = levelZombieTotals[levelIndex];
    spawnDelay = levelFirstSpawnDelays[levelIndex];
    spawnTimer = 0.0f;
    scene = Scene::Adventure;
}

bool Game::insideGrid(sf::Vector2i point) const {
    return point.x >= gridLeft && point.y >= gridTop
        && point.x < gridLeft + columns * cellWidth
        && point.y < gridTop + rows * cellHeight;
}

sf::Vector2i Game::gridFromPixel(sf::Vector2i point) const {
    return {
        static_cast<int>((point.x - gridLeft) / cellWidth),
        static_cast<int>((point.y - gridTop) / cellHeight)
    };
}

sf::Vector2f Game::plantPosition(int column, int row) const {
    return {gridLeft + column * cellWidth, gridTop + row * cellHeight};
}

bool Game::rowHasZombie(int row) const {
    return std::any_of(
        zombies.begin(),
        zombies.end(),
        [row](const std::unique_ptr<Zombie>& zombie) {
            return zombie->row() == row && zombie->isAlive();
        });
}

Plant* Game::plantHitBy(Zombie& zombie) {
    const int row = zombie.row();
    for (auto& column : plants) {
        auto& plant = column[row];
        if (plant && plant->bounds().findIntersection(zombie.bounds()).has_value()) {
            return plant.get();
        }
    }
    return nullptr;
}

void Game::spawnZombie() {
    const int row = std::rand() % rows;
    zombies.push_back(std::make_unique<ZombieBasic>(resources, row));
    ++spawnedZombies;
}

bool Game::isCardPorted(int card) const {
    return card == sunFlowerCard || card == peashooterCard;
}

int Game::cardPrice(int card) const {
    return cardPrices[static_cast<std::size_t>(card)];
}