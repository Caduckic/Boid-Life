#ifndef _BOID_HPP_
#define _BOID_HPP_

#define _USE_MATH_DEFINES
#include <math.h>
#include <iostream>
#include <vector>
#include <ctime>

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

#include "CONFIG.hpp"
#include "helper.hpp"

struct AngleWeight {
    float angle;
    float weight;
};

class Boid {
private:
    sf::ConvexShape shape;
    sf::ConvexShape ghostShape; // used for drawing extra when boid passes window edge

    sf::Vector2f dir;
    float speed {50.f};
    float rotSpeed {50.f};
    bool inputting {false};

    sf::Vector2f targetDir {0.f, 0.f};
    std::vector<AngleWeight> angles; // this will be for adding the weights

    void initShape(sf::Vector2f pos) {
        shape.setPointCount(3);
        shape.setPoint(0, BOID_POINTS[0]);
        shape.setPoint(1, BOID_POINTS[1]);
        shape.setPoint(2, BOID_POINTS[2]);
        shape.setOrigin(sf::Vector2f(7.5f, 5.f));

        shape.setFillColor(sf::Color(rand()%255+1, rand()%255+1, rand()%255+1));
        shape.setPosition(pos);
        float angle = fmod(atan2(dir.y,dir.x),M_PI);
        if (angle <= 0) angle = (M_PI*2) + angle;
        shape.setRotation(angle);

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

    // useful for debugging
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
        float newAngle = fmod(atan2(extraDir.y,extraDir.x),M_PI);
        if (newAngle <= 0) newAngle = (M_PI*2) + newAngle;
        // distance is used to make a value that is higher the closer another boid is, this will be our weights
        // std::cout << distance << std::endl;
        angles.push_back({newAngle, (10*(100.f - distance)) / 200.f});
    }

    void normalizeTargetDir() {
        // averages the rotation weights and applies a normalized vector from it to target dir
        sf::Vector2f angleDir {0.f,0.f};
        for (AngleWeight& angle : angles) {
            // if the weight is less than one ignore it
            if (angle.weight < 1) continue;
            
            // else for the amount of weight floored to an int, add the angle
            for (size_t i {0}; i < (int)floor(angle.weight); i++) {
                angleDir.x += cos(angle.angle);
                angleDir.y += sin(angle.angle);
            }
        }

        targetDir = normalize(angleDir);
        angles.erase(angles.begin(), angles.end());
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
        // if (inputting)
            shape.move(dir * speed * deltaTime);
        updateBoundsColliding();
    }

    void render(sf::RenderTarget& target) {
        target.draw(shape);
        sf::Vector2f pos = shape.getPosition();
        
        // use 15 as its the largest the shape is able to be
        bool showGhost {false};
        if (pos.x < 15.f) {
            ghostShape.setPosition(pos.x + static_cast<float>(WINDOW_SIZE.x), pos.y);
            showGhost = true;
        }
        else if (pos.x > WINDOW_SIZE.x - 15.f) {
            ghostShape.setPosition(pos.x - static_cast<float>(WINDOW_SIZE.x), pos.y);
            showGhost = true;
        }
        if (pos.y < 15.f) {
            ghostShape.setPosition(pos.x, pos.y + static_cast<float>(WINDOW_SIZE.y));
            showGhost = true;
        }
        else if (pos.y > WINDOW_SIZE.y - 15.f) {
            ghostShape.setPosition(pos.x, pos.y - static_cast<float>(WINDOW_SIZE.y));
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