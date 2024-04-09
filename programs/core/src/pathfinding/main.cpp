#include <iostream>
#include <vector>
#include <array>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System/Clock.hpp>
#include <imgui.h>
#include <imgui-SFML.h>

#include "parameters.hpp"
#include "maths/Vec2f.hpp"
#include "maths/Line.hpp"
#include "maths/LineSegment.hpp"
#include "maths/Circle.hpp"
#include "pathfinding/pathfinder.hpp"
#include "pathfinding/DebugGui.hpp"

int main() {
    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Pathfinding");
    ImGui::SFML::Init(window);
    
    DebugGui debugGui;

    float robotRadius = 150.f;

    Vec2f startPoint(100.f, 1000.f, sf::Color::Green), endPoint(2900.f, 1000.f, sf::Color::Red);

    std::vector<Circle> obstacles{ 
        Circle(Vec2f(35.f, 612.5f, sf::Color::Magenta), 125.f, robotRadius),
        Circle(Vec2f(35.f, 1387.5f, sf::Color::Magenta), 125.f, robotRadius),
        Circle(Vec2f(2965.f, 612.5f, sf::Color::Magenta), 125.f, robotRadius),
        Circle(Vec2f(2965.f, 1387.5f, sf::Color::Magenta), 125.f, robotRadius),

        Circle(Vec2f(1000.f, 35.f, sf::Color::Magenta), 125.f, robotRadius),
        Circle(Vec2f(2000.f, 35.f, sf::Color::Magenta), 125.f, robotRadius),

        Circle(Vec2f(1500.f, 500.f, sf::Color::Magenta), 125.f, robotRadius),
        Circle(Vec2f(1500.f, 1500.f, sf::Color::Magenta), 125.f, robotRadius),
        Circle(Vec2f(1000.f, 700.f, sf::Color::Magenta), 125.f, robotRadius),
        Circle(Vec2f(2000.f, 700.f, sf::Color::Magenta), 125.f, robotRadius),
        Circle(Vec2f(1000.f, 1300.f, sf::Color::Magenta), 125.f, robotRadius),
        Circle(Vec2f(2000.f, 1300.f, sf::Color::Magenta), 125.f, robotRadius),
    };

    std::vector<LineSegment> path = findPath(startPoint, endPoint, obstacles);
    
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
			ImGui::SFML::ProcessEvent(window, event);
			
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }
        
        debugGui.update(window, startPoint, endPoint, robotRadius, path, obstacles);

		for (Circle& obstacle : obstacles) {
			obstacle.update(robotRadius);
        }
		startPoint.updatePosition();
		endPoint.updatePosition();

        window.clear();
        for (Circle& obstacle : obstacles) {
			obstacle.m_selfShape.setFillColor(sf::Color(uint8_t(255*debugGui.m_obstaclesColor.r), uint8_t(255*debugGui.m_obstaclesColor.g), uint8_t(255*debugGui.m_obstaclesColor.b), uint8_t(255*debugGui.m_obstaclesColor.a)));
			obstacle.m_collisionShape.setOutlineColor(sf::Color(uint8_t(255*debugGui.m_obstaclesColor.r), uint8_t(255*debugGui.m_obstaclesColor.g), uint8_t(255*debugGui.m_obstaclesColor.b), uint8_t(255*debugGui.m_obstaclesColor.a)));
            obstacle.draw(window);
        }
        for (LineSegment& segment : path) {
            segment.draw(window);
        }
        for (Circle& obstacle : obstacles) {
			obstacle.m_center.m_shape.setFillColor(sf::Color(uint8_t(255*debugGui.m_obstaclesColor.r), uint8_t(255*debugGui.m_obstaclesColor.g), uint8_t(255*debugGui.m_obstaclesColor.b), uint8_t(255*debugGui.m_obstaclesColor.a)));
            obstacle.m_center.draw(window);
        }
        startPoint.draw(window);
        for (LineSegment& segment : path) {
            segment.m_p2.draw(window);
        }
        endPoint.draw(window);
		ImGui::SFML::Render(window);
        window.display();
    }

	ImGui::SFML::Shutdown();

    return 0;
}
