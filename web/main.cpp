#include <SDL.h>
#include <emscripten.h>

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <string>
#include <vector>

namespace {
constexpr int windowWidth = 820;
constexpr int windowHeight = 620;
constexpr int rows = 5;
constexpr int columns = 9;
constexpr float gridLeft = 50.0f;
constexpr float gridTop = 80.0f;
constexpr float cellWidth = 81.0f;
constexpr float cellHeight = 99.0f;

constexpr std::array<int, 6> cardPrices = {50, 100, 50, 150, 175, 200};
constexpr std::array<float, 6> cardCooldowns = {2.0f, 2.5f, 5.0f, 15.0f, 8.0f, 10.0f};
constexpr std::array<const char*, 6> cardNames = {
    "Sun", "Pea", "Nut", "Bomb", "Snow", "Repeat"
};
constexpr std::array<int, 4> levelTotals = {8, 12, 16, 22};
constexpr std::array<float, 4> levelSpawnDelays = {4.0f, 4.6f, 3.0f, 2.4f};
constexpr std::array<float, 4> levelFirstDelays = {6.0f, 8.0f, 5.0f, 3.5f};

enum class Scene {
    Title,
    Playing,
    Won,
    Lost
};

enum class PlantType {
    SunFlower,
    Peashooter,
    WallNut,
    CherryBomb,
    SnowPea,
    Repeater
};

enum class ProjectileType {
    Pea,
    Snow
};

struct Plant {
    PlantType type = PlantType::Peashooter;
    int column = 0;
    int row = 0;
    float health = 5.0f;
    float timer = 0.0f;
    bool exploded = false;
};

struct Zombie {
    int row = 0;
    float x = 0.0f;
    float health = 7.0f;
    float speed = 20.0f;
    float slowTimer = 0.0f;
    bool conehead = false;
};

struct Projectile {
    ProjectileType type = ProjectileType::Pea;
    int row = 0;
    float x = 0.0f;
    float y = 0.0f;
};

struct SunToken {
    float x = 0.0f;
    float y = 0.0f;
    float age = 0.0f;
    bool collecting = false;
};

SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;
Scene scene = Scene::Title;

std::array<std::array<int, rows>, columns> plantGrid{};
std::vector<Plant> plants;
std::vector<Zombie> zombies;
std::vector<Projectile> projectiles;
std::vector<SunToken> suns;

int sun = 500;
int selectedCard = -1;
int currentLevel = 1;
int spawned = 0;
int defeated = 0;
float spawnTimer = 0.0f;
float spawnDelay = 6.0f;
std::array<float, 6> cardTimers{};
uint32_t previousTicks = 0;

void setColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) {
    SDL_SetRenderDrawColor(renderer, r, g, b, a);
}

void fillRect(float x, float y, float w, float h) {
    SDL_FRect rect{x, y, w, h};
    SDL_RenderFillRectF(renderer, &rect);
}

void drawRect(float x, float y, float w, float h) {
    SDL_FRect rect{x, y, w, h};
    SDL_RenderDrawRectF(renderer, &rect);
}

void fillCircle(float cx, float cy, float radius) {
    for (int y = static_cast<int>(-radius); y <= static_cast<int>(radius); ++y) {
        for (int x = static_cast<int>(-radius); x <= static_cast<int>(radius); ++x) {
            if (x * x + y * y <= radius * radius) {
                SDL_RenderDrawPointF(renderer, cx + x, cy + y);
            }
        }
    }
}

float laneY(int row) {
    return gridTop + row * cellHeight + cellHeight * 0.5f;
}

float cellX(int column) {
    return gridLeft + column * cellWidth + cellWidth * 0.5f;
}

bool insideGrid(int x, int y) {
    return x >= gridLeft
        && y >= gridTop
        && x < gridLeft + columns * cellWidth
        && y < gridTop + rows * cellHeight;
}

int plantCost(PlantType type) {
    return cardPrices[static_cast<int>(type)];
}

float plantHealth(PlantType type) {
    return type == PlantType::WallNut ? 30.0f : 5.0f;
}

