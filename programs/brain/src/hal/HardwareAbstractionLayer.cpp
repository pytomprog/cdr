#include "hal/HardwareAbstractionLayer.hpp"

#include <imgui.h>
#include <imgui-SFML.h>
#include <string>
#include <iostream>

#include "hal/bcm2835.h"
#include "parameters.hpp"

HardwareAbstractionLayer::HardwareAbstractionLayer(World& world): m_world(world) {

}

void HardwareAbstractionLayer::ownRobotRollingBaseRoutine() {
	if (m_world.m_ownRobot.m_periphericalsConfig.rollingBaseConnected) {
		bcm2835_i2c_setSlaveAddress(I2C_ROLLING_BASE_ADDRESS);
		std::string writingBufferStr = "S";
		uint8_t returnCode;

		if (m_world.m_ownRobot.m_motorsEnabled) {
			switch (m_world.m_ownRobot.m_ownRobotRollingBaseMode) {
				case INDIVIDUAL_MOTOR_SPEEDS_CONTROL:
					writingBufferStr = "A " + std::to_string(m_world.m_ownRobot.m_motor1TargetSpeedPercentage) + " "
											+ std::to_string(m_world.m_ownRobot.m_motor2TargetSpeedPercentage) + " " 
											+ std::to_string(m_world.m_ownRobot.m_motor3TargetSpeedPercentage);
					break;
				case DX_DY_DTHETA_CONTROL:
					writingBufferStr = "B " + std::to_string(m_world.m_ownRobot.m_dXTarget) + " "
											+ std::to_string(m_world.m_ownRobot.m_dYTarget) + " " 
											+ std::to_string(m_world.m_ownRobot.m_dThetaTarget);

					break;
			}
		} else {
			writingBufferStr = "S";
		}

		//std::cout << "Sending: " << writingBufferStr << std::endl;
		returnCode = bcm2835_i2c_write(writingBufferStr.c_str(), writingBufferStr.size());
	}
}