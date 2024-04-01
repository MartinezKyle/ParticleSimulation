#include "Explorer.hpp"

#include <iostream>
#include <cmath>
#include <SFML/Graphics.hpp>

Explorer::Explorer(double x, double y) : x_coord(x), y_coord(y) {
    shape.setRadius(10); // Set radius of explorer
    shape.setFillColor(sf::Color::Blue);
    shape.setPosition(x_coord - 10, y_coord - 10); // Adjust position for drawing
}

double Explorer::getXCoord() const {
    return x_coord;
}

double Explorer::getYCoord() const {
    return y_coord;
}

void Explorer::moveUp() {
    y_coord -= 5;
    shape.setPosition(x_coord, y_coord); // Adjust position for drawing
}

void Explorer::moveDown() {
    y_coord += 5;
    shape.setPosition(x_coord, y_coord); // Adjust position for drawing
}

void Explorer::moveLeft() {
    x_coord -= 5;
    shape.setPosition(x_coord, y_coord); // Adjust position for drawing
}

void Explorer::moveRight() {
    x_coord += 5;
    shape.setPosition(x_coord, y_coord); // Adjust position for drawing
}

void Explorer::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    target.draw(shape, states);
}
