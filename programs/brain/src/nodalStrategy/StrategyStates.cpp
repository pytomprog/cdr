#include "nodalStrategy/StrategyStates.hpp"

// ======================================= Idle state ======================================= 

void IdleState::startRoutine() {
	std::cout << "Entering in idle state" << std::endl;
}

void IdleState::loopRoutine() {}

std::string IdleState::getTypeString() { return "IdleState"; }
	
YAML::Node IdleState::serializeToYaml(unsigned int stateIndex) {
	YAML::Node node;
	node["id"] = stateIndex;
	node["type"] = getTypeString();
	YAML::Node parametersNode = node["parameters"];
	return node;
}

std::unique_ptr<StrategyState> IdleState::createFromYaml(StrategyStateMachine* stateMachinePtr, const YAML::Node& node) {
	//m_stateIdInYaml = node["id"].as<unsigned int>();
	return std::make_unique<IdleState>(stateMachinePtr);
}

void IdleState::updateOutputLinks() {}


// ======================================= Test state ======================================= 

TestState::TestState(StrategyStateMachine* stateMachinePtr, std::string startMessage, std::string loopMessage, unsigned int nextStateIdInYaml): StrategyState(stateMachinePtr) {
	m_startMessage = startMessage;
	m_loopMessage = loopMessage;
	m_nextStateIdInYaml = nextStateIdInYaml;
}    

void TestState::startRoutine() {
	std::cout << m_startMessage << std::endl;
}

void TestState::loopRoutine() {
	std::cout << m_loopMessage << std::endl;
	
	if (m_stateMachinePtr->getStepsOnSameState() >= 5) {
		m_stateMachinePtr->setNextState(m_nextStatePtr);
	}
}

std::string TestState::getTypeString() { return "TestState"; }
	
YAML::Node TestState::serializeToYaml(unsigned int stateIndex) {
	YAML::Node node;
	node["id"] = stateIndex;
	node["type"] = getTypeString();
	YAML::Node parametersNode = node["parameters"];
	parametersNode["startMessage"] = m_startMessage;
	parametersNode["loopMessage"] = m_loopMessage;
	parametersNode["nextStateId"] = m_stateMachinePtr->getStateIndexByPtrValue(m_nextStatePtr);
	return node;
}

std::unique_ptr<StrategyState> TestState::createFromYaml(StrategyStateMachine* stateMachinePtr, const YAML::Node& node) {
	//m_stateIdInYaml = node["id"].as<unsigned int>();
	YAML::Node parametersNode = node["parameters"];
	std::string startMessage = parametersNode["startMessage"] ? parametersNode["startMessage"].as<std::string>(): "default start message";
	std::string loopMessage = parametersNode["loopMessage"] ? parametersNode["loopMessage"].as<std::string>(): "default loop message";
	unsigned int nextStateIdInYaml = parametersNode["nextStateId"].as<unsigned int>();
	return std::make_unique<TestState>(stateMachinePtr, startMessage, loopMessage, nextStateIdInYaml);
}

void TestState::updateOutputLinks() {
	m_nextStatePtr = m_stateMachinePtr->m_stateIdsToPtrs[m_nextStateIdInYaml];
}


// ======================================= Test state 2 ======================================= 

TestState2::TestState2(StrategyStateMachine* stateMachinePtr, unsigned int nextStateIdInYaml): StrategyState(stateMachinePtr) {
	m_nextStateIdInYaml = nextStateIdInYaml;
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
	
YAML::Node TestState2::serializeToYaml(unsigned int stateIndex) {
	YAML::Node node;
	node["id"] = stateIndex;
	node["type"] = getTypeString();
	YAML::Node parametersNode = node["parameters"];
	parametersNode["nextStateId"] = m_stateMachinePtr->getStateIndexByPtrValue(m_nextStatePtr);
	return node;
}

std::unique_ptr<StrategyState> TestState2::createFromYaml(StrategyStateMachine* stateMachinePtr, const YAML::Node& node) {
	//m_stateIdInYaml = node["id"].as<unsigned int>();
	YAML::Node parametersNode = node["parameters"];
	unsigned int nextStateIdInYaml = parametersNode["nextStateId"].as<unsigned int>();
	return std::make_unique<TestState2>(stateMachinePtr, nextStateIdInYaml);
}

void TestState2::updateOutputLinks() {
	m_nextStatePtr = m_stateMachinePtr->m_stateIdsToPtrs[m_nextStateIdInYaml];
}
