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

StrategyStateMachine::StrategyStateMachine(World& world): m_world(world) {}

void StrategyStateMachine::addState(std::shared_ptr<StrategyState> newStatePtr) {
	m_statePtrs.push_back(std::move(newStatePtr));
}

void StrategyStateMachine::setNextState(std::shared_ptr<StrategyState> nextStatePtr) {
	m_nextStatePtr = nextStatePtr;
}

unsigned int StrategyStateMachine::getStateIndexByPtrValue(std::shared_ptr<StrategyState> statePtr) {
	auto it = std::find(m_statePtrs.begin(), m_statePtrs.end(), statePtr);
	if (it == m_statePtrs.end()) {
		std::cerr << "State with pointer value " << statePtr << " isn't in the state machine states list !" << std::endl;
		return 0;
	} else {
		return std::distance(m_statePtrs.begin(), it);
	}
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

unsigned int StrategyStateMachine::getStepsOnSameState() {
	return m_stepsOnSameState;
}

void StrategyStateMachine::saveStatesToYaml(const std::string& filename) {
	YAML::Node rootNode;
	rootNode["states"] = YAML::Node(YAML::NodeType::Sequence);
	for (size_t stateIndex = 0; stateIndex < m_statePtrs.size(); stateIndex++) {
		const auto& statePtr = m_statePtrs[stateIndex];
		rootNode["states"].push_back(statePtr->serializeToYaml(stateIndex));
	}
	std::ofstream fout(filename);
	fout << rootNode;
	std::cout << "States saved to " << filename << std::endl;
}

void StrategyStateMachine::loadStatesFromYaml(const std::string& filename) {
	YAML::Node rootNode = YAML::LoadFile(filename);
	m_statePtrs.clear();  // Remove any previous states
	m_stateIdsToPtrs.clear();
	
	if (rootNode["states"]) {
		for (const auto& node : rootNode["states"]) {
			std::string type = node["type"].as<std::string>();
			std::shared_ptr<StrategyState> newStatePtr;
			if (type == "IdleState") {
				newStatePtr = IdleState::createFromYaml(this, node);
			} else if (type == "TestState") {
				newStatePtr = TestState::createFromYaml(this, node);
			} else if (type == "StartState") {
				newStatePtr = StartState::createFromYaml(this, node);
			} else if (type == "TiretteState") {
				newStatePtr = TiretteState::createFromYaml(this, node);
			} else if (type == "MovetoState") {
				newStatePtr = MovetoState::createFromYaml(this, node);
			} else if (type == "MoveThroughState") {
				newStatePtr = MoveThroughState::createFromYaml(this, node);
			} else if (type == "ChangeVacuumStateState") {
				newStatePtr = ChangeVacuumStateState::createFromYaml(this, node);
			} else if (type == "SetStepperPosState") {
				newStatePtr = SetStepperPosState::createFromYaml(this, node);
			} else if (type == "SetServoPosState") {
				newStatePtr = SetServoPosState::createFromYaml(this, node);
			} else if (type == "WaitState") {
				newStatePtr = WaitState::createFromYaml(this, node);
			} else {
				std::cerr << "Unknown state type: " << type << std::endl;
				continue;
			}
			addState(newStatePtr);
			unsigned int newStateIdInYaml = node["id"].as<unsigned int>();
			m_stateIdsToPtrs[newStateIdInYaml] = newStatePtr;
		}
		
		for (const auto& statePtr: m_statePtrs) {
			statePtr->updateOutputLinks();
		}
		
		std::cout << "States loaded from " << filename << std::endl;
	}
}
