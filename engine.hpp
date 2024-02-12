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
        boids.push_back(Boid({100.f, 100.f}, {-1.f, -0.3f}));
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