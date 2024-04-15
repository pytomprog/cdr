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
	bcm2835_i2c_setSlaveAddress(I2C_ROLLING_BASE_ADDRESS);
	char writingBuffer[MAX_I2C_FRAME_LEN];
	std::string writtingBufferStr;
	uint32_t writingBufferLength = 0;
	uint8_t returnCode;

	if (m_world.m_ownRobot.m_periphericalsConfig.rollingBaseConnected) {
		switch (m_world.m_ownRobot.m_ownRobotRollingBaseMode) {
			case INDIVIDUAL_MOTOR_SPEEDS_CONTROL:
				writtingBufferStr = "T " + std::to_string(m_world.m_ownRobot.m_motor1TargetSpeedPercentage) + " "
					                    + std::to_string(m_world.m_ownRobot.m_motor2TargetSpeedPercentage) + " " 
					                    + std::to_string(m_world.m_ownRobot.m_motor3TargetSpeedPercentage);
				break;
			case DX_DY_DTHETA_CONTROL:

				break;
		}
	} else {
		writtingBufferStr = "T 0 0 0";
	}
	std::copy(std::begin(writingBuffer), std::end(writingBuffer), writtingBufferStr);
	writingBufferLength = writtingBufferStr.size();
	std::cout << "Sending: " << writingBuffer << std::endl;
	returnCode = bcm2835_i2c_write(writingBuffer, writingBufferLength);
	ImGui::End();
}