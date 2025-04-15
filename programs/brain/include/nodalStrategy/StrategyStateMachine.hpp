#pragma once

#ifndef STRATEGY_STATE_MACHINE
#define STRATEGY_STATE_MACHINE

#include <iostream> 
#include <vector>
#include <memory>
#include <fstream>
#include <string>

#include <yaml-cpp/yaml.h>

class StrategyStateMachine;

class StrategyState {
public:
	StrategyStateMachine* m_stateMachinePtr;

	StrategyState(StrategyStateMachine* stateMachinePtr);
	//StrategyState(const StrategyState& otherState);
	//StrategyState(StrategyState&& otherState);
	//StrategyState& operator=(const StrategyState&);
	~StrategyState() = default;
	
	virtual void startRoutine() = 0;
	virtual void loopRoutine() = 0;

    virtual std::string getTypeString() = 0;
    virtual YAML::Node serializeToYaml() = 0;
};

class StrategyStateMachine {
private:
	unsigned int m_stepsOnSameState = 0;
	
public:
	std::vector<std::shared_ptr<StrategyState>> m_statePtrs;
	std::shared_ptr<StrategyState> m_currentStatePtr = nullptr;
	std::shared_ptr<StrategyState> m_nextStatePtr = nullptr;
	
	StrategyStateMachine() = default;
	void addState(std::shared_ptr<StrategyState> newStatePtr);
	void setNextState(std::shared_ptr<StrategyState> nextStatePtr);
	
	unsigned int getStepsOnSameState();
	
	void routine();
	
	void saveStatesToYaml(const std::string& filename);
    void loadStatesFromYaml(const std::string& filename);
};

#endif //STRATEGY_STATE_MACHINE
