#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

#include "parameters.hpp"
#include "Vec2f.hpp"
#include "LineSegment.hpp"
#include "Circle.hpp"

int main() {

    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Game");

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }

        Vec2f startPoint(100.f, 1000.f), endPoint(2900.f, 1000.f);

        LineSegment testSegment(startPoint, endPoint);
        Circle testCircle(Vec2f(1500.f, 1100.f), 200.f, sf::Color::Magenta);



        window.clear();

        testCircle.draw(window);
        testSegment.draw(window);

        testCircle.m_center.draw(window);
        startPoint.draw(window);
        endPoint.draw(window);

        window.display();
    }

    return 0;
}