bool rowHasZombie(int row, float x) {
    return std::any_of(zombies.begin(), zombies.end(), [row, x](const Zombie& zombie) {
        return zombie.row == row && zombie.x > x;
    });
}

void resetLevel(int level) {
    plants.clear();
    zombies.clear();
    projectiles.clear();
    suns.clear();
    for (auto& column : plantGrid) {
        column.fill(-1);
    }
    currentLevel = std::clamp(level, 1, 4);
    sun = 500 + (currentLevel - 1) * 75;
    selectedCard = -1;
    spawned = 0;
    defeated = 0;
    spawnTimer = 0.0f;
    spawnDelay = levelFirstDelays[currentLevel - 1];
    cardTimers = cardCooldowns;
    scene = Scene::Playing;
}

void spawnZombie() {
    if (spawned >= levelTotals[currentLevel - 1]) {
        return;
    }
    const bool conehead = currentLevel >= 2 && spawned % 3 == 2;
    zombies.push_back({
        spawned % rows,
        gridLeft + columns * cellWidth + 20.0f,
        conehead ? 14.0f : 7.0f,
        20.0f,
        0.0f,
        conehead
    });
    spawned++;
}

void handleMouse(int x, int y) {
    if (scene == Scene::Title) {
        resetLevel(1);
        return;
    }
    if (scene == Scene::Won) {
        if (currentLevel < 4) {
            resetLevel(currentLevel + 1);
        } else {
            scene = Scene::Title;
        }
        return;
    }
    if (scene == Scene::Lost) {
        scene = Scene::Title;
        return;
    }
    if (scene != Scene::Playing) {
        return;
    }

    if (y >= 5 && y <= 78 && x >= 245 && x < 245 + 55 * 6) {
        const int card = (x - 245) / 55;
        if (cardTimers[card] <= 0.0f && sun >= cardPrices[card]) {
            selectedCard = card;
        }
        return;
    }

    if (selectedCard >= 0 && insideGrid(x, y)) {
        const int column = static_cast<int>((x - gridLeft) / cellWidth);
        const int row = static_cast<int>((y - gridTop) / cellHeight);
        if (plantGrid[column][row] == -1) {
            const auto type = static_cast<PlantType>(selectedCard);
            plantGrid[column][row] = static_cast<int>(plants.size());
            plants.push_back({type, column, row, plantHealth(type)});
            sun -= plantCost(type);
            cardTimers[selectedCard] = cardCooldowns[selectedCard];
            selectedCard = -1;
        }
    }
}

