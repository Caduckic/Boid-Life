#ifndef _BOID_HPP_
#define _BOID_HPP_

#define _USE_MATH_DEFINES
#include <math.h>
#include <iostream>
#include <vector>

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
    float rotSpeed {50.f};
    bool inputting {false};

    sf::Vector2f targetDir {0.f, 0.f};
    std::vector<float> angles {}; // this will be for adding the weights

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
    void input(float deltaTime) {
        inputting = false;
        targetDir = {0.f, 0.f};
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
            targetDir.x -= 1.f;
            inputting = true;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
            targetDir.x += 1.f;
            inputting = true;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
            targetDir.y += 1.f;
            inputting = true;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
            targetDir.y -= 1.f;
            inputting = true;
        }
        targetDir = normalize(targetDir);
    }

    void addRotWeight(sf::Vector2f extraDir, float distance) {
        // will use distance in the future to determine how much weight to add
        float newAngle = fmod(atan2(extraDir.y,extraDir.x),M_PI);
        if (newAngle <= 0) newAngle = (M_PI*2) + newAngle;
        angles.push_back(newAngle);
    }

    void normalizeTargetDir() {
        // averages the rotation weights and applies a normalized vector from it to target dir
        sf::Vector2f angleDir {0.f,0.f};
        for (float angle : angles) {
            angleDir.x += cos(angle);
            angleDir.y += sin(angle);
        }

        targetDir = normalize(angleDir);
        angles.clear();
    }

    void updateRotation(float deltaTime) {
        // this will need to be refactored at some point
        // currently you can't move pure left, it will just go right
        float angle = shape.getRotation()*M_PI/180.f;
        float targetAngle = fmod(atan2(targetDir.y,targetDir.x),M_PI);
        if (targetAngle <= 0) targetAngle = (M_PI*2) + targetAngle;
        dir = {static_cast<float>(cos(angle)), static_cast<float>(sin(angle))};

        float minusAngle = targetAngle - angle;
        if (minusAngle > M_PI || (minusAngle < 0.f && minusAngle > -M_PI)) {
            shape.rotate(-rotSpeed * deltaTime);
        }
        else if ((minusAngle < M_PI && minusAngle >= 0.f) || minusAngle < -M_PI) {
            shape.rotate(rotSpeed * deltaTime);
        }
        
        ghostShape.setRotation(shape.getRotation());
    }

    void update(float deltaTime) {
        // input(deltaTime);
        updateRotation(deltaTime);
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

    sf::Vector2f getPosition() {
        return shape.getPosition();
    }
};

#endif