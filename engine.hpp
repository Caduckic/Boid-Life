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

    std::vector<std::shared_ptr<Boid>> boids;
    float GlobalRotation {0.f};
    float GlobalRotationSpeed {5.f};

    std::array<std::array<std::vector<std::shared_ptr<Boid>>, GRID_COUNT>, GRID_COUNT> grid;

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
            float x = static_cast<float>(rand() % 200 + 1);
            float y = static_cast<float>(rand() % 200 + 1);
            
            // easier than going to full cpp randomness, jeez why can't things just be easy
            if (x > 100.f) x -= 200.f;
            if (y > 100.f) y -= 200.f;
            sf::Vector2f randomDir {x, y};
            randomDir = normalize(randomDir);
            boids.push_back(
                std::make_shared<Boid>(
                    sf::Vector2f{static_cast<float>(rand() % 200 + 1), static_cast<float>(rand() % 200 + 1)},
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

    void updateGrid() {
        for (auto& row : grid) {
            for (auto& element : row) {
                element.clear();
            }
        }
        for (std::shared_ptr<Boid> boid : boids) {
            sf::Vector2f pos = boid->getPosition();

            int indexX = floor(pos.x / GRID_SIZE);
            if (indexX > GRID_COUNT - 1) indexX = GRID_COUNT - 1;
            else if (indexX < 0) indexX = 0;
            int indexY = floor(pos.y / GRID_SIZE);
            if (indexY > GRID_COUNT - 1) indexY = GRID_COUNT - 1;
            else if (indexY < 0) indexY = 0;

            // auto it = std::find(grid.at(indexY).at(indexX).begin(), grid.at(indexY).at(indexX).end(), boid);
            // if (it == grid.at(indexY).at(indexX).end()) {
                grid.at(indexY).at(indexX).push_back(boid);
            // }
        }
    }

    void updateBoids() {
        for (size_t i {0}; i < boids.size(); i++) {
            sf::Vector2f pos = boids.at(i)->getPosition();

            // get grid position
            int indexX = floor(pos.x / GRID_SIZE);
            if (indexX > GRID_COUNT - 1) indexX = GRID_COUNT - 1;
            else if (indexX < 0) indexX = 0;
            int indexY = floor(pos.y / GRID_SIZE);
            if (indexY > GRID_COUNT - 1) indexY = GRID_COUNT - 1;
            else if (indexY < 0) indexY = 0;
            
            // get nearby grid elements
            int left = (indexX - 1 < 0) ? GRID_COUNT - 1 : indexX - 1;
            int top = (indexY - 1 < 0) ? GRID_COUNT - 1 : indexY - 1;
            int right = (indexX + 1 > GRID_COUNT - 1) ? 0 : indexX + 1;
            int bottom = (indexY + 1 > GRID_COUNT - 1) ? 0 : indexY + 1;

            sf::Vector2i nearbyElements [] = {
                {left, top},
                {indexX, top},
                {right, top},
                {left, indexY},
                {right, indexY},
                {left, bottom},
                {indexX, bottom},
                {right, bottom}
            };

            for (size_t nE {0}; nE < sizeof(nearbyElements) / sizeof(sf::Vector2i); nE++) {
                for (size_t j {0}; j < grid.at(nearbyElements[nE].y).at(nearbyElements[nE].x).size(); j++){
                    std::shared_ptr<Boid>& otherBoid = grid.at(nearbyElements[nE].y).at(nearbyElements[nE].x).at(j);
                    if (boids.at(i).get() != otherBoid.get()) {
                        sf::Vector2f otherBoidWindowPos = otherBoid->getPosition();
                        sf::Vector2f currentBoidPos = boids.at(i)->getPosition();
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
                        for (size_t k {0}; k < sizeof(offsetVecs) / sizeof(sf::Vector2f); k++) {
                            distance = getDistanceRaw(currentBoidPos, (otherBoidWindowPos + offsetVecs[k]));
                            if (distance < lastDistance) {
                                dirToBoid = currentBoidPos - (otherBoidWindowPos + offsetVecs[k]);
                                lastDistance = distance;
                            }
                        }
                        boids.at(i)->addRotWeight(dirToBoid, lastDistance);
                    }
                }
            }
            float globalRads = GlobalRotation * M_PI/180.f;
            sf::Vector2f globalDir {cos(globalRads), sin(globalRads)};
            boids.at(i)->addRotWeight(globalDir, 30.f); // adds a global weight all boids follow
            boids.at(i)->normalizeTargetDir();

            boids.at(i)->update(deltaTime);
        }
    }

    void update() {
        updateDeltaTime();
        updateGlobalRotation();
        updateGrid();
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
                    gridElement.setPosition(sf::Vector2f{static_cast<float>(j*GRID_SIZE), static_cast<float>(i*GRID_SIZE)});
                    window->draw(gridElement);
                }
            }
        }
        else {
            for (auto& boid : boids) {
                sf::Vector2f pos = boid->getPosition();

                int indexX = floor(pos.x / GRID_SIZE);
                if (indexX > GRID_COUNT - 1) indexX = GRID_COUNT - 1;
                else if (indexX < 0) indexX = 0;
                int indexY = floor(pos.y / GRID_SIZE);
                if (indexY > GRID_COUNT - 1) indexY = GRID_COUNT - 1;
                else if (indexY < 0) indexY = 0;
                gridElement.setPosition(indexX * GRID_SIZE, indexY * GRID_SIZE);
                window->draw(gridElement);
                
                int left = (indexX - 1 < 0) ? GRID_COUNT - 1 : indexX - 1;
                int top = (indexY - 1 < 0) ? GRID_COUNT - 1 : indexY - 1;
                int right = (indexX + 1 > GRID_COUNT - 1) ? 0 : indexX + 1;
                int bottom = (indexY + 1 > GRID_COUNT - 1) ? 0 : indexY + 1;

                sf::Vector2i nearbyElements [] = {
                    {left, top},
                    {indexX, top},
                    {right, top},
                    {left, indexY},
                    {right, indexY},
                    {left, bottom},
                    {indexX, bottom},
                    {right, bottom}
                };

                for (size_t i {0}; i < sizeof(nearbyElements) / sizeof(sf::Vector2i); i++) {
                    sf::RectangleShape s;
                    s.setFillColor(sf::Color(255, 0, 0, 50));
                    s.setSize(sf::Vector2f{GRID_SIZE - 2.f, GRID_SIZE - 2.f});
                    s.setPosition(nearbyElements[i].x * GRID_SIZE, nearbyElements[i].y * GRID_SIZE);
                    window->draw(s);
                }
            }
        }
    }

    void render() {
        window->clear();

            // renderGrid(true);

            // rendering boids
            for (auto& boid : boids) {
                boid->render(*window);
            }

            if (displayFPS)
                window->draw(text);

        window->display();
    }
};

#endif