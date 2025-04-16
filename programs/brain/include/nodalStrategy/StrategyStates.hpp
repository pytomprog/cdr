#pragma once

#ifndef STRATEGY_STATES
#define STRATEGY_STATES

#include <string>
#include <iostream>

#include "nodalStrategy/StrategyStateMachine.hpp"

class IdleState : public StrategyState {
public:
	IdleState(StrategyStateMachine* stateMachinePtr): StrategyState(stateMachinePtr) {};
	void startRoutine() override;
	void loopRoutine() override;
	std::string getTypeString() override;
    virtual YAML::Node serializeToYaml(unsigned int stateIndex) override;
    static std::unique_ptr<StrategyState> createFromYaml(StrategyStateMachine* stateMachinePtr, const YAML::Node& node);
    void updateOutputLinks();
};

class TestState : public StrategyState {
public:
	std::shared_ptr<StrategyState> m_nextStatePtr;
	unsigned int m_nextStateIdInYaml;
	
	std::string m_startMessage;
	std::string m_loopMessage;

	TestState(StrategyStateMachine* stateMachinePtr, std::string startMessage="default start message", std::string loopMessage="default loop message", unsigned int nextStateIdInYaml=0);
	void startRoutine() override;
	void loopRoutine() override;
	std::string getTypeString() override;
    virtual YAML::Node serializeToYaml(unsigned int stateIndex) override;
    static std::unique_ptr<StrategyState> createFromYaml(StrategyStateMachine* stateMachinePtr, const YAML::Node& node);
    void updateOutputLinks();
};

class TestState2 : public StrategyState {
public:
	std::shared_ptr<StrategyState> m_nextStatePtr;
	unsigned int m_nextStateIdInYaml = 0;

	TestState2(StrategyStateMachine* stateMachinePtr, unsigned int nextStateIdInYaml=0);
	void startRoutine() override;
	void loopRoutine() override;
	std::string getTypeString() override;
    virtual YAML::Node serializeToYaml(unsigned int stateIndex) override;
    static std::unique_ptr<StrategyState> createFromYaml(StrategyStateMachine* stateMachinePtr, const YAML::Node& node);
    void updateOutputLinks();
};

#endif //STRATEGY_STATES
