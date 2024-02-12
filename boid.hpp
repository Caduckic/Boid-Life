#ifndef _BOID_HPP_
#define _BOID_HPP_

#include <cmath>

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

#include "CONFIG.hpp"
#include "helper.hpp"

class Boid {
private:
    sf::ConvexShape shape;
    sf::ConvexShape ghostShape; // used for drawing extra when boid passes window edge

    sf::Vector2f dir;
    float speed {50.f};

    sf::Vector2f targetDir {0.f, 0.f};

    void initShape(sf::Vector2f pos) {
        shape.setPointCount(3);
        shape.setPoint(0, BOID_POINTS[0]);
        shape.setPoint(1, BOID_POINTS[1]);
        shape.setPoint(2, BOID_POINTS[2]);
        shape.setOrigin(sf::Vector2f(7.5f, 5.f));

        shape.setFillColor(sf::Color::Blue);
        shape.setPosition(pos);

        ghostShape = shape;
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

    // temp to test out rendering at edges of the window
    void input() {
        dir = {0.f, 0.f};
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
            dir.x -= 1.f;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
            dir.x = 1.f;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
            dir.y += 1.f;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
            dir.y -= 1.f;
        }
        dir = normalize(dir);
    }

    void updateRotation() {
        float rot = atan2(dir.y, dir.x)*180.f/M_PI;
        shape.setRotation(rot);
        ghostShape.setRotation(rot);
    }

    void update(float deltaTime) {
        input();
        updateRotation();
        shape.move(dir * speed * deltaTime);
        updateBoundsColliding();
    }

    void render(sf::RenderTarget& target) {
        target.draw(shape);
        sf::Vector2f pos = shape.getPosition();
        
        // use 15 as its the largest the shape is able to be
        bool showGhost {false};
        if (pos.x < 15.f) {
            ghostShape.setPosition(pos.x + 200.f, pos.y);
            showGhost = true;
        }
        else if (pos.x > WINDOW_SIZE.x - 15.f) {
            ghostShape.setPosition(pos.x - 200.f, pos.y);
            showGhost = true;
        }
        if (pos.y < 15.f) {
            ghostShape.setPosition(pos.x, pos.y + 200.f);
            showGhost = true;
        }
        else if (pos.y > WINDOW_SIZE.y - 15.f) {
            ghostShape.setPosition(pos.x, pos.y - 200.f);
            showGhost = true;
        }

        if (showGhost) {
            target.draw(ghostShape);
        }
    }
};

#endif