#include "nodalStrategy/StrategyStates.hpp"

#include "parameters.hpp"

#include "hal/bcm2835.h"

#include <chrono>

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


// ======================================= Start state ======================================= 

StartState::StartState(StrategyStateMachine* stateMachinePtr, unsigned int nextStateIdInYaml): StrategyState(stateMachinePtr) {
	m_nextStateIdInYaml = nextStateIdInYaml;
}    

void StartState::startRoutine() {
	if (!bcm2835_init())
		return;

	// Set RPI pin P1-15 to be an input
	bcm2835_gpio_fsel(PIN_TIRRETE, BCM2835_GPIO_FSEL_INPT);
	//  with a pullup
	bcm2835_gpio_set_pud(PIN_TIRRETE, BCM2835_GPIO_PUD_UP);
	
	m_stateMachinePtr->m_world.m_ownRobot.m_ownRobotRollingBaseMode = POSE_CONTROL;
	m_stateMachinePtr->m_world.m_ownRobot.m_motorsEnabled = false;
	m_stateMachinePtr->m_world.m_ownRobot.m_motorsEnabled = false;
	
	m_stateMachinePtr->setNextState(m_nextStatePtr);
}

void StartState::loopRoutine() {}

std::string StartState::getTypeString() { return "StartState"; }
	
YAML::Node StartState::serializeToYaml(unsigned int stateIndex) {
	YAML::Node node;
	node["id"] = stateIndex;
	node["type"] = getTypeString();
	YAML::Node parametersNode = node["parameters"];
	parametersNode["nextStateId"] = m_stateMachinePtr->getStateIndexByPtrValue(m_nextStatePtr);
	return node;
}

std::unique_ptr<StrategyState> StartState::createFromYaml(StrategyStateMachine* stateMachinePtr, const YAML::Node& node) {
	//m_stateIdInYaml = node["id"].as<unsigned int>();
	YAML::Node parametersNode = node["parameters"];
	unsigned int nextStateIdInYaml = parametersNode["nextStateId"].as<unsigned int>();
	return std::make_unique<StartState>(stateMachinePtr, nextStateIdInYaml);
}

void StartState::updateOutputLinks() {
	m_nextStatePtr = m_stateMachinePtr->m_stateIdsToPtrs[m_nextStateIdInYaml];
}

// ======================================= Tirette state ======================================= 

TiretteState::TiretteState(StrategyStateMachine* stateMachinePtr, unsigned int nextStateIdInYaml): StrategyState(stateMachinePtr) {
	m_nextStateIdInYaml = nextStateIdInYaml;
}    

void TiretteState::startRoutine() {
	m_tirettePushed = false;
	if (bcm2835_gpio_lev(PIN_TIRRETE)) {
		std::cout << "Waiting for tirette push ..." << std::endl;
	} else {
		m_tirettePushed = true;
		std::cout << "Waiting for tirette pull ..." << std::endl;
	}
}

void TiretteState::loopRoutine() {
	bool value = bcm2835_gpio_lev(PIN_TIRRETE);
	if (!m_tirettePushed && !value) {
		m_tirettePushed = true;
		std::cout << "Waiting for tirette pull ..." << std::endl;
	} else if (m_tirettePushed && value) {
		std::cout << "Match started" << std::endl;
		m_stateMachinePtr->m_world.started = 1;
		m_stateMachinePtr->m_world.startTime = std::chrono::high_resolution_clock::now();
		m_stateMachinePtr->setNextState(m_nextStatePtr);
	}
}

std::string TiretteState::getTypeString() { return "TiretteState"; }
	
YAML::Node TiretteState::serializeToYaml(unsigned int stateIndex) {
	YAML::Node node;
	node["id"] = stateIndex;
	node["type"] = getTypeString();
	YAML::Node parametersNode = node["parameters"];
	parametersNode["nextStateId"] = m_stateMachinePtr->getStateIndexByPtrValue(m_nextStatePtr);
	return node;
}

