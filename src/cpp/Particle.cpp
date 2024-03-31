#include "Particle.hpp"

#include <iostream>
#include <cmath>
#include <SFML/Graphics.hpp>
#include <corecrt_math_defines.h>

Particle::Particle(double x, double y, double velocity, double angle) 
    : x_coord(x), y_coord(y), velocity(velocity), angle(angle) {
    shape.setRadius(5); // Set radius of particle
    shape.setFillColor(sf::Color::Red);
    shape.setPosition(x_coord - 5, 720 - y_coord - 5); // Adjust position for drawing
}

void Particle::updatePosition(double time) {
    double x2 = x_coord + getVelocityX() * time;
    double y2 = y_coord + getVelocityY() * time;
    if (x2 <= 0 || x2 >= 1280) {
        angle = 180 - angle; 
    }
    if (y2 <= 0 || y2 >= 720) {
        angle = -angle;
    }
    x_coord += getVelocityX() * time;
    y_coord += getVelocityY() * time;
    shape.setPosition(x_coord - 5, 720 - y_coord - 5); // Adjust position for drawing
}

double Particle::getXCoord() const {
    return x_coord;
}

double Particle::getYCoord() const {
    return y_coord;
}

double Particle::getAngle() const {
    return angle;
}

double Particle::getVelocity() const {
    return velocity;
}

double Particle::getVelocityX() const {
    return velocity * std::cos(angle * M_PI / 180.0);
}

double Particle::getVelocityY() const {
    return velocity * std::sin(angle * M_PI / 180.0);
}

void Particle::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    target.draw(shape, states);
}
