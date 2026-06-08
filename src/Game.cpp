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
          "Button1.png",
          "Highlight.png",
          {410.0f, 65.0f}),
      quitButton(resources,
          "Quit1.png",
          "Quit2.png",
          {724.0f, 515.0f}),
      gameOverButton(resources,
          "GameOver.png",
          "GameOver_highlight.png",
          {0.0f, 0.0f}) {
    window.setFramerateLimit(60);
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
 //BACKGROUND
}

void Game::drawAdventure() {
   //PLACEHOLDER
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

    