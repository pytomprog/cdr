#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

#include "parameters.hpp"
#include "Vec2f.hpp"
#include "LineSegment.hpp"

int main() {

    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Game");

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }

        LineSegment testSegment(Vec2f(100.f, 1000.f), Vec2f(2900.f, 1000.f));

        window.clear();
        testSegment.m_p1.draw(window);
        testSegment.m_p2.draw(window);
        testSegment.draw(window);
        window.display();
    }

    return 0;
}