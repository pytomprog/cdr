#include "nodalStrategy/StrategyStates.hpp"

void IdleState::startRoutine() {
	std::cout << "Entering in idle state" << std::endl;
}

void IdleState::loopRoutine() {}

std::string IdleState::getTypeString() { return "IdleState"; }
	
YAML::Node IdleState::serializeToYaml() {
	YAML::Node node;
	node["type"] = getTypeString();
	return node;
}

std::unique_ptr<StrategyState> IdleState::createFromYaml(StrategyStateMachine* stateMachinePtr, const YAML::Node& node) {
	return std::make_unique<IdleState>(stateMachinePtr);
}

    

void TestState::startRoutine() {
	std::cout << "a" << std::endl;
}

void TestState::loopRoutine() {
	std::cout << "b" << std::endl;
	
	if (m_stateMachinePtr->getStepsOnSameState() >= 5) {
		m_stateMachinePtr->setNextState(m_nextStatePtr);
	}
}

std::string TestState::getTypeString() { return "TestState"; }
	
YAML::Node TestState::serializeToYaml() {
	YAML::Node node;
	node["type"] = getTypeString();
	return node;
}

std::unique_ptr<StrategyState> TestState::createFromYaml(StrategyStateMachine* stateMachinePtr, const YAML::Node& node) {
	return std::make_unique<TestState>(stateMachinePtr);
}



void TestState2::startRoutine() {
	std::cout << "c" << std::endl;
}

void TestState2::loopRoutine() {
	std::cout << "d" << std::endl;
	
	if (m_stateMachinePtr->getStepsOnSameState() >= 5) {
		m_stateMachinePtr->setNextState(m_nextStatePtr);
	}
}

std::string TestState2::getTypeString() { return "TestState2"; }
	
YAML::Node TestState2::serializeToYaml() {
	YAML::Node node;
	node["type"] = getTypeString();
	return node;
}

std::unique_ptr<StrategyState> TestState2::createFromYaml(StrategyStateMachine* stateMachinePtr, const YAML::Node& node) {
	return std::make_unique<TestState2>(stateMachinePtr);
}
