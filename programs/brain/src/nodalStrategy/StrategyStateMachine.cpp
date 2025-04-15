#include "nodalStrategy/StrategyStateMachine.hpp"
#include "nodalStrategy/StrategyStates.hpp"

StrategyState::StrategyState(StrategyStateMachine* stateMachinePtr) {
	m_stateMachinePtr = stateMachinePtr;
}

/*StrategyState::StrategyState(StrategyState&& otherState) {
	//m_data = otherState.m_data;
	//otherState.m_data = 0;
}

StrategyState::~StrategyState() {
	//m_data = 1;
}*/

void StrategyStateMachine::addState(std::shared_ptr<StrategyState> newStatePtr) {
	m_statePtrs.push_back(std::move(newStatePtr));
}

void StrategyStateMachine::setNextState(std::shared_ptr<StrategyState> nextStatePtr) {
	m_nextStatePtr = nextStatePtr;
}

unsigned int StrategyStateMachine::getStepsOnSameState() {
	return m_stepsOnSameState;
}

void StrategyStateMachine::routine() {
	if (m_nextStatePtr != m_currentStatePtr) {
		if (m_nextStatePtr == nullptr) {
			std::cerr << "Error: The next state pointer is nullptr !" << std::endl;
		}
		
		m_currentStatePtr = m_nextStatePtr;
		m_stepsOnSameState = 0;
		m_currentStatePtr->startRoutine();
	}
	m_currentStatePtr->loopRoutine();
	m_stepsOnSameState++;
}

void StrategyStateMachine::saveStatesToYaml(const std::string& filename) {
	YAML::Node rootNode;
	rootNode["states"] = YAML::Node(YAML::NodeType::Sequence);
	for (const auto& statePtr : m_statePtrs) {
		rootNode["states"].push_back(statePtr->serializeToYaml());
	}
	std::ofstream fout(filename);
	fout << rootNode;
	std::cout << "States saved to " << filename << std::endl;
}

void StrategyStateMachine::loadStatesFromYaml(const std::string& filename) {
	YAML::Node rootNode = YAML::LoadFile(filename);
	m_statePtrs.clear();  // Remove any previous states

	if (rootNode["states"]) {
		for (const auto& node : rootNode["states"]) {
			std::string type = node["type"].as<std::string>();
			if (type == "IdleState") {
				addState(IdleState::createFromYaml(this, node));
			} else if (type == "TestState") {
				addState(TestState::createFromYaml(this, node));
			} else if (type == "TestState2") {
				addState(TestState2::createFromYaml(this, node));
			} else {
				std::cerr << "Unknown state type: " << type << std::endl;
			}
		}
	}
	std::cout << "States loaded from " << filename << std::endl;
}
