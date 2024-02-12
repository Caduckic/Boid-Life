#ifndef _ENGINE_HPP
#define _ENGINE_HPP

#ifdef WIN32
    #include <windows.h>
#endif

#include <memory>
#include <vector>

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

#include "boid.hpp"
#include "CONFIG.hpp"
#include "helper.hpp"

class Engine {
private:
    std::shared_ptr<sf::RenderWindow> window;
    sf::Event ev;

    // Window moving logic
    float lastDownX {};
    float lastDownY {};
    bool isMouseDragging {false};

    // Delta Time Logic (for framerate independace)
    sf::Clock deltaClock;
    sf::Time dt;
    float deltaTime;

    std::vector<Boid> boids;

    void updateDeltaTime() {
        dt = deltaClock.restart();
        deltaTime = dt.asSeconds();
    }

    void initWindow() {
        window = std::make_shared<sf::RenderWindow>(sf::VideoMode(WINDOW_SIZE.x, WINDOW_SIZE.y), "Boid Life", sf::Style::None);
        window->setFramerateLimit(240);
        #ifdef WIN32
            HWND hwnd = window->getSystemHandle();
            SetWindowPos(hwnd, HWND_TOPMOST, 0,0,0,0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
        #endif
    }

    void initBoids() {
        for (size_t i {0}; i < 2; i++) {
            boids.push_back(Boid({100.f+i, 100.f+i}, {-1.f, -0.3f}));
        }
    }

public:
    Engine() {
        initWindow();
        initBoids();
    }

    virtual ~Engine() = default;

    void run() {
        while (window->isOpen()) {
            pollEvents();
            update();
            render();
        }
    }

    void pollEvents() {
        while(window->pollEvent(ev)) {
            switch (ev.type)
            {
            case sf::Event::KeyPressed:
                if (ev.key.code == sf::Keyboard::Escape)
                    window->close();
                break;
            case sf::Event::Closed:
                window->close();
                break;
            case sf::Event::MouseMoved:
                if (isMouseDragging) {
                window->setPosition(window->getPosition() + sf::Vector2<int>(ev.mouseMove.x- lastDownX, ev.mouseMove.y- lastDownY));
                }
                break;
            case sf::Event::MouseButtonPressed:
                lastDownX = ev.mouseButton.x;
                lastDownY = ev.mouseButton.y;
                isMouseDragging = true;
                break;
            case sf::Event::MouseButtonReleased:
                isMouseDragging = false;
                break;
            }
        }
    }

    void update() {
        updateDeltaTime();

        for (size_t i {0}; i < boids.size(); i++) {
            for (size_t j {0}; j < boids.size(); j++) {
                if (i != j) {
                    float distance = getDistance(boids.at(i).getPosition(), boids.at(j).getPosition());
                    // doesn't really make any sense currently, but hey we have basic boids for now at least
                    sf::Vector2f dirToBoid = boids.at(i).getPosition() - boids.at(j).getPosition();
                    boids.at(i).addToTargetDir(dirToBoid, distance);
                }
                // float distance = getDistance(sf::Vector2f{WINDOW_SIZE.x/2.f,WINDOW_SIZE.y/2.f}, boids.at(i).getPosition());
                // boids.at(i).addToTargetDir(sf::Vector2f{WINDOW_SIZE.x/2.f,WINDOW_SIZE.y/2.f} - boids.at(i).getPosition(), distance/2.f);
                // boids.at(i).addToTargetDir(boids.at(i).getPosition() - sf::Vector2f{WINDOW_SIZE.x,WINDOW_SIZE.y}, distance/0.5f);
                // boids.at(i).addToTargetDir(boids.at(i).getPosition() - sf::Vector2f{WINDOW_SIZE.x,0.f}, distance/0.5f);
                // boids.at(i).addToTargetDir(boids.at(i).getPosition() - sf::Vector2f{0.f, WINDOW_SIZE.y}, distance/0.5f);
                // boids.at(i).addToTargetDir(boids.at(i).getPosition() - sf::Vector2f{0.f, 0.f}, distance/0.5f);
            }
            boids.at(i).normalizeTargetDir();

            boids.at(i).update(deltaTime);
        }
    }

    void render() {
        window->clear();

            // rendering boids
            for (auto boid : boids) {
                boid.render(*window);
            }

        window->display();
    }
};

#endif