std::unique_ptr<StrategyState> TiretteState::createFromYaml(StrategyStateMachine* stateMachinePtr, const YAML::Node& node) {
	//m_stateIdInYaml = node["id"].as<unsigned int>();
	YAML::Node parametersNode = node["parameters"];
	unsigned int nextStateIdInYaml = parametersNode["nextStateId"].as<unsigned int>();
	return std::make_unique<TiretteState>(stateMachinePtr, nextStateIdInYaml);
}

void TiretteState::updateOutputLinks() {
	m_nextStatePtr = m_stateMachinePtr->m_stateIdsToPtrs[m_nextStateIdInYaml];
}

// ======================================= Moveto state ======================================= 

MovetoState::MovetoState(StrategyStateMachine* stateMachinePtr, Pose2f targetPose, unsigned int nextStateIdInYaml): StrategyState(stateMachinePtr) {
	m_targetPose = targetPose;
	m_nextStateIdInYaml = nextStateIdInYaml;
}    

void MovetoState::startRoutine() {
	m_stateMachinePtr->m_world.m_ownRobot.m_targetPose = m_targetPose;
    m_stateMachinePtr->m_world.m_ownRobot.m_motorsEnabled = true;
}

void MovetoState::loopRoutine() {
	float xyError = getDistance(m_stateMachinePtr->m_world.m_ownRobot.m_currentPose.m_position, m_stateMachinePtr->m_world.m_ownRobot.m_targetPose.m_position);
	float primaryThetaError = abs(m_stateMachinePtr->m_world.m_ownRobot.m_currentPose.m_orientation - m_stateMachinePtr->m_world.m_ownRobot.m_targetPose.m_orientation);
	float thetaError = std::min(primaryThetaError, TWO_PI - primaryThetaError);
	if (xyError < 20.f && thetaError < 0.08f) {
		m_stateMachinePtr->m_world.m_ownRobot.m_motorsEnabled = false;
		m_stateMachinePtr->setNextState(m_nextStatePtr);
    }
}

std::string MovetoState::getTypeString() { return "MovetoState"; }
	
YAML::Node MovetoState::serializeToYaml(unsigned int stateIndex) {
	YAML::Node node;
	node["id"] = stateIndex;
	node["type"] = getTypeString();
	YAML::Node parametersNode = node["parameters"];
	parametersNode["targetX"] = m_targetPose.m_position.m_x;
	parametersNode["targetY"] = m_targetPose.m_position.m_y;
	parametersNode["targetTheta"] = m_targetPose.m_orientation;
	parametersNode["nextStateId"] = m_stateMachinePtr->getStateIndexByPtrValue(m_nextStatePtr);
	return node;
}

std::unique_ptr<StrategyState> MovetoState::createFromYaml(StrategyStateMachine* stateMachinePtr, const YAML::Node& node) {
	//m_stateIdInYaml = node["id"].as<unsigned int>();
	YAML::Node parametersNode = node["parameters"];
	float targetX = parametersNode["targetX"] ? parametersNode["targetX"].as<float>(): 0.f;
	float targetY = parametersNode["targetY"] ? parametersNode["targetY"].as<float>(): 0.f;
	float targetTheta = parametersNode["targetTheta"] ? parametersNode["targetTheta"].as<float>(): 0.f;
	Pose2f targetPose(Vec2f(targetX, targetY), targetTheta);
	unsigned int nextStateIdInYaml = parametersNode["nextStateId"].as<unsigned int>();
	return std::make_unique<MovetoState>(stateMachinePtr, targetPose, nextStateIdInYaml);
}

void MovetoState::updateOutputLinks() {
	m_nextStatePtr = m_stateMachinePtr->m_stateIdsToPtrs[m_nextStateIdInYaml];
}

// ======================================= MoveThroughState state ======================================= 

MoveThroughState::MoveThroughState(StrategyStateMachine* stateMachinePtr, Pose2f targetPose, unsigned int nextStateIdInYaml): StrategyState(stateMachinePtr) {
	m_targetPose = targetPose;
	m_nextStateIdInYaml = nextStateIdInYaml;
}    

