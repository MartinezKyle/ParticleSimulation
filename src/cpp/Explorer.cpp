#include "Explorer.hpp"

#include <iostream>
#include <cmath>
#include <SFML/Graphics.hpp>

Explorer::Explorer(int clientID, double x, double y) : x_coord(x), y_coord(y) {
    this->clientID = clientID;
    shape.setRadius(10); 
    shape.setFillColor(sf::Color::Blue);
    shape.setPosition(x_coord - 10, y_coord - 10); 
    moved = true;
}

double Explorer::getXCoord() const {
    return x_coord;
}

double Explorer::getYCoord() const {
    return y_coord;
}

double Explorer::getID() const {
    return clientID;
}

void Explorer::updateCoords(double x, double y){
        x_coord = x;
        y_coord = y;
        shape.setPosition(x_coord, y_coord);
        moved = true;
    }

void Explorer::moveUp() {
    y_coord -= 5;
    shape.setPosition(x_coord, y_coord); 
    moved = true;
}

void Explorer::moveDown() {
    y_coord += 5;
    shape.setPosition(x_coord, y_coord); 
    moved = true;
}

void Explorer::moveLeft() {
    x_coord -= 5;
    shape.setPosition(x_coord, y_coord); 
    moved = true;
}

void Explorer::moveRight() {
    x_coord += 5;
    shape.setPosition(x_coord, y_coord); 
    moved = true;
}

void Explorer::revertMove() {
    moved = false;
}

bool Explorer::getMove() {
    return moved;
}

void Explorer::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    target.draw(shape, states);
}
