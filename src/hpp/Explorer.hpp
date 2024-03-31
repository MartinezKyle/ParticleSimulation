#ifndef EXPLORER_H
#define EXPLORER_H

#include <SFML/Graphics.hpp>

class Explorer : public sf::Drawable {
public:
    Explorer(double x, double y);
    double getXCoord() const;
    double getYCoord() const;
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

private:
    double x_coord;
    double y_coord;
    sf::CircleShape shape;
};

#endif // EXPLORER_H