void MoveThroughState::startRoutine() {
	m_stateMachinePtr->m_world.m_ownRobot.m_targetPose = m_targetPose;
    m_stateMachinePtr->m_world.m_ownRobot.m_motorsEnabled = true;
}

void MoveThroughState::loopRoutine() {
	float xyError = getDistance(m_stateMachinePtr->m_world.m_ownRobot.m_currentPose.m_position, m_stateMachinePtr->m_world.m_ownRobot.m_targetPose.m_position);
	float primaryThetaError = abs(m_stateMachinePtr->m_world.m_ownRobot.m_currentPose.m_orientation - m_stateMachinePtr->m_world.m_ownRobot.m_targetPose.m_orientation);
	float thetaError = std::min(primaryThetaError, TWO_PI - primaryThetaError);
	if (xyError < 100.f && thetaError < 0.4f) {
		m_stateMachinePtr->setNextState(m_nextStatePtr);
    }
}

std::string MoveThroughState::getTypeString() { return "MoveThroughState"; }
	
YAML::Node MoveThroughState::serializeToYaml(unsigned int stateIndex) {
	YAML::Node node;
	node["id"] = stateIndex;
	node["type"] = getTypeString();
	YAML::Node parametersNode = node["parameters"];
	parametersNode["targetX"] = m_targetPose.m_position.m_x;
	parametersNode["targetY"] = m_targetPose.m_position.m_y;
	parametersNode["targetTheta"] = m_targetPose.m_orientation;
	parametersNode["nextStateId"] = m_stateMachinePtr->getStateIndexByPtrValue(m_nextStatePtr);
	return node;
}

std::unique_ptr<StrategyState> MoveThroughState::createFromYaml(StrategyStateMachine* stateMachinePtr, const YAML::Node& node) {
	//m_stateIdInYaml = node["id"].as<unsigned int>();
	YAML::Node parametersNode = node["parameters"];
	float targetX = parametersNode["targetX"] ? parametersNode["targetX"].as<float>(): 0.f;
	float targetY = parametersNode["targetY"] ? parametersNode["targetY"].as<float>(): 0.f;
	float targetTheta = parametersNode["targetTheta"] ? parametersNode["targetTheta"].as<float>(): 0.f;
	Pose2f targetPose(Vec2f(targetX, targetY), targetTheta);
	unsigned int nextStateIdInYaml = parametersNode["nextStateId"].as<unsigned int>();
	return std::make_unique<MoveThroughState>(stateMachinePtr, targetPose, nextStateIdInYaml);
}

void MoveThroughState::updateOutputLinks() {
	m_nextStatePtr = m_stateMachinePtr->m_stateIdsToPtrs[m_nextStateIdInYaml];
}

// ======================================= Change vacuum state state ======================================= 

ChangeVacuumStateState::ChangeVacuumStateState(StrategyStateMachine* stateMachinePtr, bool outerVacuumChanging, bool innerVacuumChanging, bool plankVacuumChanging, bool newVacumState, unsigned int nextStateIdInYaml): StrategyState(stateMachinePtr) {
	m_outerVacuumChanging = outerVacuumChanging;
	m_innerVacuumChanging = innerVacuumChanging;
	m_plankVacuumChanging = plankVacuumChanging;
	m_newVacumState = newVacumState;
	m_nextStateIdInYaml = nextStateIdInYaml;
}    

void ChangeVacuumStateState::startRoutine() {}

void ChangeVacuumStateState::loopRoutine() {
	Arm& arm1 = m_stateMachinePtr->m_world.m_ownRobot.m_arm1;
	if (!arm1.m_isCommandToSend) {
		arm1.m_command.immediate = false;
		arm1.m_command.commandType = 2;
		arm1.m_command.argument = (m_outerVacuumChanging << 3) + (m_innerVacuumChanging << 2) + (m_plankVacuumChanging << 1) + m_newVacumState;
		arm1.m_isCommandToSend = true;
		
		m_stateMachinePtr->setNextState(m_nextStatePtr);
    }
}

std::string ChangeVacuumStateState::getTypeString() { return "ChangeVacuumStateState"; }
	
