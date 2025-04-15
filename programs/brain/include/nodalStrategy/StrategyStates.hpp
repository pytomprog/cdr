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
    virtual YAML::Node serializeToYaml() override;
    static std::unique_ptr<StrategyState> createFromYaml(StrategyStateMachine* stateMachinePtr, const YAML::Node& node);
};

class TestState : public StrategyState {
public:
	std::shared_ptr<StrategyState> m_nextStatePtr;

	TestState(StrategyStateMachine* stateMachinePtr): StrategyState(stateMachinePtr) {};
	void startRoutine() override;
	void loopRoutine() override;
	std::string getTypeString() override;
    virtual YAML::Node serializeToYaml() override;
    static std::unique_ptr<StrategyState> createFromYaml(StrategyStateMachine* stateMachinePtr, const YAML::Node& node);
};

class TestState2 : public StrategyState {
public:
	std::shared_ptr<StrategyState> m_nextStatePtr;

	TestState2(StrategyStateMachine* stateMachinePtr): StrategyState(stateMachinePtr) {};
	void startRoutine() override;
	void loopRoutine() override;
	std::string getTypeString() override;
    virtual YAML::Node serializeToYaml() override;
    static std::unique_ptr<StrategyState> createFromYaml(StrategyStateMachine* stateMachinePtr, const YAML::Node& node);
};

#endif //STRATEGY_STATES
