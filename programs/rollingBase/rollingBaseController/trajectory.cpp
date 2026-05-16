#include <Arduino.h> // Just used for Serial print, could be removed otherwise
#include <vector>
#include <cmath>
#include "trajectory.hpp"

float getPhasesDuration(const std::vector<Phase> &phases) {
    float phasesDuration = 0.f;
    for (const Phase &phase: phases) {
        phasesDuration += phase.m_duration;
    }
    return phasesDuration;
}

std::vector<Phase> computePhases(const State1D &initialState, const State1D &finalState, const Limit &limit) {
    const float &aMax = limit.m_aMax;
    const float &aMin = limit.m_aMin;
    const float &vMax = limit.m_vMax;

    const float &pi = initialState.m_pos;
    const float &vi = initialState.m_vel;
    const float &pf = finalState.m_pos;
    const float &vf = finalState.m_vel;
    const float deltaP = abs(pf - pi);
    const float sign = (pf >= pi) ? 1 : -1;

    if (vi >= vMax && vf >= vMax) {
        const float T2 = deltaP / vi;
        return std::vector<Phase> {
            Phase{CONSTANT_VELOCITY, sign*std::max(vi, vMax), T2}
        };
    } else if (vf >= vMax) {
        float T1 = (vMax - vi) / aMax; // T1: time to accelerate to max speed
        const float deltaP1 = vi * T1 + 0.5 * aMax * T1*T1; // distance covered during acceleration
        if (deltaP1 >= deltaP) {
            // FIXED: Cap acceleration at aMax to prevent physics violations on short distances
            const float v_reached = std::sqrtf(vi*vi + 2.f * aMax * deltaP);
            T1 = (v_reached - vi) / aMax;
            return std::vector<Phase> {
                Phase{ACCELERATION, sign*aMax, T1}
            };
        } else {
            const float deltaP2 = deltaP - deltaP1;
            const float T2 = deltaP2 / vMax;
            return std::vector<Phase> {
                Phase{ACCELERATION, sign*aMax, T1},
                Phase{CONSTANT_VELOCITY, sign*vMax, T2}
            };
        }
    } else if (vi >= vMax) {
        float T3 = (vMax - vf) / aMin; // T3: time to decelerate from max speed
        const float deltaP3 = vf * T3 + 0.5 * aMin * T3*T3; // distance covered during deceleration
        if (deltaP3 >= deltaP) {
            // FIXED: Cap deceleration at aMin to prevent physics violations on short distances
            const float v_reached = std::sqrtf(std::max(0.0f, vMax*vMax - 2.f * aMin * deltaP));
            T3 = (vMax - v_reached) / aMin;
            return std::vector<Phase> {
                Phase{DECELERATION, sign*aMin, T3}
            };
        } else {
            const float deltaP2 = deltaP - deltaP3;
            const float T2 = deltaP2 / vMax;
            return std::vector<Phase> {
                Phase{CONSTANT_VELOCITY, sign*vMax, T2},
                Phase{DECELERATION, sign*aMin, T3}
            };
        }
    } else {
        float T1 = (vMax - vi) / aMax; // T1: time to accelerate to max speed
        const float deltaP1 = vi * T1 + 0.5 * aMax * T1*T1; // distance covered during acceleration
        float T3 = (vMax - vf) / aMin; // T3: time to decelerate from max speed
        const float deltaP3 = vf * T3 + 0.5 * aMin * T3*T3; // distance covered during deceleration
        if (deltaP1 + deltaP3 >= deltaP) {
            // FIXED: Moved the factor of 2 strictly to the deltaP term based on v_peak derivation
            const float v_peak = std::sqrtf((2*aMax*aMin*deltaP + aMin*vi*vi + aMax*vf*vf)/(aMax + aMin));
            T1 = (v_peak - vi) / aMax;
            T3 = (v_peak - vf) / aMin;
            return std::vector<Phase> {
                Phase{ACCELERATION, sign*aMax, T1},
                Phase{DECELERATION, sign*aMin, T3}
            };
        } else {
            const float deltaP2 = deltaP - deltaP1 - deltaP3;
            const float T2 = deltaP2 / vMax;
            return std::vector<Phase> {
                Phase{ACCELERATION, sign*aMax, T1},
                Phase{CONSTANT_VELOCITY, sign*vMax, T2},
                Phase{DECELERATION, sign*aMin, T3}
            };
        }
    }
}

