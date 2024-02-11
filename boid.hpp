#ifndef _BOID_HPP_
#define _BOID_HPP_

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

class Boid {
private:
    sf::ConvexShape shape;

    void initShape() {
        shape.setPointCount(3);
        shape.setPoint(0, sf::Vector2f(5.f, 0.f));
        shape.setPoint(1, sf::Vector2f(10.f, 15.f));
        shape.setPoint(2, sf::Vector2f(0.f, 15.f));
        shape.setOrigin(sf::Vector2f(5.f, 7.5f));

        shape.setFillColor(sf::Color::Blue);
        shape.setPosition(sf::Vector2f(100.f,100.f)); // temp so we can see it
    }

public:
    Boid() {
        initShape();
    }

    virtual ~Boid() = default;

    void update(float deltaTime) {

    }

    void render(sf::RenderTarget& target) {
        target.draw(shape);
    }
};

#endif