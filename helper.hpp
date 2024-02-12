#ifndef _HELPER_HPP_
#define _HELPER_HPP_

#include <cmath>
#include <SFML/System.hpp>

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

#endif