// FIXED: Added initialVelocity default parameter to prepare for non-zero velocity chaining
DesiredState1D getDesiredState1D(float currentTime, const std::vector<Phase> &phases, const Limit &limit, float initialVelocity = 0.f) {
    float t = currentTime; // Not a const reference because its value is modified on purpose in the function
    float pStartOfPhase = 0.f;
    float vStartOfPhase = initialVelocity; // TODO: sometimes, it could be different from 0.f, eg: when recomputing trajectory during movement, or if not stopping between two linear segments
    const float &vMax = limit.m_vMax;

    vStartOfPhase = std::min(vStartOfPhase, vMax);

    for (const Phase &phase: phases) {
        if (t <= phase.m_duration) {
            if (phase.m_type == ACCELERATION) {
                return DesiredState1D{pStartOfPhase + vStartOfPhase * t + 0.5f * phase.m_value * t*t , vStartOfPhase + phase.m_value * t, phase.m_value, false};
            } else if (phase.m_type == CONSTANT_VELOCITY) {
                return DesiredState1D{pStartOfPhase + phase.m_value * t , phase.m_value, 0, false};
            } else if (phase.m_type == DECELERATION) {
                return DesiredState1D{pStartOfPhase + vStartOfPhase * t - 0.5f * phase.m_value * t*t , vStartOfPhase - phase.m_value * t, -phase.m_value, false};
            } else if (phase.m_type == WAITING) {
                return DesiredState1D{pStartOfPhase + vStartOfPhase * t, vStartOfPhase, 0, false};
            }
        } else {
            if (phase.m_type == ACCELERATION) {
                pStartOfPhase += vStartOfPhase * phase.m_duration + 0.5f * phase.m_value * phase.m_duration*phase.m_duration;
                vStartOfPhase += phase.m_value * phase.m_duration;
            } else if (phase.m_type == CONSTANT_VELOCITY) {
                pStartOfPhase += vStartOfPhase * phase.m_duration;
            } else if (phase.m_type == DECELERATION) {
                pStartOfPhase += vStartOfPhase * phase.m_duration - 0.5f * phase.m_value * phase.m_duration*phase.m_duration;
                vStartOfPhase -= phase.m_value * phase.m_duration;
            }
            t -= phase.m_duration;
        }
    }

    return DesiredState1D{pStartOfPhase, vStartOfPhase, 0, true};
}

Trajectory2D::Trajectory2D() {}

