#ifndef TRAJECTORY_HPP
#define TRAJECTORY_HPP

#include <vector>
#include "SparkFun_Qwiic_OTOS_Arduino_Library.h" // Just used for sfe_otos_pose2d_t, could be removed otherwise
#include "utils.hpp"

enum PhaseType {
    WAITING,
    ACCELERATION,
    CONSTANT_VELOCITY,
    DECELERATION
};

struct Phase {
    PhaseType m_type;
    float m_value;
    float m_duration;
};

struct State1D {
    float m_pos; // Position
    float m_vel; // Velocity
    float m_accel; // Acceleration
};

struct DesiredState1D {
    State1D m_state;
    bool m_phasesFinished;
};

struct Limit {
    float m_aMax; // Maximum acceleration > 0
    float m_aMin; // Maximum deceleration > 0
    float m_vMax; // Maximum velocity > 0
};

float getPhasesDuration(const std::vector<Phase> &phases);
std::vector<Phase> computePhases(const State1D &initialState, const State1D &finalState, const Limit &limit);
DesiredState1D getDesiredState1D(float currentTime, const std::vector<Phase> &phases, const Limit &limit, float initialVelocity);

struct LinearPhasesSegment {
    std::vector<Phase> m_phases;
    float m_xStart;
    float m_yStart;
    float m_xScale;
    float m_yScale;
};

enum Trajectory2DStepType {
    WAIT,
    GO_TO,
    ROTATE_TO,
    ROTATE_AND_GO_TO
};

struct Trajectory2DStep {
    Trajectory2DStepType m_type;
    float value1;
    float value2;
};

struct DesiredState2D {
    sfe_otos_pose2d_t m_pos;
    float m_XYVel;
    float m_thetaVel;
    float m_XYAccel;
    float m_thetaAccel;
    bool m_trajectoryFinished;
};

class Trajectory2D {
public:
    std::vector<LinearPhasesSegment> m_linearPhasesSegments;
    std::vector<Phase> m_angularPhases;
    float m_pThetaStart;

    Limit m_linearLimit;
    Limit m_angularLimit;

    std::vector<Trajectory2DStep> m_steps;

    Trajectory2D();

    void computeSubTrajectories(float pXInit = 0.f, float pYInit = 0.f, float pThetaInit = 0.f);
    DesiredState2D getDesiredState2D(float currentTime);
};

#endif //TRAJECTORY_HPP