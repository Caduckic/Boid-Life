#ifndef _ENGINE_HPP
#define _ENGINE_HPP

#ifdef WIN32
    #include <windows.h>
#endif

#include <memory>
#include <vector>
#include <array>
#include <sstream>
#include <ctime>

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
    float GlobalRotation {0.f};
    float GlobalRotationSpeed {5.f};

    std::array<std::array<std::vector<Boid>, GRID_COUNT>, GRID_COUNT> grid;

    void updateDeltaTime() {
        dt = deltaClock.restart();
        deltaTime = dt.asSeconds();
    }

    void updateFPS() {
        sf::Time currentTime = FPSClock.getElapsedTime();
        float fps = floor(1.0f / (currentTime.asSeconds() - lastTime.asSeconds()));
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
        for (size_t i {0}; i < BOID_COUNT; i++) {
            sf::Vector2f randomDir {static_cast<float>(rand() % 100 + 1), static_cast<float>(rand() % 100 + 1)};
            boids.push_back(
                Boid(
                    {static_cast<float>(rand() % 200 + 1), static_cast<float>(rand() % 200 + 1)},
                    normalize(randomDir)
                )
            );
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
        // seed random
        srand(time(NULL));
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

    void updateGlobalRotation() {
        GlobalRotation += deltaTime * GlobalRotationSpeed;
        if (GlobalRotation >= 180.f) GlobalRotation = GlobalRotation - 360.f;
        else if (GlobalRotation < -180.f) GlobalRotation = 360.f + GlobalRotation;
    }

    void updateBoids() {
        for (size_t i {0}; i < boids.size(); i++) {
            unsigned index {0};
            for (size_t j {0}; j < boids.size(); j++) {
                if (i != j) {
                    sf::Vector2f otherBoidWindowPos = boids.at(j).getPosition();
                    sf::Vector2f currentBoidPos = boids.at(i).getPosition();
                    sf::Vector2f dirToBoid;

                    float lastDistance = INFINITY;

                    sf::Vector2f offsetVecs [] = {
                        sf::Vector2f{0.f, 0.f},
                        sf::Vector2f{200.f, 0.f},
                        sf::Vector2f{-200.f, 0.f},
                        sf::Vector2f{0.f, 200.f},
                        sf::Vector2f{0.f, -200.f}
                    };

                    float distance {0};
                    for (size_t i {0}; i < sizeof(offsetVecs); i++) {
                        distance = getDistanceRaw(currentBoidPos, (otherBoidWindowPos + offsetVecs[i]));
                        if (distance < lastDistance) {
                            dirToBoid = currentBoidPos - (otherBoidWindowPos + offsetVecs[i]);
                            lastDistance = distance;
                        }
                        // std::cout << distance << std::endl;
                        // losing some accuracy for performace here, the less distance checks the better
                        if (distance < 20.f) break;
                    }
        
                    boids.at(i).addRotWeight(dirToBoid, index, distance);

                    if (index == BOID_COUNT-1) std::cout << "issue here" << std::endl;
                    index++;
                }
                float globalRads = GlobalRotation * M_PI/180.f;
                sf::Vector2f globalDir {cos(globalRads), sin(globalRads)};
                boids.at(i).addRotWeight(globalDir, BOID_COUNT-1, 0.f); // distance of zero to be the highest weight
            }
            boids.at(i).normalizeTargetDir();

            boids.at(i).update(deltaTime);
        }
    }

    void update() {
        updateDeltaTime();
        updateGlobalRotation();
        updateBoids();
    }

    // debugging to see grid
    void renderGrid(bool boidBased) {
        sf::RectangleShape gridElement;
        gridElement.setFillColor(sf::Color(255, 0, 0, 50));
        gridElement.setSize(sf::Vector2f{GRID_SIZE - 2.f, GRID_SIZE - 2.f});
        if (!boidBased) {
            for (size_t i {0}; i < grid.size(); i++) { // rows
                for (size_t j {0}; j < grid.at(i).size(); j++) { // elements
                    gridElement.setPosition(sf::Vector2f{j*GRID_SIZE, i*GRID_SIZE});
                    window->draw(gridElement);
                }
            }
        }
        else {
            for (auto& boid : boids) {
                sf::Vector2f pos = boid.getPosition();

                int indexX = floor(pos.x / GRID_SIZE);
                int indexY = floor(pos.y / GRID_SIZE);
                gridElement.setPosition(indexX * GRID_SIZE, indexY * GRID_SIZE);
                window->draw(gridElement);
            }
        }
    }

    void render() {
        window->clear();

            renderGrid(true);

            // rendering boids
            for (auto& boid : boids) {
                boid.render(*window);
            }

            if (displayFPS)
                window->draw(text);

        window->display();
    }
};

#endif