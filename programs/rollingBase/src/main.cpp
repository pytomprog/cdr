#include <iostream>
#include <vector>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

#include "parameters.hpp"
#include "Vec2f.hpp"
#include "Line.hpp"
#include "LineSegment.hpp"
#include "Circle.hpp"

int main() {

    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Game");

    while (window.isOpen()) {
        Vec2f startPoint(100.f, 1000.f, sf::Color::Green), endPoint(2900.f, 1000.f, sf::Color::Red);

        Circle obstacle(Vec2f(1500.f, 1100.f), 200.f);

        std::vector<LineSegment> path{ LineSegment(startPoint, endPoint) };
        
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }

        window.clear();

        obstacle.draw(window);
        for (LineSegment segment : path) {
            segment.draw(window);
        }

        std::vector<LineSegment> newPath(path);
        bool pathValid = false;
        while (!pathValid) {
            pathValid = true;
            path = newPath;
            newPath.clear();

            for (LineSegment segment : path) {
                Line lineOfSegment(segment);
                Line perpendicularLine(lineOfSegment.getPerpendicularLineSegment(obstacle.m_center));

                Vec2f collisionPoint = getCollisionPoint(lineOfSegment, perpendicularLine);
                // First check if the collision point belongs to the segment and after check if 
                // TODO: the collision point can do not belong to the segment and be close enough of a segment vertex, so that should trigger collision, but that's not the case 
                if (segment.containsPoint(collisionPoint) && LineSegment(collisionPoint, obstacle.m_center).m_direction.getNorm() < obstacle.m_radius) {
                    std::cout << "Collision with obstacle" << std::endl;
                    pathValid = false;


                    // TODO: keep only one element, the element 0 at first and later implement an algo to choose the best one 
                    //       (choose with new path distance and with the obstacle velocity) 
                    std::vector<Vec2f> tangentSegmentsPoints(obstacle.getPointsOfTangentLines(segment.m_p1));
                    std::vector<LineSegment> tangentSegments;
                    for (Vec2f p : tangentSegmentsPoints) {
                        tangentSegments.push_back(LineSegment(segment.m_p1, p));
                    }
                    for (LineSegment s : tangentSegments) {
                        s.draw(window);
                    }
                    for (Vec2f p : tangentSegmentsPoints) {
                        p.draw(window);
                    }
                } else {
                    newPath.push_back(segment);
                }

                perpendicularLine.draw(window);
                collisionPoint.draw(window);
            }
        }

        obstacle.m_center.draw(window);
        startPoint.draw(window);
        for (LineSegment segment : path) {
            segment.m_p2.draw(window); 
        }
        endPoint.draw(window);

        window.display();
    }

    return 0;
}