#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System/Clock.hpp>

#include "world/World.hpp"

class GraphicalUserInterface {
public:
	sf::RenderWindow& m_window;
	World& m_world;

	GraphicalUserInterface(sf::RenderWindow& window, World& world);

	void ownRobotPeriphericalsConfigRoutine();
	void ownRobotCameraRoutine();
	void ownRobotRollingBaseRoutine();
	void ownRobotArmRoutine();

	void update();
};