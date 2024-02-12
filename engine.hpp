#ifndef _ENGINE_HPP
#define _ENGINE_HPP

#ifdef WIN32
    #include <windows.h>
#endif

#include <memory>
#include <vector>
#include <sstream>

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

    // FPS displaying
    sf::Clock FPSClock;
    sf::Font font;
    sf::Text text;
    bool displayFPS {true};
    sf::Time lastTime;

    std::vector<Boid> boids;
    // float GlobalRotation;

    void updateDeltaTime() {
        dt = deltaClock.restart();
        deltaTime = dt.asSeconds();
    }

    void updateFPS() {
        sf::Time currentTime = FPSClock.getElapsedTime();
        float fps = 1.0f / (currentTime.asSeconds() - lastTime.asSeconds());
        lastTime = currentTime;

        std::stringstream ss;
        ss << "FPS: " << fps;
        text.setString(ss.str());
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
        for (size_t i {0}; i < 200; i++) {
            boids.push_back(Boid({100.f+i, 100.f+i}, {-1.f, -0.3f}));
        }
    }

    void initFontAndText() {
        if (!font.loadFromFile("./BebasNeue-Regular.ttf"))
            std::cerr << "Failed to load font" << std::endl;

        text.setFont(font);
        text.setFillColor(sf::Color::White);
        text.setString("NONE");
        text.setCharacterSize(12);
    }

public:
    Engine() {
        initWindow();
        initBoids();
        initFontAndText();
    }

    virtual ~Engine() = default;

    void run() {
        while (window->isOpen()) {
            pollEvents();
            update();
            render();
            if (displayFPS)
                updateFPS();
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
                    sf::Vector2f dirToBoid = boids.at(i).getPosition() - boids.at(j).getPosition();
                    boids.at(i).addRotWeight(dirToBoid, distance);
                }
                // boids.at(i).addToTargetDir(sf::Vector2f{1.f, 0.f}, 1.f);
                // float distance = getDistance(sf::Vector2f{WINDOW_SIZE.x/2.f,WINDOW_SIZE.y/2.f}, boids.at(i).getPosition());

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

            if (displayFPS)
                window->draw(text);

        window->display();
    }
};

#endif