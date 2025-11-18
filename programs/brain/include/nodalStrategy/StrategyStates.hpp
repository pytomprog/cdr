#pragma once

#ifndef STRATEGY_STATES
#define STRATEGY_STATES

#include <string>
#include <iostream>

#include "nodalStrategy/StrategyStateMachine.hpp"
#include "world/World.hpp"

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

class StartState : public StrategyState {
public:
    std::shared_ptr<StrategyState> m_nextStatePtr;
    unsigned int m_nextStateIdInYaml = 0;
    
    StartState(StrategyStateMachine* stateMachinePtr, unsigned int nextStateIdInYaml=0);
    void startRoutine() override;
    void loopRoutine() override;
    std::string getTypeString() override;
    virtual YAML::Node serializeToYaml(unsigned int stateIndex) override;
    static std::unique_ptr<StrategyState> createFromYaml(StrategyStateMachine* stateMachinePtr, const YAML::Node& node);
    void updateOutputLinks();
};

class TiretteState : public StrategyState {
public:
    std::shared_ptr<StrategyState> m_nextStatePtr;
    unsigned int m_nextStateIdInYaml = 0;
    
    bool m_tirettePushed;
    
    TiretteState(StrategyStateMachine* stateMachinePtr, unsigned int nextStateIdInYaml=0);
    void startRoutine() override;
    void loopRoutine() override;
    std::string getTypeString() override;
    virtual YAML::Node serializeToYaml(unsigned int stateIndex) override;
    static std::unique_ptr<StrategyState> createFromYaml(StrategyStateMachine* stateMachinePtr, const YAML::Node& node);
    void updateOutputLinks();
};

class MovetoState : public StrategyState {
public:
    std::shared_ptr<StrategyState> m_nextStatePtr;
    unsigned int m_nextStateIdInYaml = 0;
    
    Pose2f m_targetPose;
    
    MovetoState(StrategyStateMachine* stateMachinePtr, Pose2f targetPose = Pose2f(), unsigned int nextStateIdInYaml=0);
    void startRoutine() override;
    void loopRoutine() override;
    std::string getTypeString() override;
    virtual YAML::Node serializeToYaml(unsigned int stateIndex) override;
    static std::unique_ptr<StrategyState> createFromYaml(StrategyStateMachine* stateMachinePtr, const YAML::Node& node);
    void updateOutputLinks();
};

class MoveThroughState : public StrategyState {
public:
    std::shared_ptr<StrategyState> m_nextStatePtr;
    unsigned int m_nextStateIdInYaml = 0;
    
    Pose2f m_targetPose;
    
    MoveThroughState(StrategyStateMachine* stateMachinePtr, Pose2f targetPose = Pose2f(), unsigned int nextStateIdInYaml=0);
    void startRoutine() override;
    void loopRoutine() override;
    std::string getTypeString() override;
    virtual YAML::Node serializeToYaml(unsigned int stateIndex) override;
    static std::unique_ptr<StrategyState> createFromYaml(StrategyStateMachine* stateMachinePtr, const YAML::Node& node);
    void updateOutputLinks();
};

class ChangeVacuumStateState : public StrategyState {
public:
    std::shared_ptr<StrategyState> m_nextStatePtr;
    unsigned int m_nextStateIdInYaml = 0;
    
    bool m_outerVacuumChanging, m_innerVacuumChanging, m_plankVacuumChanging, m_newVacumState;
    
    ChangeVacuumStateState(StrategyStateMachine* stateMachinePtr, bool outerVacuumChanging = false, bool innerVacuumChanging = false, bool plankVacuumChanging = false, bool newVacumState = false, unsigned int nextStateIdInYaml=0);
    void startRoutine() override;
    void loopRoutine() override;
    std::string getTypeString() override;
    virtual YAML::Node serializeToYaml(unsigned int stateIndex) override;
    static std::unique_ptr<StrategyState> createFromYaml(StrategyStateMachine* stateMachinePtr, const YAML::Node& node);
    void updateOutputLinks();
};

class SetStepperPosState : public StrategyState {
public:
    std::shared_ptr<StrategyState> m_nextStatePtr;
    unsigned int m_nextStateIdInYaml = 0;
    
    unsigned int m_newStepperPosition;
    
    SetStepperPosState(StrategyStateMachine* stateMachinePtr, unsigned int newStepperPosition = 0, unsigned int nextStateIdInYaml=0);
    void startRoutine() override;
    void loopRoutine() override;
    std::string getTypeString() override;
    virtual YAML::Node serializeToYaml(unsigned int stateIndex) override;
    static std::unique_ptr<StrategyState> createFromYaml(StrategyStateMachine* stateMachinePtr, const YAML::Node& node);
    void updateOutputLinks();
};

class SetServoPosState : public StrategyState {
public:
    std::shared_ptr<StrategyState> m_nextStatePtr;
    unsigned int m_nextStateIdInYaml = 0;
    
    unsigned int m_newServoPosition;
    
    SetServoPosState(StrategyStateMachine* stateMachinePtr, unsigned int newServoPosition = 0, unsigned int nextStateIdInYaml=0);
    void startRoutine() override;
    void loopRoutine() override;
    std::string getTypeString() override;
    virtual YAML::Node serializeToYaml(unsigned int stateIndex) override;
    static std::unique_ptr<StrategyState> createFromYaml(StrategyStateMachine* stateMachinePtr, const YAML::Node& node);
    void updateOutputLinks();
};

class WaitState : public StrategyState {
public:
    std::shared_ptr<StrategyState> m_nextStatePtr;
    unsigned int m_nextStateIdInYaml = 0;
    
    float m_duration;
    std::chrono::time_point<std::chrono::high_resolution_clock> m_startTime;
    
    WaitState(StrategyStateMachine* stateMachinePtr, float duration = 0.f, unsigned int nextStateIdInYaml=0);
    void startRoutine() override;
    void loopRoutine() override;
    std::string getTypeString() override;
    virtual YAML::Node serializeToYaml(unsigned int stateIndex) override;
    static std::unique_ptr<StrategyState> createFromYaml(StrategyStateMachine* stateMachinePtr, const YAML::Node& node);
    void updateOutputLinks();
};

#endif //STRATEGY_STATES