YAML::Node ChangeVacuumStateState::serializeToYaml(unsigned int stateIndex) {
	YAML::Node node;
	node["id"] = stateIndex;
	node["type"] = getTypeString();
	YAML::Node parametersNode = node["parameters"];
	parametersNode["outer"] = m_outerVacuumChanging;
	parametersNode["inner"] = m_innerVacuumChanging;
	parametersNode["plank"] = m_plankVacuumChanging;
	parametersNode["newState"] = m_newVacumState;
	parametersNode["nextStateId"] = m_stateMachinePtr->getStateIndexByPtrValue(m_nextStatePtr);
	return node;
}

std::unique_ptr<StrategyState> ChangeVacuumStateState::createFromYaml(StrategyStateMachine* stateMachinePtr, const YAML::Node& node) {
	//m_stateIdInYaml = node["id"].as<unsigned int>();
	YAML::Node parametersNode = node["parameters"];
	bool outerVacuumChanging = parametersNode["outer"] ? parametersNode["outer"].as<bool>(): false;
	bool innerVacuumChanging = parametersNode["inner"] ? parametersNode["inner"].as<bool>(): false;
	bool plankVacuumChanging = parametersNode["plank"] ? parametersNode["plank"].as<bool>(): false;
	bool newVacumState = parametersNode["newState"] ? parametersNode["newState"].as<bool>(): false;
	unsigned int nextStateIdInYaml = parametersNode["nextStateId"].as<unsigned int>();
	return std::make_unique<ChangeVacuumStateState>(stateMachinePtr, outerVacuumChanging, innerVacuumChanging, plankVacuumChanging, newVacumState, nextStateIdInYaml);
}

void ChangeVacuumStateState::updateOutputLinks() {
	m_nextStatePtr = m_stateMachinePtr->m_stateIdsToPtrs[m_nextStateIdInYaml];
}

// ======================================= Set stepper pos state ======================================= 

SetStepperPosState::SetStepperPosState(StrategyStateMachine* stateMachinePtr, unsigned int newStepperPosition, unsigned int nextStateIdInYaml): StrategyState(stateMachinePtr) {
	m_newStepperPosition = newStepperPosition;
	m_nextStateIdInYaml = nextStateIdInYaml;
}    

void SetStepperPosState::startRoutine() {}

void SetStepperPosState::loopRoutine() {
	Arm& arm1 = m_stateMachinePtr->m_world.m_ownRobot.m_arm1;
	if (!arm1.m_isCommandToSend) {
		arm1.m_command.immediate = false;
		arm1.m_command.commandType = 3;
		arm1.m_command.argument = m_newStepperPosition;
		arm1.m_isCommandToSend = true;
		
		m_stateMachinePtr->setNextState(m_nextStatePtr);
    }
}

std::string SetStepperPosState::getTypeString() { return "SetStepperPosState"; }
	
YAML::Node SetStepperPosState::serializeToYaml(unsigned int stateIndex) {
	YAML::Node node;
	node["id"] = stateIndex;
	node["type"] = getTypeString();
	YAML::Node parametersNode = node["parameters"];
	parametersNode["pos"] = m_newStepperPosition;
	parametersNode["nextStateId"] = m_stateMachinePtr->getStateIndexByPtrValue(m_nextStatePtr);
	return node;
}

std::unique_ptr<StrategyState> SetStepperPosState::createFromYaml(StrategyStateMachine* stateMachinePtr, const YAML::Node& node) {
	//m_stateIdInYaml = node["id"].as<unsigned int>();
	YAML::Node parametersNode = node["parameters"];
	unsigned int newStepperPosition = parametersNode["pos"] ? parametersNode["pos"].as<unsigned int>(): 0;
	unsigned int nextStateIdInYaml = parametersNode["nextStateId"].as<unsigned int>();
	return std::make_unique<SetStepperPosState>(stateMachinePtr, newStepperPosition, nextStateIdInYaml);
}

void SetStepperPosState::updateOutputLinks() {
	m_nextStatePtr = m_stateMachinePtr->m_stateIdsToPtrs[m_nextStateIdInYaml];
}

// ======================================= Set servo pos state ======================================= 

