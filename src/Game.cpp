#include "Game.hpp"

#include "plants/CherryBomb.hpp"
#include "plants/Peashooter.hpp"
#include "plants/Repeater.hpp"
#include "plants/SnowPea.hpp"
#include "plants/SunFlower.hpp"
#include "plants/WallNut.hpp"
#include "zombies/ZombieBasic.hpp"
#include "zombies/ZombieConehead.hpp"
#include "zombies/ZombieVariant.hpp"

#include <algorithm>
#include <array>
#include <cctype>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <filesystem>
#include <fstream>

namespace {
constexpr unsigned int windowWidth = 820;
constexpr unsigned int windowHeight = 620;
constexpr std::array<int, 4> levelZombieTotals = {8, 11, 15, 20};
constexpr std::array<float, 4> levelFirstSpawnDelays = {
    7.0f, 9.0f, 6.0f, 4.5f
};
constexpr std::array<float, 4> levelSpawnDelays = {4.4f, 5.0f, 3.6f, 3.0f};
constexpr std::array<int, 6> cardPrices = {50, 100, 50, 150, 175, 200};
constexpr std::array<float, 6> cardCooldownDurations = {
    2.0f, 2.5f, 5.0f, 15.0f, 8.0f, 10.0f
};
constexpr int sunFlowerCard = 0;
constexpr int peashooterCard = 1;
constexpr int wallNutCard = 2;
constexpr int cherryBombCard = 3;
constexpr int snowPeaCard = 4;
constexpr int repeaterCard = 5;
constexpr const char* progressPath = "save_progress.txt";

float distance(sf::Vector2f left, sf::Vector2f right) {
    const sf::Vector2f delta = left - right;
    return std::sqrt(delta.x * delta.x + delta.y * delta.y);
}

bool isSupportedMusicFile(const std::filesystem::path& path) {
    std::string extension = path.extension().string();
    std::transform(extension.begin(), extension.end(), extension.begin(), [](unsigned char c) {
        return static_cast<char>(std::tolower(c));
    });
    return extension == ".ogg"
        || extension == ".wav"
        || extension == ".flac"
        || extension == ".aiff"
        || extension == ".mp3";
}
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
          {280.0f, 335.0f}) {
    window.setFramerateLimit(60);
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
    loadProgress();
    loadMusic();
    updateMusicPlayback();
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
        } else if (const auto* pressed = event->getIf<sf::Event::MouseButtonPressed>()) {
            if (pressed->button == sf::Mouse::Button::Left) {
                handleClick(pressed->position);
            }
        } else if (const auto* key = event->getIf<sf::Event::KeyPressed>()) {
            if (key->code == sf::Keyboard::Key::Escape || key->code == sf::Keyboard::Key::Q) {
                window.close();
            } else if (key->code == sf::Keyboard::Key::M) {
                musicEnabled = !musicEnabled;
                updateMusicPlayback();
            } else if (key->code == sf::Keyboard::Key::N) {
                soundEffectsEnabled = !soundEffectsEnabled;
            } else if (key->code == sf::Keyboard::Key::R && scene == Scene::Start) {
                resetProgress();
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
        if (gameOverButton.contains(point)) {
            scene = Scene::Start;
        }
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
            } else if (selectedCard == wallNutCard) {
                slot = std::make_unique<WallNut>(
                    resources,
                    cell.x,
                    cell.y,
                    plantPosition(cell.x, cell.y));
            } else if (selectedCard == cherryBombCard) {
                slot = std::make_unique<CherryBomb>(
                    resources,
                    cell.x,
                    cell.y,
                    plantPosition(cell.x, cell.y));
            } else if (selectedCard == snowPeaCard) {
                slot = std::make_unique<SnowPea>(
                    resources,
                    cell.x,
                    cell.y,
                    plantPosition(cell.x, cell.y));
            } else if (selectedCard == repeaterCard) {
                slot = std::make_unique<Repeater>(
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
        if (currentLevel >= 3 && spawnedZombies == firstWaveTotal / 2) {
            spawnZombie();
        }
        if (currentLevel >= 4 && spawnedZombies >= firstWaveTotal - 3) {
            spawnZombie();
        }
        spawnTimer = 0.0f;
        spawnDelay = levelSpawnDelays[static_cast<std::size_t>(currentLevel - 1)];
    }

    for (auto& column : plants) {
        for (auto& plant : column) {
            if (plant) {
                const Plant::Action action = plant->update(
                    deltaSeconds,
                    rowHasZombie(plant->row()),
                    projectiles);
                if (action.sun > 0) {
                    sunTokens.emplace_back(
                        resources,
                        plant->sunSpawnPosition(),
                        plant->sunSpawnPosition());
                }
                if (action.explode) {
                    for (auto& zombie : zombies) {
                        if (zombie->isAlive()
                            && distance(zombie->position(), action.explosionCenter)
                                <= action.explosionRadius) {
                            zombie->destroy();
                        }
                    }
                }
                if (action.removeSelf) {
                    plant.reset();
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
                if (projectile.slowsTarget()) {
                    zombie->applySlow();
                }
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
        if (currentLevel < maxLevel) {
            maxUnlockedLevel = std::max(maxUnlockedLevel, currentLevel + 1);
        } else {
            maxUnlockedLevel = maxLevel;
        }
        saveProgress();
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
    drawText(
        "Progress: Level " + std::to_string(maxUnlockedLevel),
        {485.0f, 448.0f},
        18,
        sf::Color(245, 235, 190));
    drawText(
        "M Music: " + std::string(musicEnabled ? "On" : "Off")
            + "   N SFX: " + std::string(soundEffectsEnabled ? "On" : "Off")
            + "   R Reset",
        {330.0f, 586.0f},
        16,
        sf::Color(245, 235, 190));
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
        } else if (selectedCard == wallNutCard) {
            drawSprite("assets/img/Blurs/WallNut.png", plantPosition(cell.x, cell.y));
        } else if (selectedCard == cherryBombCard) {
            drawSprite("assets/img/Blurs/CherryBomb.png", plantPosition(cell.x, cell.y));
        } else if (selectedCard == snowPeaCard) {
            drawSprite("assets/img/Blurs/SnowPea.png", plantPosition(cell.x, cell.y));
        } else if (selectedCard == repeaterCard) {
            drawSprite("assets/img/Blurs/Repeater.png", plantPosition(cell.x, cell.y));
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
        gameOverButton.draw(window);
    }
}

void Game::drawSeedBank() {
    constexpr sf::Vector2f sunCounterTextPosition{190.0f, 61.0f};

    drawSprite("assets/ui/SeedBank.png", {170.0f, 0.0f});
    drawSprite("assets/ui/ShovelBank.png", {170.0f + 446.0f, 0.0f});
    drawSprite("assets/ui/Shovel.png", {170.0f + 446.0f, 0.0f});
    drawCards();
    drawText(std::to_string(sun), sunCounterTextPosition, 18, sf::Color::Black);
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
    drawText(
        std::string("M Music: ") + (musicEnabled ? "On" : "Off")
            + "  N SFX: " + (soundEffectsEnabled ? "On" : "Off"),
        {10.0f, 585.0f},
        15,
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
    beginLevel(maxUnlockedLevel);
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

void Game::loadProgress() {
    std::ifstream saveFile(progressPath);
    int savedLevel = 1;
    if (saveFile >> savedLevel) {
        maxUnlockedLevel = std::clamp(savedLevel, 1, maxLevel);
    }
}

void Game::saveProgress() const {
    std::ofstream saveFile(progressPath, std::ios::trunc);
    if (saveFile) {
        saveFile << maxUnlockedLevel << '\n';
    }
}

void Game::resetProgress() {
    maxUnlockedLevel = 1;
    saveProgress();
}

void Game::loadMusic() {
    static const std::array<const char*, 4> soundtrackFolders = {
        "soundtrack",
        "Soundtrack",
        "assets/soundtrack",
        "assets/Soundtrack"
    };

    for (const char* folder : soundtrackFolders) {
        const std::filesystem::path folderPath(folder);
        if (!std::filesystem::exists(folderPath)) {
            continue;
        }

        for (const auto& entry : std::filesystem::directory_iterator(folderPath)) {
            if (!entry.is_regular_file() || !isSupportedMusicFile(entry.path())) {
                continue;
            }

            if (backgroundMusic.openFromFile(entry.path())) {
                backgroundMusic.setLooping(true);
                backgroundMusic.setVolume(55.0f);
                musicLoaded = true;
                return;
            }
        }
    }
}

void Game::updateMusicPlayback() {
    if (!musicLoaded) {
        return;
    }

    if (musicEnabled) {
        if (backgroundMusic.getStatus() != sf::SoundSource::Status::Playing) {
            backgroundMusic.play();
        }
    } else {
        backgroundMusic.pause();
    }
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
    if (spawnedZombies >= firstWaveTotal) {
        return;
    }

    const int row = std::rand() % rows;
    if (currentLevel >= 4 && spawnedZombies % 5 == 4) {
        zombies.push_back(
            std::make_unique<ZombieVariant>(resources, row, ZombieVariant::Kind::Tough));
    } else if (currentLevel >= 3 && spawnedZombies % 4 == 1) {
        zombies.push_back(
            std::make_unique<ZombieVariant>(resources, row, ZombieVariant::Kind::Fast));
    } else if (currentLevel >= 2 && spawnedZombies % 3 == 2) {
        zombies.push_back(std::make_unique<ZombieConehead>(resources, row));
    } else {
        zombies.push_back(std::make_unique<ZombieBasic>(resources, row));
    }
    ++spawnedZombies;
}

bool Game::isCardPorted(int card) const {
    return card >= sunFlowerCard && card <= repeaterCard;
}

int Game::cardPrice(int card) const {
    return cardPrices[static_cast<std::size_t>(card)];
}
