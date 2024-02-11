#ifdef WIN32
    #include <windows.h>
#endif

#include <memory>

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

class Engine {
private:
    std::shared_ptr<sf::RenderWindow> window;
    sf::Event ev;

    // Window moving logic
    float lastDownX {};
    float lastDownY {};
    bool isMouseDragging {false};

    sf::Clock deltaClock;
    sf::Time dt;
    float deltaTime;


    void updateDeltaTime() {
        dt = deltaClock.restart();
        deltaTime = dt.asSeconds();
    }

    void initWindow() {
        window = std::make_shared<sf::RenderWindow>(sf::VideoMode(200,200), "Boid Life", sf::Style::None);
        window->setFramerateLimit(240);
        #ifdef WIN32
            HWND hwnd = window->getSystemHandle();
            SetWindowPos(hwnd, HWND_TOPMOST, 0,0,0,0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
        #endif
    }

public:
    Engine() {
        initWindow();
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
    }

    void render() {
        window->clear();

            // rendering boids

        window->display();
    }
};