SetServoPosState::SetServoPosState(StrategyStateMachine* stateMachinePtr, unsigned int newServoPosition, unsigned int nextStateIdInYaml): StrategyState(stateMachinePtr) {
	m_newServoPosition = newServoPosition;
	m_nextStateIdInYaml = nextStateIdInYaml;
}    

void SetServoPosState::startRoutine() {}

void SetServoPosState::loopRoutine() {
	Arm& arm1 = m_stateMachinePtr->m_world.m_ownRobot.m_arm1;
	if (!arm1.m_isCommandToSend) {
		arm1.m_command.immediate = false;
		arm1.m_command.commandType = 1;
		arm1.m_command.argument = m_newServoPosition;
		arm1.m_isCommandToSend = true;
		
		m_stateMachinePtr->setNextState(m_nextStatePtr);
    }
}

std::string SetServoPosState::getTypeString() { return "SetServoPosState"; }
	
YAML::Node SetServoPosState::serializeToYaml(unsigned int stateIndex) {
	YAML::Node node;
	node["id"] = stateIndex;
	node["type"] = getTypeString();
	YAML::Node parametersNode = node["parameters"];
	parametersNode["pos"] = m_newServoPosition;
	parametersNode["nextStateId"] = m_stateMachinePtr->getStateIndexByPtrValue(m_nextStatePtr);
	return node;
}

std::unique_ptr<StrategyState> SetServoPosState::createFromYaml(StrategyStateMachine* stateMachinePtr, const YAML::Node& node) {
	//m_stateIdInYaml = node["id"].as<unsigned int>();
	YAML::Node parametersNode = node["parameters"];
	unsigned int newServoPosition = parametersNode["pos"] ? parametersNode["pos"].as<unsigned int>(): 0.0f;
	unsigned int nextStateIdInYaml = parametersNode["nextStateId"].as<unsigned int>();
	return std::make_unique<SetServoPosState>(stateMachinePtr, newServoPosition, nextStateIdInYaml);
}

void SetServoPosState::updateOutputLinks() {
	m_nextStatePtr = m_stateMachinePtr->m_stateIdsToPtrs[m_nextStateIdInYaml];
}

// ======================================= Wait state ======================================= 

WaitState::WaitState(StrategyStateMachine* stateMachinePtr, float duration, unsigned int nextStateIdInYaml): StrategyState(stateMachinePtr) {
	m_duration = duration;
	m_nextStateIdInYaml = nextStateIdInYaml;
}    

void WaitState::startRoutine() {
	m_startTime = std::chrono::high_resolution_clock::now();
}

void WaitState::loopRoutine() {
	if (std::chrono::duration_cast<std::chrono::seconds>(std::chrono::high_resolution_clock::now() - m_startTime).count() >= m_duration) {
		m_stateMachinePtr->setNextState(m_nextStatePtr);
	}
}

std::string WaitState::getTypeString() { return "WaitState"; }
	
YAML::Node WaitState::serializeToYaml(unsigned int stateIndex) {
	YAML::Node node;
	node["id"] = stateIndex;
	node["type"] = getTypeString();
	YAML::Node parametersNode = node["parameters"];
	parametersNode["duration"] = m_duration;
	parametersNode["nextStateId"] = m_stateMachinePtr->getStateIndexByPtrValue(m_nextStatePtr);
	return node;
}

std::unique_ptr<StrategyState> WaitState::createFromYaml(StrategyStateMachine* stateMachinePtr, const YAML::Node& node) {
	//m_stateIdInYaml = node["id"].as<unsigned int>();
	YAML::Node parametersNode = node["parameters"];
	float duration = parametersNode["duration"] ? parametersNode["duration"].as<float>(): 0.0f;
	unsigned int nextStateIdInYaml = parametersNode["nextStateId"].as<unsigned int>();
	return std::make_unique<WaitState>(stateMachinePtr, duration, nextStateIdInYaml);
}

void WaitState::updateOutputLinks() {
	m_nextStatePtr = m_stateMachinePtr->m_stateIdsToPtrs[m_nextStateIdInYaml];
}
