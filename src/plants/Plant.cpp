#include "plants/Plant.hpp"

Plant::Plant(int column, int row, sf::Vector2f drawPosition)
    : gridColumn(column), gridRow(row), position(drawPosition) {}

void Plant::damage(float amount) {
    health -= amount;
}

bool Plant::isAlive() const {
    return health > 0.0f;
}

int Plant::row() const {
    return gridRow;
}

int Plant::column() const {
    return gridColumn;
}

sf::Vector2f Plant::sunSpawnPosition() const {
    return {position.x - 10.0f, position.y - 10.0f};
}

sf::FloatRect Plant::bounds() const {
    return {{position.x + 8.0f, position.y + 10.0f}, {62.0f, 78.0f}};
}