void Trajectory2D::computeSubTrajectories(float pXInit, float pYInit, float pThetaInit) {
    m_linearPhasesSegments.clear();
    m_angularPhases.clear();

    float pXPrev = pXInit, pYPrev = pYInit, pThetaPrev = pThetaInit; // Previous pos setpoint
    float vXYPrev = 0.f; //TODO: sometimes, it could be different from 0.f, eg: when recomputing trajectory during movement
    float vThetaPrev = 0.f; // Previous vel setpoint

    m_pThetaStart = pThetaInit; // Used in getDesiredState2D

    for (size_t stepId = 0; stepId < m_steps.size(); stepId++) {
        const Trajectory2DStep &step = m_steps[stepId];

        if (step.m_type == WAIT) {     
            const float waitingTime = std::max(step.value1, 0.f);
                  
            m_linearPhasesSegments.push_back(LinearPhasesSegment {
                std::vector<Phase>{ Phase{WAITING, 0.f, waitingTime} },
                pXPrev,
                pYPrev,
                0.f,
                0.f,
            });

            m_angularPhases.push_back(Phase{WAITING, 0.f, waitingTime});
        } else if (step.m_type == GO_TO) {
            float segmentDistance = std::sqrtf(std::powf(step.value1 - pXPrev, 2) + std::powf(step.value2 - pYPrev, 2));

            // FIXED: Prevent division by zero (NaN) if target is the same as current position
            if (segmentDistance < 1e-4f) continue;

            float vXYEnd = 0.f; // TODO: sometimes, it could be different from 0.f, eg: if not stopping between two linear segments
            
            m_linearPhasesSegments.push_back(LinearPhasesSegment {
                computePhases(State1D{0.f, vXYPrev}, State1D{segmentDistance, vXYEnd}, m_linearLimit),
                pXPrev,
                pYPrev,
                (step.value1 - pXPrev)/segmentDistance,
                (step.value2 - pYPrev)/segmentDistance,
            });

            m_angularPhases.push_back(Phase{WAITING, 0.f, getPhasesDuration(m_linearPhasesSegments.back().m_phases)});

            pXPrev = step.value1;
            pYPrev = step.value2;
            vXYPrev = vXYEnd;
        } else if (step.m_type == ROTATE_TO) {
            float vThetaEnd = 0.f; // TODO: sometimes, it could be different from 0.f

            float pThetaEnd = wrapToPi(step.value1 - pThetaPrev) + pThetaPrev;
            
            std::vector<Phase> newAngularPhases = computePhases(State1D{pThetaPrev, vThetaPrev}, State1D{pThetaEnd, vThetaEnd}, m_angularLimit);
            m_angularPhases.insert(m_angularPhases.end(), newAngularPhases.begin(), newAngularPhases.end());

            m_linearPhasesSegments.push_back(LinearPhasesSegment {
                std::vector<Phase>{ Phase{WAITING, 0.f, getPhasesDuration(newAngularPhases)} },
                pXPrev,
                pYPrev,
                0.f,
                0.f,
            });

            pThetaPrev = step.value1;
            vThetaPrev = vThetaEnd;
        } else if (step.m_type == ROTATE_AND_GO_TO) {
            float segmentDistance = std::sqrtf(std::powf(step.value1 - pXPrev, 2) + std::powf(step.value2 - pYPrev, 2));
            float segmentAngle = std::atan2(step.value2 - pYPrev, step.value1 - pXPrev) - HALF_PI;
            float angleDiff = wrapToPi(segmentAngle - pThetaPrev);
            if (angleDiff > HALF_PI) {
                angleDiff -= PI;
                segmentDistance = -segmentDistance;
            } else if (angleDiff < -HALF_PI) {
                angleDiff += PI;
                segmentDistance = -segmentDistance;
            }
            float angleGoal = angleDiff + pThetaPrev;

            // FIXED: Prevent processing logic on a zero-distance move
            if (abs(segmentDistance) < 1e-4f) continue;

            float vXYEnd = 0.f; // TODO: sometimes, it could be different from 0.f, eg: if not stopping between two linear segments

            // Rotate part

            std::vector<Phase> newAngularPhases = computePhases(State1D{pThetaPrev, vThetaPrev}, State1D{angleGoal, 0}, m_angularLimit); // The angular speed at the end can't be different than 0.f because there is the linear move after the angular move
            m_angularPhases.insert(m_angularPhases.end(), newAngularPhases.begin(), newAngularPhases.end());

            m_linearPhasesSegments.push_back(LinearPhasesSegment {
                std::vector<Phase>{ Phase{WAITING, 0.f, getPhasesDuration(newAngularPhases)} },
                pXPrev,
                pYPrev,
                0.f,
                0.f,
            });

            // Go to part
            
            m_linearPhasesSegments.push_back(LinearPhasesSegment {
                computePhases(State1D{0.f, vXYPrev}, State1D{segmentDistance, vXYEnd}, m_linearLimit),
                pXPrev,
                pYPrev,
                (step.value1 - pXPrev)/segmentDistance,
                (step.value2 - pYPrev)/segmentDistance,
            });

            m_angularPhases.push_back(Phase{WAITING, 0.f, getPhasesDuration(m_linearPhasesSegments.back().m_phases)});

            pXPrev = step.value1;
            pYPrev = step.value2;
            vXYPrev = vXYEnd;
            pThetaPrev = segmentAngle;
            vThetaPrev = 0; // The angular speed at the end can't be different than 0.f because there is the linear move after the angular move
        }
    }

    Serial.println("Trajectory:");
    Serial.println("  Linear:");
    for (size_t segmentId = 0; segmentId < m_linearPhasesSegments.size(); segmentId++) {
        const LinearPhasesSegment &linearPhasesSegment = m_linearPhasesSegments[segmentId];
        Serial.printf("    Segment %u:\n", segmentId);
        Serial.printf("      xStart: %f, yStart: %f, xScale: %f, yScale: %f\n", linearPhasesSegment.m_xStart, linearPhasesSegment.m_yStart, linearPhasesSegment.m_xScale, linearPhasesSegment.m_yScale);
        Serial.println("      Phases:");
        for (const Phase &phase: linearPhasesSegment.m_phases) {
            Serial.printf("        %s: value: %f, duration: %f\n", (const char*[]){"Waiting", "Acceleration", "Constant velocity", "Deceleration"}[phase.m_type], phase.m_value, phase.m_duration);
        }
    }
    Serial.println("  Angular:");
    Serial.printf("    thetaStart:%f\n", m_pThetaStart);
    Serial.println("    Phases:");
    for (const Phase &phase: m_angularPhases) {
        Serial.printf("      %s: value: %f, duration: %f\n", (const char*[]){"Waiting", "Acceleration", "Constant velocity", "Deceleration"}[phase.m_type], phase.m_value, phase.m_duration);
    }
}

