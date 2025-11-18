#pragma once

#ifndef STRATEGY_STATE_MACHINE
#define STRATEGY_STATE_MACHINE

#include <iostream> 
#include <vector>
#include <memory>
#include <fstream>
#include <string>
#include <unordered_map>

#include <yaml-cpp/yaml.h>

#include "world/World.hpp"

class StrategyStateMachine;

class StrategyState {
public:
	StrategyStateMachine* m_stateMachinePtr;
	//unsigned int m_stateIdInYaml = 0;

	StrategyState(StrategyStateMachine* stateMachinePtr);
	//StrategyState(const StrategyState& otherState);
	//StrategyState(StrategyState&& otherState);
	//StrategyState& operator=(const StrategyState&);
	~StrategyState() = default;
	
	virtual void startRoutine() = 0;
	virtual void loopRoutine() = 0;

    virtual std::string getTypeString() = 0;
    virtual YAML::Node serializeToYaml(unsigned int stateIndex) = 0;
    virtual void updateOutputLinks() = 0;
};

class StrategyStateMachine {
private:
	unsigned int m_stepsOnSameState = 0;
	
public:
	std::vector<std::shared_ptr<StrategyState>> m_statePtrs;
	std::unordered_map<unsigned int, std::shared_ptr<StrategyState>> m_stateIdsToPtrs;
	std::shared_ptr<StrategyState> m_currentStatePtr = nullptr;
	std::shared_ptr<StrategyState> m_nextStatePtr = nullptr;
	
	World& m_world;
	
	StrategyStateMachine() = delete;
	StrategyStateMachine(World& world);
	void addState(std::shared_ptr<StrategyState> newStatePtr);
	void setNextState(std::shared_ptr<StrategyState> nextStatePtr);
	unsigned int getStateIndexByPtrValue(std::shared_ptr<StrategyState> statePtr);
	
	void routine();
	unsigned int getStepsOnSameState();
	
	void saveStatesToYaml(const std::string& filename);
    void loadStatesFromYaml(const std::string& filename);
};

#endif //STRATEGY_STATE_MACHINE
