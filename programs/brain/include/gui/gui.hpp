#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System/Clock.hpp>
#include <vector>
#include <string>

#include "parameters.hpp"
#include "world/World.hpp"

class GraphicalUserInterface {
public:
	sf::RenderWindow& m_window;
	World& m_world;
	bool m_gamepadEnabled = false;
	const char* m_strategiesNamesCharPtr[MAX_STRATEGIES_NB];
	std::vector<std::string> m_strategiesNames;

	GraphicalUserInterface(sf::RenderWindow& window, World& world, std::vector<std::string> strategiesNames);

	void homeMenuConfig();
	void periphericalsConfigRoutine();
	void cameraRoutine();
	void ownRobotRollingBaseRoutine();
	void ownRobotArmRoutine();

	void update();
};