DesiredState2D Trajectory2D::getDesiredState2D(float currentTime) {
    // FIXED: Prevent Segfault if getDesiredState2D is called on an empty trajectory
    if (m_linearPhasesSegments.empty()) {
        return DesiredState2D {
            sfe_otos_pose2d_t{0.f, 0.f, m_pThetaStart}, 0.f, 0.f, 0.f, 0.f, true
        };
    }

    float linearCurrentTime = currentTime; // Not a const reference because its value is modified on purpose in the function
    DesiredState1D desiredLinearState;
    size_t currentLinearPhasesSegmentId = 0; // FIXED: Variable initialized
    for (size_t linearPhasesSegmentId = 0; linearPhasesSegmentId < m_linearPhasesSegments.size(); linearPhasesSegmentId++) {
        const LinearPhasesSegment &linearPhasesSegment = m_linearPhasesSegments[linearPhasesSegmentId];
        
        float phasesDuration = getPhasesDuration(linearPhasesSegment.m_phases);
        if (linearCurrentTime <= phasesDuration || linearPhasesSegmentId == m_linearPhasesSegments.size()-1) {
            desiredLinearState = getDesiredState1D(linearCurrentTime, linearPhasesSegment.m_phases, m_linearLimit);
            currentLinearPhasesSegmentId = linearPhasesSegmentId;
            break;
        } else {
            linearCurrentTime -= phasesDuration;
        }
    }
    const DesiredState1D desiredAngularState = getDesiredState1D(currentTime, m_angularPhases, m_angularLimit);

    const LinearPhasesSegment &currentLinearPhasesSegment = m_linearPhasesSegments[currentLinearPhasesSegmentId];
    const float pXDesired = currentLinearPhasesSegment.m_xStart + desiredLinearState.m_state.m_pos * currentLinearPhasesSegment.m_xScale;
    const float pYDesired = currentLinearPhasesSegment.m_yStart + desiredLinearState.m_state.m_pos * currentLinearPhasesSegment.m_yScale;
    const float pThetaDesired = m_pThetaStart + desiredAngularState.m_state.m_pos;

    return DesiredState2D {
        sfe_otos_pose2d_t{pXDesired, pYDesired, pThetaDesired},
        desiredLinearState.m_state.m_vel,
        desiredAngularState.m_state.m_vel,
        desiredLinearState.m_state.m_accel,
        desiredAngularState.m_state.m_accel,
        desiredLinearState.m_phasesFinished && desiredAngularState.m_phasesFinished
    };
}