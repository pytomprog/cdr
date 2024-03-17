#include <iostream>
#include <vector>
#include <array>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

#include "parameters.hpp"
#include "Vec2f.hpp"
#include "Line.hpp"
#include "LineSegment.hpp"
#include "Circle.hpp"
#include "pathfinder.hpp"

int main() {
    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Game");

    Vec2f startPoint(100.f, 1000.f, sf::Color::Green), endPoint(2900.f, 1000.f, sf::Color::Red);

    std::vector<Circle> obstacles{ Circle(Vec2f(1000.f, 1100.f, sf::Color::Magenta), 400.f), Circle(Vec2f(2000.f, 1100.f, sf::Color::Magenta), 400.f), Circle(Vec2f(1500.f, 650.f, sf::Color::Magenta), 100.f) };

    std::vector<LineSegment> path = findPath(startPoint, endPoint, obstacles);

    window.display();
    
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }

        window.clear();

        for (Circle obstacle : obstacles) {
            obstacle.draw(window);
        }
        for (LineSegment segment : path) {
            segment.draw(window);
        }
        for (Circle obstacle : obstacles) {
            obstacle.m_center.draw(window);
        }
        startPoint.draw(window);
        for (LineSegment segment : path) {
            segment.m_p2.draw(window);
        }
        endPoint.draw(window);

        window.display();
    }

    return 0;
}