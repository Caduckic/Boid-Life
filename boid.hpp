#ifndef _BOID_HPP_
#define _BOID_HPP_

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

#include "CONFIG.hpp"

class Boid {
private:
    sf::ConvexShape shape;

    sf::Vector2f dir;
    float speed {50.f};

    void initShape(sf::Vector2f pos) {
        shape.setPointCount(3);
        shape.setPoint(0, sf::Vector2f(5.f, 0.f));
        shape.setPoint(1, sf::Vector2f(10.f, 15.f));
        shape.setPoint(2, sf::Vector2f(0.f, 15.f));
        shape.setOrigin(sf::Vector2f(5.f, 7.5f));

        shape.setFillColor(sf::Color::Blue);
        shape.setPosition(pos);
    }

public:
    Boid(sf::Vector2f pos, sf::Vector2f dir) : dir{dir} {
        initShape(pos);
    }

    virtual ~Boid() = default;

    void updateBoundsColliding() {
        if (shape.getPosition().x > WINDOW_SIZE.x) {
            shape.setPosition(0.f, shape.getPosition().y);
        }
        else if (shape.getPosition().x < 0) {
            shape.setPosition(WINDOW_SIZE.x, shape.getPosition().y);
        }
        if (shape.getPosition().y > WINDOW_SIZE.y) {
            shape.setPosition(shape.getPosition().x, 0.f);
        }
        else if (shape.getPosition().y < 0) {
            shape.setPosition(shape.getPosition().x, WINDOW_SIZE.y);
        }
    }

    void update(float deltaTime) {
        shape.move(dir * speed * deltaTime);
        updateBoundsColliding();
    }

    void render(sf::RenderTarget& target) {
        target.draw(shape);
    }
};

#endif