void update(float dt) {
    if (scene != Scene::Playing) {
        return;
    }

    for (float& timer : cardTimers) {
        timer = std::max(0.0f, timer - dt);
    }

    spawnTimer += dt;
    if (spawnTimer >= spawnDelay) {
        spawnZombie();
        if (currentLevel >= 3 && spawned == levelTotals[currentLevel - 1] / 2) {
            spawnZombie();
        }
        spawnTimer = 0.0f;
        spawnDelay = levelSpawnDelays[currentLevel - 1];
    }

    for (Plant& plant : plants) {
        if (plant.health <= 0.0f) {
            continue;
        }
        plant.timer += dt;
        const float px = cellX(plant.column);
        const float py = laneY(plant.row);
        const bool hasTarget = rowHasZombie(plant.row, px);

        if (plant.type == PlantType::SunFlower && plant.timer >= 10.0f) {
            plant.timer = 0.0f;
            suns.push_back({px - 40.0f, py - 55.0f});
        } else if (plant.type == PlantType::Peashooter && hasTarget && plant.timer >= 2.0f) {
            plant.timer = 0.0f;
            projectiles.push_back({ProjectileType::Pea, plant.row, px + 25.0f, py - 15.0f});
        } else if (plant.type == PlantType::SnowPea && hasTarget && plant.timer >= 2.0f) {
            plant.timer = 0.0f;
            projectiles.push_back({ProjectileType::Snow, plant.row, px + 25.0f, py - 15.0f});
        } else if (plant.type == PlantType::Repeater && hasTarget && plant.timer >= 2.0f) {
            plant.timer = 0.0f;
            projectiles.push_back({ProjectileType::Pea, plant.row, px + 25.0f, py - 15.0f});
            projectiles.push_back({ProjectileType::Pea, plant.row, px + 50.0f, py - 15.0f});
        } else if (plant.type == PlantType::CherryBomb && !plant.exploded && plant.timer >= 0.55f) {
            plant.exploded = true;
            for (Zombie& zombie : zombies) {
                const float dx = zombie.x - px;
                const float dy = laneY(zombie.row) - py;
                if (std::sqrt(dx * dx + dy * dy) <= 300.0f) {
                    zombie.health = 0.0f;
                }
            }
            plant.health = 0.0f;
        }
    }

    for (SunToken& token : suns) {
        token.age += dt;
        if (token.age >= 0.6f) {
            token.collecting = true;
        }
        if (token.collecting) {
            const float dx = 170.0f - token.x;
            const float dy = 0.0f - token.y;
            const float len = std::max(1.0f, std::sqrt(dx * dx + dy * dy));
            token.x += dx / len * 800.0f * dt;
            token.y += dy / len * 800.0f * dt;
        }
    }
    int collectedSunCount = 0;
    suns.erase(std::remove_if(suns.begin(), suns.end(), [&collectedSunCount](const SunToken& token) {
        if (token.collecting && std::abs(token.x - 170.0f) < 20.0f && token.y < 20.0f) {
            collectedSunCount++;
            return true;
        }
        return false;
    }), suns.end());
    sun += collectedSunCount * 25;

    for (Projectile& projectile : projectiles) {
        projectile.x += 600.0f * dt;
        for (Zombie& zombie : zombies) {
            if (zombie.row == projectile.row
                && zombie.health > 0.0f
                && std::abs(zombie.x - projectile.x) < 28.0f) {
                zombie.health -= 1.0f;
                if (projectile.type == ProjectileType::Snow) {
                    zombie.slowTimer = 1.5f;
                }
                projectile.x = 1000.0f;
                break;
            }
        }
    }

    for (Zombie& zombie : zombies) {
        if (zombie.health <= 0.0f) {
            continue;
        }
        zombie.slowTimer = std::max(0.0f, zombie.slowTimer - dt);
        Plant* target = nullptr;
        for (Plant& plant : plants) {
            if (plant.health > 0.0f
                && plant.row == zombie.row
                && std::abs(cellX(plant.column) - zombie.x) < 38.0f) {
                target = &plant;
                break;
            }
        }

        if (target) {
            target->health -= 1.2f * dt;
        } else {
            zombie.x -= (zombie.slowTimer > 0.0f ? 10.0f : zombie.speed) * dt;
        }

        if (zombie.x < gridLeft) {
            scene = Scene::Lost;
        }
    }

    for (Plant& plant : plants) {
        if (plant.health <= 0.0f && plantGrid[plant.column][plant.row] >= 0) {
            plantGrid[plant.column][plant.row] = -1;
        }
    }

    const auto before = zombies.size();
    zombies.erase(std::remove_if(zombies.begin(), zombies.end(), [](const Zombie& zombie) {
        return zombie.health <= 0.0f;
    }), zombies.end());
    defeated += static_cast<int>(before - zombies.size());

    projectiles.erase(std::remove_if(projectiles.begin(), projectiles.end(), [](const Projectile& projectile) {
        return projectile.x > windowWidth;
    }), projectiles.end());

    if (spawned >= levelTotals[currentLevel - 1] && zombies.empty()) {
        scene = Scene::Won;
    }
}

void drawPlant(const Plant& plant) {
    const float x = cellX(plant.column);
    const float y = laneY(plant.row);
    switch (plant.type) {
    case PlantType::SunFlower:
        setColor(236, 184, 52);
        fillCircle(x, y, 24.0f);
        setColor(90, 150, 55);
        fillRect(x - 7.0f, y + 18.0f, 14.0f, 24.0f);
        break;
    case PlantType::Peashooter:
        setColor(64, 170, 78);
        fillCircle(x, y, 24.0f);
        setColor(38, 112, 58);
        fillRect(x + 16.0f, y - 10.0f, 28.0f, 14.0f);
        break;
    case PlantType::WallNut:
        setColor(172, 121, 62);
        fillCircle(x, y, 30.0f);
        break;
    case PlantType::CherryBomb:
        setColor(205, 54, 50);
        fillCircle(x - 12.0f, y, 18.0f);
        fillCircle(x + 12.0f, y, 18.0f);
        break;
    case PlantType::SnowPea:
        setColor(112, 198, 205);
        fillCircle(x, y, 24.0f);
        setColor(60, 132, 150);
        fillRect(x + 16.0f, y - 10.0f, 28.0f, 14.0f);
        break;
    case PlantType::Repeater:
        setColor(54, 155, 70);
        fillCircle(x - 8.0f, y, 22.0f);
        fillCircle(x + 12.0f, y, 22.0f);
        break;
    }
}

