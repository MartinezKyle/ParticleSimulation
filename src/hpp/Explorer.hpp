#ifndef EXPLORER_H
#define EXPLORER_H

#include <SFML/Graphics.hpp>

class Explorer : public sf::Drawable {
public:
    Explorer(int clientID, double x, double y);
    double getXCoord() const;
    double getYCoord() const;
    double getID() const;
    void updateCoords(double x, double y);
    void moveUp();
    void moveDown();
    void moveLeft();
    void moveRight();
    bool getMove();
    void revertMove();
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

private:
    double x_coord;
    double y_coord;
    sf::CircleShape shape;
    bool moved;
    int clientID;
};

#endif // EXPLORER_H
