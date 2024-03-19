#include <iostream>
#include <vector>
#include <array>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System/Clock.hpp>
#include <imgui.h>
#include <imgui-SFML.h>

#include "parameters.hpp"
#include "Vec2f.hpp"
#include "Line.hpp"
#include "LineSegment.hpp"
#include "Circle.hpp"
#include "pathfinder.hpp"

struct Color4f
{
    // [0-255]
    float r,g,b,a;
};

int main() {
    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Pathfinding");
    window.setFramerateLimit(60);
    ImGui::SFML::Init(window);

    Vec2f startPoint(100.f, 1000.f, sf::Color::Green), endPoint(2900.f, 1000.f, sf::Color::Red);

    std::vector<Circle> obstacles{ Circle(Vec2f(1000.f, 1100.f, sf::Color::Magenta), 400.f), Circle(Vec2f(2000.f, 1100.f, sf::Color::Magenta), 400.f), Circle(Vec2f(1500.f, 650.f, sf::Color::Magenta), 100.f) };
	Color4f obstaclesColor{1.f, 0.f, 1.f, 1.f};

    std::vector<LineSegment> path = findPath(startPoint, endPoint, obstacles);
    
    sf::Clock deltaClock;
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
			ImGui::SFML::ProcessEvent(window, event);
			
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }
        
        ImGui::SFML::Update(window, deltaClock.restart());

        ImGui::ShowDemoWindow();
        ImGui::Begin("My tools");
        ImGui::ColorEdit4("Obstacles Color", &obstaclesColor.r);
        ImGui::SliderFloat("Start point X", &startPoint.m_x, 0.f, 3000.f);
        ImGui::SliderFloat("Start point Y", &startPoint.m_y, 0.f, 3000.f);
        ImGui::SliderFloat("End point X", &endPoint.m_x, 0.f, 3000.f);
        ImGui::SliderFloat("End point Y", &endPoint.m_y, 0.f, 3000.f);
        if (ImGui::Button("Recalculate path"))
		{
			path = findPath(startPoint, endPoint, obstacles);
		}
        ImGui::End();
        
        startPoint.updatePosition();
        endPoint.updatePosition();

        window.clear();
        for (Circle obstacle : obstacles) {
			obstacle.m_shape.setOutlineColor(sf::Color(uint8_t(255*obstaclesColor.r), uint8_t(255*obstaclesColor.g), uint8_t(255*obstaclesColor.b), uint8_t(255*obstaclesColor.a)));
            obstacle.draw(window);
        }
        for (LineSegment segment : path) {
            segment.draw(window);
        }
        for (Circle obstacle : obstacles) {
			obstacle.m_center.m_shape.setFillColor(sf::Color(uint8_t(255*obstaclesColor.r), uint8_t(255*obstaclesColor.g), uint8_t(255*obstaclesColor.b), uint8_t(255*obstaclesColor.a)));
            obstacle.m_center.draw(window);
        }
        startPoint.draw(window);
        for (LineSegment segment : path) {
            segment.m_p2.draw(window);
        }
        endPoint.draw(window);
		ImGui::SFML::Render(window);
        window.display();
    }

	ImGui::SFML::Shutdown();

    return 0;
}