void drawScene() {
    setColor(44, 82, 58);
    SDL_RenderClear(renderer);

    setColor(226, 203, 125);
    fillRect(0.0f, 0.0f, windowWidth, 80.0f);
    setColor(95, 65, 42);
    fillRect(170.0f, 0.0f, 446.0f, 80.0f);

    for (int i = 0; i < 6; ++i) {
        setColor(cardTimers[i] <= 0.0f && sun >= cardPrices[i] ? 232 : 105, 210, 136);
        fillRect(245.0f + i * 55.0f, 5.0f, 48.0f, 70.0f);
        setColor(45, 45, 45);
        drawRect(245.0f + i * 55.0f, 5.0f, 48.0f, 70.0f);
    }

    for (int column = 0; column < columns; ++column) {
        for (int row = 0; row < rows; ++row) {
            const bool alternate = (column + row) % 2 == 0;
            setColor(alternate ? 96 : 78, alternate ? 146 : 124, 72);
            fillRect(gridLeft + column * cellWidth, gridTop + row * cellHeight, cellWidth - 3.0f, cellHeight - 3.0f);
        }
    }

    for (const Plant& plant : plants) {
        if (plant.health > 0.0f) {
            drawPlant(plant);
        }
    }

    for (const Projectile& projectile : projectiles) {
        if (projectile.type == ProjectileType::Snow) {
            setColor(150, 225, 255);
        } else {
            setColor(118, 220, 74);
        }
        fillCircle(projectile.x, projectile.y, 8.0f);
    }

    for (const SunToken& token : suns) {
        setColor(255, 226, 91, 220);
        fillCircle(token.x, token.y, 34.0f);
        setColor(255, 244, 150, 230);
        fillCircle(token.x + 20.0f, token.y + 20.0f, 22.0f);
    }

    for (const Zombie& zombie : zombies) {
        setColor(zombie.conehead ? 92 : 110, 102, 126);
        fillCircle(zombie.x, laneY(zombie.row), 28.0f);
        setColor(235, 137, 82);
        if (zombie.conehead) {
            fillRect(zombie.x - 15.0f, laneY(zombie.row) - 44.0f, 30.0f, 18.0f);
        }
    }

    if (scene == Scene::Title || scene == Scene::Won || scene == Scene::Lost) {
        setColor(25, 25, 25, 210);
        fillRect(160.0f, 190.0f, 500.0f, 210.0f);
        setColor(238, 218, 138);
        drawRect(160.0f, 190.0f, 500.0f, 210.0f);
    }

    SDL_RenderPresent(renderer);
}

void mainLoop() {
    SDL_Event event{};
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            emscripten_cancel_main_loop();
        } else if (event.type == SDL_MOUSEBUTTONDOWN) {
            handleMouse(event.button.x, event.button.y);
        }
    }

    const uint32_t now = SDL_GetTicks();
    const float dt = std::min(0.05f, static_cast<float>(now - previousTicks) / 1000.0f);
    previousTicks = now;
    update(dt);
    drawScene();
}
}

int main() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        return 1;
    }

    window = SDL_CreateWindow(
        "Plants vs. Zombies C++ Web Preview",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        windowWidth,
        windowHeight,
        SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    if (!window || !renderer) {
        return 1;
    }

    for (auto& column : plantGrid) {
        column.fill(-1);
    }
    previousTicks = SDL_GetTicks();
    emscripten_set_main_loop(mainLoop, 0, 1);
    return 0;
}
