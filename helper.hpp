#ifndef _HELPER_HPP_
#define _HELPER_HPP_

#include <cmath>
#include <SFML/System.hpp>

// credit foobarbaz, me bad at math so v helpful :D
template <class T>
sf::Vector2<T>& normalize(sf::Vector2<T>& vector2) {
        float length = (float)(vector2.x*vector2.x + vector2.y*vector2.y);
        if (length == 0.f)
                return vector2;
        length = std::sqrt( 1.f/length );
        vector2.x = (T)(vector2.x * length);
        vector2.y = (T)(vector2.y * length);
        return vector2;
}

template <class T>
float getDot(sf::Vector2<T>& a, sf::Vector2<T>& b) {
        return a.x*b.x + a.y*b.y;
}

template <class T>
float getDistance(const sf::Vector2<T>& p1, const sf::Vector2<T>& p2) {
    float diffY = p1.y - p2.y;
    float diffX = p1.x - p2.x;
    return sqrt((diffY * diffY) + (diffX * diffX));
}

#endif