#include <iostream>
#include <vector>
#include <array>
#include <string>
#include <memory>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System/Clock.hpp>
#include <imgui.h>
#include <imgui-SFML.h>

#include "parameters.hpp"
#include "nodalStrategy/DebugGui.hpp"
#include "nodalStrategy/StrategyStateMachine.hpp"
#include "nodalStrategy/StrategyStates.hpp"

int main() {
	/*sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Nodal strategy");
	ImGui::SFML::Init(window);
	
	DebugGui debugGui;
	
	while (window.isOpen()) {
		sf::Event event;
		while (window.pollEvent(event)) {
			ImGui::SFML::ProcessEvent(window, event);
			if (event.type == sf::Event::Closed) {
				window.close();
			}
		}
		
		debugGui.update(window);
		
		window.clear();
		ImGui::SFML::Render(window);
		window.display();
	}
	
	ImGui::SFML::Shutdown();*/
	
	StrategyStateMachine stateMachine;
	
	/*stateMachine.addState(std::make_shared<TestState>(&stateMachine));
	stateMachine.addState(std::make_shared<TestState2>(&stateMachine));
	stateMachine.addState(std::make_shared<IdleState>(&stateMachine));
	
	std::static_pointer_cast<TestState>(stateMachine.m_statePtrs[0])->m_nextStatePtr = stateMachine.m_statePtrs[1];
	std::static_pointer_cast<TestState2>(stateMachine.m_statePtrs[1])->m_nextStatePtr = stateMachine.m_statePtrs[2];
	
	stateMachine.saveStatesToYaml("testStrategy.yaml");*/
	
	stateMachine.loadStatesFromYaml("testStrategy.yaml");
	/*std::cout << "States: " << stateMachine.m_statePtrs.size() << std::endl;
	for (size_t stateId = 0; stateId < stateMachine.m_statePtrs.size(); stateId++)
		std::cout << "State " << stateId << " type: " << stateMachine.m_statePtrs[stateId]->getTypeString() << std::endl;*/
	
	stateMachine.setNextState(stateMachine.m_statePtrs[0]);
	while (1) {
		stateMachine.routine();
	}
		
	return 0;
}
