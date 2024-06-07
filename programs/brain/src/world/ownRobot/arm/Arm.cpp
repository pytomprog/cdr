#include "world/ownRobot/arm/Arm.hpp"

Arm::Arm() {
    m_isCommandToSend = false;
    m_command = ArmCommand{0, 0, 0};
}