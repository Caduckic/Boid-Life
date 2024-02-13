#ifndef _CONFIG_HPP_
#define _CONFIG_HPP_

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

const sf::Vector2i WINDOW_SIZE {200, 200};

const sf::Vector2f BOID_POINTS [] = {sf::Vector2f(0.f, 0.f), sf::Vector2f(0.f, 10.f), sf::Vector2f(15.f, 5.f)};

const unsigned BOID_COUNT {100};

#endif