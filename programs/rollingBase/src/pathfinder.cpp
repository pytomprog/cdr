#include "pathfinder.hpp"

#include <iostream>
#include <vector>
#include <array>

#include "parameters.hpp"

std::vector<LineSegment> findPath(Vec2f startPoint, Vec2f endPoint, std::vector<Circle> obstacles) {
    std::vector<LineSegment> path{ LineSegment(startPoint, endPoint)};
    std::vector<LineSegment> newPath(path);
    unsigned int step = 0;
    bool pathValid = false;
    while (!pathValid && step < 100) {
        pathValid = true;
        path = newPath;
        newPath.clear();
        std::cout << "================ Step " << step << "================" << std::endl;

        for (int segmentIndex = 0; segmentIndex < path.size(); segmentIndex++) {
            LineSegment segment = path[segmentIndex];
            Line lineOfSegment(segment);

            bool segmentValid = true;
            for (Circle obstacle : obstacles) {
                Line perpendicularLine(lineOfSegment.getPerpendicularLineSegment(obstacle.m_center));

                Vec2f pathCollisionPoint = getCollisionPoint(lineOfSegment, perpendicularLine);

                // First check if the collision point belongs to the segment and after check if circle collides with segment
                // TODO: the collision point can do not belong to the segment and be close enough of a segment vertex, so that should trigger collision, but that's not the case
                if ((LineSegment(segment.m_p1, obstacle.m_center).m_direction.getNorm() + LINE_SEGMENT_CIRCLE_COLLISION_DISTANCE_THRESHOLD < obstacle.m_radius) ||
                    (LineSegment(segment.m_p2, obstacle.m_center).m_direction.getNorm() + LINE_SEGMENT_CIRCLE_COLLISION_DISTANCE_THRESHOLD < obstacle.m_radius)) {
                    std::cerr << "Error: a segment vertex is in a circle, fix this" << std::endl;
                }
                if (segment.containsPoint(pathCollisionPoint) && LineSegment(pathCollisionPoint, obstacle.m_center).m_direction.getNorm() + LINE_SEGMENT_CIRCLE_COLLISION_DISTANCE_THRESHOLD < obstacle.m_radius) {
                    std::cout << "Collision with obstacle" << std::endl;
                    pathValid = false;
                    segmentValid = false;

                    // TODO: keep only one element, the element 0 at first and later implement an algo to choose the best one 
                    //       (choose with new path distance and with the obstacle velocity) 
                    std::array<Vec2f, 2> p1TangentPoints(obstacle.getPointsOfTangentLines(segment.m_p1));
                    std::array<Vec2f, 2> p2TangentPoints(obstacle.getPointsOfTangentLines(segment.m_p2));
                    std::array<Line, 2> p1TangentLines{ {
                            Line(p1TangentPoints[0] - segment.m_p1, segment.m_p1),
                            Line(p1TangentPoints[1] - segment.m_p1, segment.m_p1) } };
                    std::array<Line, 2> p2TangentLines{ {
                            Line(p2TangentPoints[0] - segment.m_p2, segment.m_p2),
                            Line(p2TangentPoints[1] - segment.m_p2, segment.m_p2) } };
                    std::array<Vec2f, 2> collisionPoints{ {
                            getCollisionPoint(p1TangentLines[0], p2TangentLines[1]),
                            getCollisionPoint(p1TangentLines[1], p2TangentLines[0]) } };

                    LineSegment testSegment1_1(segment.m_p1, collisionPoints[0]), testSegment1_2(collisionPoints[0], segment.m_p2);
                    LineSegment testSegment2_1(segment.m_p1, collisionPoints[1]), testSegment2_2(collisionPoints[1], segment.m_p2);

                    if (testSegment1_1.m_direction.getNorm() + testSegment1_2.m_direction.getNorm() <= testSegment2_1.m_direction.getNorm() + testSegment2_2.m_direction.getNorm()) {
                        newPath.push_back(testSegment1_1);
                        newPath.push_back(testSegment1_2);
                    } else {
                        newPath.push_back(testSegment2_1);
                        newPath.push_back(testSegment2_2);
                    }

                    break;
                }
            }

            if (segmentValid) {
                newPath.push_back(segment);
            } else {
                if (segmentIndex != path.size() - 1) {
                    std::cout << "Not the last segment so add last segments to new path" << std::endl;
                    for (segmentIndex += 1; segmentIndex < path.size(); segmentIndex++) {
                        newPath.push_back(path[segmentIndex]);
                    }
                }
                break;
            }
        }
        step += 1;
    }

    return path;
}
