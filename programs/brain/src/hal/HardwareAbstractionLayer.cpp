#include "hal/HardwareAbstractionLayer.hpp"

#include <imgui.h>
#include <imgui-SFML.h>
#include <string>
#include <iostream>
#include <sstream>
#include <cmath>
#include <bitset>

#include "hal/bcm2835.h"
#include "parameters.hpp"

HardwareAbstractionLayer::HardwareAbstractionLayer(World& world): m_world(world) {
	m_cameraServerSocket = socket(AF_INET, SOCK_STREAM, 0);
	m_cameraServerAddress.sin_family = AF_INET; 
    m_cameraServerAddress.sin_port = htons(8910); 
    m_cameraServerAddress.sin_addr.s_addr = INADDR_ANY;
	m_cameraClientConnected = false; 
    bind(m_cameraServerSocket, (struct sockaddr*)&m_cameraServerAddress, sizeof(m_cameraServerAddress)); 
	fcntl(m_cameraServerSocket, F_SETFL, O_NONBLOCK);
	listen(m_cameraServerSocket, 1); 
}

void HardwareAbstractionLayer::ownRobotCameraRoutine() {
	if (m_world.m_ownRobot.m_periphericalsConfig.cameraConnected) {
		if (!m_cameraClientConnected){
			m_cameraClientSocket = accept(m_cameraServerSocket, nullptr, nullptr); 
			if(m_cameraClientSocket<0){
				if(errno==EWOULDBLOCK){
					//std::cout << "Still waiting for camera connection ..." << std::endl;
				}
			} else {
				fcntl(m_cameraClientSocket, F_SETFL, O_NONBLOCK);
				m_cameraClientConnected = true;
			}
		}
		if (m_cameraClientConnected) {
			char buffer[1024] = { 0 };
			recv(m_cameraClientSocket, buffer, sizeof(buffer), 0);
			std::string receivedMessage(buffer);
			std::stringstream receivedMessageStream(receivedMessage);
			if (receivedMessage.size() > 0) {
				//std::cout << "Received from camera: " << buffer << std::endl;
				
				std::string commandType;
				receivedMessageStream >> commandType;
				if (commandType == "A") {
					receivedMessageStream >> m_world.m_ownRobot.m_currentPose.m_position.m_x;
					receivedMessageStream >> m_world.m_ownRobot.m_currentPose.m_position.m_y;
					receivedMessageStream >> m_world.m_ownRobot.m_currentPose.m_orientation;
				} else if (commandType == "B") {
					receivedMessageStream >> m_world.m_advRobot.m_currentPose.m_position.m_x;
					receivedMessageStream >> m_world.m_advRobot.m_currentPose.m_position.m_y;
					receivedMessageStream >> m_world.m_advRobot.m_currentPose.m_orientation;
				}
			}
		}
		//close(serverSocket); // Put it when camera program want to shutdown
	}
}

void HardwareAbstractionLayer::ownRobotRollingBaseRoutine() {
	if (m_world.m_ownRobot.m_periphericalsConfig.rollingBaseConnected) {
		bcm2835_i2c_setSlaveAddress(I2C_ROLLING_BASE_ADDRESS);
		std::string writingBufferStr = "S";
		uint8_t returnCode;

		float robotsDistance = getDistance(m_world.m_advRobot.m_currentPose.m_position, m_world.m_ownRobot.m_currentPose.m_position);

		if (m_world.m_ownRobot.m_motorsEnabled && (!m_world.m_ownRobot.m_obstacleAvoidance || (robotsDistance > m_world.m_ownRobot.m_radius + m_world.m_advRobot.m_radius))) {
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
				case POSE_CONTROL:
					writingBufferStr = "C " + std::to_string((int)((m_world.m_ownRobot.m_targetPose.m_position.m_x - m_world.m_ownRobot.m_currentPose.m_position.m_x) * cos(-m_world.m_ownRobot.m_currentPose.m_orientation) - (m_world.m_ownRobot.m_targetPose.m_position.m_y - m_world.m_ownRobot.m_currentPose.m_position.m_y) * sin(-m_world.m_ownRobot.m_currentPose.m_orientation))) + " "
											+ std::to_string((int)((m_world.m_ownRobot.m_targetPose.m_position.m_x - m_world.m_ownRobot.m_currentPose.m_position.m_x) * sin(-m_world.m_ownRobot.m_currentPose.m_orientation) + (m_world.m_ownRobot.m_targetPose.m_position.m_y - m_world.m_ownRobot.m_currentPose.m_position.m_y) * cos(-m_world.m_ownRobot.m_currentPose.m_orientation))) + " "
											+ std::to_string((int)(200*(remainder(m_world.m_ownRobot.m_targetPose.m_orientation - m_world.m_ownRobot.m_currentPose.m_orientation, 2*PI)))) + " "
											+ std::to_string((int)std::min((double)m_world.m_ownRobot.m_maxSpeedPercentage, 0.0001f*(std::pow(m_world.m_ownRobot.m_targetPose.m_position.m_x - m_world.m_ownRobot.m_currentPose.m_position.m_x, 2) + std::pow(m_world.m_ownRobot.m_targetPose.m_position.m_y - m_world.m_ownRobot.m_currentPose.m_position.m_y, 2) + 100*std::pow(remainder(m_world.m_ownRobot.m_targetPose.m_orientation - m_world.m_ownRobot.m_currentPose.m_orientation, 2*PI), 2))));

					break;
			}
		} else {
			writingBufferStr = "S";
		}

		//std::cout << "Sending to rolling base: " << writingBufferStr << std::endl;
		returnCode = bcm2835_i2c_write(writingBufferStr.c_str(), writingBufferStr.size());
	}
}

void HardwareAbstractionLayer::ownRobotArmRoutine() {
	if (m_world.m_ownRobot.m_periphericalsConfig.armConnected) {
		if (m_world.m_ownRobot.m_arm1.m_isCommandToSend) {
			bcm2835_i2c_setSlaveAddress(I2C_ARM_1_ADDRESS);
			uint8_t returnCode;
			char sendingByte = m_world.m_ownRobot.m_arm1.m_command.immediate << 7 |
							   m_world.m_ownRobot.m_arm1.m_command.commandType << 4 |
							   m_world.m_ownRobot.m_arm1.m_command.argument;
			std::cout << "Sending to arm 1: " << std::bitset<8>(sendingByte).to_string() << std::endl;
			returnCode = bcm2835_i2c_write(&sendingByte, 1);
			m_world.m_ownRobot.m_arm1.m_isCommandToSend = false;			
		}
		if (m_world.m_ownRobot.m_arm2.m_isCommandToSend) {
			bcm2835_i2c_setSlaveAddress(I2C_ARM_2_ADDRESS);
			uint8_t returnCode;
			char sendingByte = m_world.m_ownRobot.m_arm2.m_command.immediate << 7 |
							   m_world.m_ownRobot.m_arm2.m_command.commandType << 4 |
							   m_world.m_ownRobot.m_arm2.m_command.argument;
			//std::cout << "Sending to arm 2: " << std::bitset<8>(sendingByte).to_string() << std::endl;
			returnCode = bcm2835_i2c_write(&sendingByte, 1);
			m_world.m_ownRobot.m_arm2.m_isCommandToSend = false;
		}
	}
}