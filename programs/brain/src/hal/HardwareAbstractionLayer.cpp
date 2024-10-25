#include "hal/HardwareAbstractionLayer.hpp"

#include <imgui.h>
#include <imgui-SFML.h>
#include <string>
#include <iostream>
#include <sstream>
#include <cmath>
#include <chrono>
#include <bitset>

#include "hal/bcm2835.h"
#include "parameters.hpp"


HardwareAbstractionLayer::HardwareAbstractionLayer(World& world): m_world(world) {
	m_cameraServerSocket = socket(AF_INET, SOCK_STREAM, 0);
	m_cameraServerAddress.sin_family = AF_INET; 
    m_cameraServerAddress.sin_port = htons(8910); 
    m_cameraServerAddress.sin_addr.s_addr = INADDR_ANY;
    bind(m_cameraServerSocket, (struct sockaddr*)&m_cameraServerAddress, sizeof(m_cameraServerAddress)); 
	fcntl(m_cameraServerSocket, F_SETFL, O_NONBLOCK);
	listen(m_cameraServerSocket, 1); 
}

void HardwareAbstractionLayer::ownRobotCameraRoutine() {
	using clock = std::chrono::high_resolution_clock;
	using time_point = std::chrono::time_point<clock>;
						
	static time_point previousOwnRobotSpeedCalculationTime = clock::now();
	static time_point previousReceivedCameraMessageTime = clock::now();
	static int ownRobotPositionAcquiredNumber = 0;
	
	int returnedCode;
	
	if (m_world.m_ownRobot.m_periphericalsConfig.cameraEnabled) {
		if (!m_world.m_ownRobot.m_cameraConnected){
			m_cameraClientSocket = accept(m_cameraServerSocket, nullptr, nullptr);
			if (m_cameraClientSocket>=0) {
				fcntl(m_cameraClientSocket, F_SETFL, O_NONBLOCK);
				m_world.m_ownRobot.m_cameraConnected = true;
				std::cout << "Camera connected" << std::endl;
				previousReceivedCameraMessageTime = clock::now();
			} else {
				if(errno==EWOULDBLOCK){
					//std::cout << "Still waiting for camera connection ..." << std::endl;
				}
			}
		}
		if (m_world.m_ownRobot.m_cameraConnected) {
			char buffer[1024] = { 0 };
			returnedCode = recv(m_cameraClientSocket, buffer, sizeof(buffer), 0);
			std::string receivedMessage(buffer);
			std::stringstream receivedMessageStream(receivedMessage);
			if (receivedMessage.size() > 0) {
				previousReceivedCameraMessageTime = clock::now();
				//std::cout << "Received from camera: " << buffer << std::endl;
				
				std::string commandType;
				receivedMessageStream >> commandType;
				if (commandType == "A") {
					ownRobotPositionAcquiredNumber++;
					
					receivedMessageStream >> m_world.m_ownRobot.m_currentPose.m_position.m_x;
					receivedMessageStream >> m_world.m_ownRobot.m_currentPose.m_position.m_y;
					receivedMessageStream >> m_world.m_ownRobot.m_currentPose.m_orientation;
					
					if (ownRobotPositionAcquiredNumber == 5) {
						time_point currentTime = clock::now();
						std::chrono::duration<double> timeDifference = currentTime - previousOwnRobotSpeedCalculationTime;
						float speedDeltaTime = timeDifference.count();
					
						static float oldX = 0.f, oldY = 0.f, oldTheta = 0.f;
					
						m_world.m_ownRobot.m_currentSpeed.m_position.m_x = (m_world.m_ownRobot.m_currentPose.m_position.m_x - oldX) / speedDeltaTime;
						m_world.m_ownRobot.m_currentSpeed.m_position.m_y = (m_world.m_ownRobot.m_currentPose.m_position.m_y - oldY) / speedDeltaTime;
						m_world.m_ownRobot.m_currentSpeed.m_orientation = (m_world.m_ownRobot.m_currentPose.m_orientation - oldTheta) / speedDeltaTime;
						
						oldX = m_world.m_ownRobot.m_currentPose.m_position.m_x;
						oldY = m_world.m_ownRobot.m_currentPose.m_position.m_y;
						oldTheta = m_world.m_ownRobot.m_currentPose.m_orientation;						
						previousOwnRobotSpeedCalculationTime = currentTime;
						
						ownRobotPositionAcquiredNumber = 0;
					}
					
				} else if (commandType == "B") {
					receivedMessageStream >> m_world.m_advRobot.m_currentPose.m_position.m_x;
					receivedMessageStream >> m_world.m_advRobot.m_currentPose.m_position.m_y;
					receivedMessageStream >> m_world.m_advRobot.m_currentPose.m_orientation;
				}
			} else {
				time_point currentTime = clock::now();
				std::chrono::duration<double> timeDifference = currentTime - previousReceivedCameraMessageTime;
				float deltaTime = timeDifference.count();
				if (deltaTime > 0.5f) {
					//std::cout << "Sending message to camera to test connexion" << std::endl;
					if (send(m_cameraClientSocket, " ", 1 , 0) < 0) {
						//std::cout << "Errno: " << errno << std::endl;
						close(m_cameraClientSocket); 
						m_world.m_ownRobot.m_cameraConnected = false;
						std::cout << "Camera unexpectedly disconnected" << std::endl;
					}
				}
			}
		}
		//close(serverSocket); // Put it when camera program want to shutdown
	} else {
		if (m_world.m_ownRobot.m_cameraConnected){
			close(m_cameraClientSocket); 
			m_world.m_ownRobot.m_cameraConnected = false;
			std::cout << "Camera disconnected" << std::endl;
		}
	}
}

void HardwareAbstractionLayer::ownRobotRollingBaseRoutine() {
	using clock = std::chrono::high_resolution_clock;
    using time_point = std::chrono::time_point<clock>;
    
	if (m_world.m_ownRobot.m_periphericalsConfig.rollingBaseEnabled) {
		bcm2835_i2c_setSlaveAddress(I2C_ROLLING_BASE_ADDRESS);
		std::string writingBufferStr = "S";
		uint8_t returnedCode;
		static std::string previousCommandSent = "";
		static time_point previousRollingBaseCommandSentTime = clock::now();

		float robotsDistance = getDistance(m_world.m_advRobot.m_currentPose.m_position, m_world.m_ownRobot.m_currentPose.m_position);

		if (m_world.m_ownRobot.m_motorsEnabled && (!m_world.m_ownRobot.m_obstacleAvoidance || (robotsDistance > m_world.m_ownRobot.m_radius + m_world.m_advRobot.m_radius))) {
			switch (m_world.m_ownRobot.m_ownRobotRollingBaseMode) {
				case INDIVIDUAL_MOTOR_SPEEDS_CONTROL:
					writingBufferStr = "A " + std::to_string(m_world.m_ownRobot.m_motor1TargetSpeedPercentage) + " "
											+ std::to_string(m_world.m_ownRobot.m_motor2TargetSpeedPercentage) + " " 
											+ std::to_string(m_world.m_ownRobot.m_motor3TargetSpeedPercentage) + "\n";
					break;
				case DX_DY_DTHETA_CONTROL:
					writingBufferStr = "D " + std::to_string((int)(m_world.m_ownRobot.m_dXTarget)) + " "
											+ std::to_string((int)(m_world.m_ownRobot.m_dYTarget)) + " " 
											+ std::to_string((int)(m_world.m_ownRobot.m_dThetaTarget*1000)) + " "
											+ std::to_string((int)m_world.m_ownRobot.m_maxSpeedPercentage) + "\n";

					break;
				case GAMEPAD_CONTROL:
					writingBufferStr = "D " + std::to_string((int)(m_world.m_ownRobot.m_gamepadPrimaryAxisSpeedTarget * cos(PI/3.f) - m_world.m_ownRobot.m_gamepadSecondaryAxisSpeedTarget * sin(PI/3.f))) + " " 
					                        + std::to_string((int)(m_world.m_ownRobot.m_gamepadPrimaryAxisSpeedTarget * sin(PI/3.f) + m_world.m_ownRobot.m_gamepadSecondaryAxisSpeedTarget * cos(PI/3.f))) + " " 
					                        + std::to_string((int)(m_world.m_ownRobot.m_dThetaTarget*1000)) + " " 
					                        + std::to_string((int)m_world.m_ownRobot.m_maxSpeedPercentage) + "\n";

					break;
				case POSE_CONTROL:			
					static float errorXPrevious = 0;
					float& errorXIntegrated = m_world.m_ownRobot.m_errorPoseIntegrated.m_position.m_x;
					float errorX = m_world.m_ownRobot.m_targetPose.m_position.m_x - m_world.m_ownRobot.m_currentPose.m_position.m_x;
					errorXIntegrated += errorX * m_deltaTime;
					float errorXDerivated = - m_world.m_ownRobot.m_currentSpeed.m_position.m_x;  // TODO: xTargetSpeed - m_world.m_ownRobot.m_currentSpeed.m_position.m_x;
					//TODO: Improve it: the hal loop is very fast so the derivative is computed on a small m_delatTime so it computes almost "instantaneous derivative" whereas we maybe want more a slipping mean 
					float xCommandSpeed = errorX * m_world.m_ownRobot.m_xyPCoefficient + errorXIntegrated * m_world.m_ownRobot.m_xyICoefficient + errorXDerivated * m_world.m_ownRobot.m_xyDCoefficient;
					errorXPrevious = errorX;
					
					static float errorYPrevious = 0;
					float& errorYIntegrated = m_world.m_ownRobot.m_errorPoseIntegrated.m_position.m_y;
					float errorY = m_world.m_ownRobot.m_targetPose.m_position.m_y - m_world.m_ownRobot.m_currentPose.m_position.m_y;
					errorYIntegrated += errorY * m_deltaTime;
					float errorYDerivated = - m_world.m_ownRobot.m_currentSpeed.m_position.m_y;
					float yCommandSpeed = errorY * m_world.m_ownRobot.m_xyPCoefficient + errorYIntegrated * m_world.m_ownRobot.m_xyICoefficient + errorYDerivated * m_world.m_ownRobot.m_xyDCoefficient;
					errorYPrevious = errorY;
					
					static float errorThetaPrevious = 0;
					float& errorThetaIntegrated = m_world.m_ownRobot.m_errorPoseIntegrated.m_orientation;
					float errorTheta = remainder(m_world.m_ownRobot.m_targetPose.m_orientation - m_world.m_ownRobot.m_currentPose.m_orientation, 2*PI);
					errorThetaIntegrated += errorTheta * m_deltaTime;
					float errorThetaDerivated = - m_world.m_ownRobot.m_currentSpeed.m_orientation;
					float thetaCommandSpeed = errorTheta * m_world.m_ownRobot.m_thetaPCoefficient + errorThetaIntegrated * m_world.m_ownRobot.m_thetaICoefficient + errorThetaDerivated * m_world.m_ownRobot.m_thetaDCoefficient;
					errorThetaPrevious = errorTheta;
					
					m_world.m_ownRobot.m_commandSpeed.m_position.m_x = xCommandSpeed;
					m_world.m_ownRobot.m_commandSpeed.m_position.m_y = yCommandSpeed;
					m_world.m_ownRobot.m_commandSpeed.m_orientation = thetaCommandSpeed;
					
					
					writingBufferStr = "D " + std::to_string((int)(xCommandSpeed * cos(-m_world.m_ownRobot.m_currentPose.m_orientation) - yCommandSpeed * sin(-m_world.m_ownRobot.m_currentPose.m_orientation))) + " " 
					                        + std::to_string((int)(xCommandSpeed * sin(-m_world.m_ownRobot.m_currentPose.m_orientation) + yCommandSpeed * cos(-m_world.m_ownRobot.m_currentPose.m_orientation))) + " " 
					                        + std::to_string((int)(thetaCommandSpeed*1000)) + " " 
					                        + std::to_string((int)m_world.m_ownRobot.m_maxSpeedPercentage) + "\n";
					                        
					//writingBufferStr = "D 0 500 0 20\n";
					
					break;
			}
		} else {
			writingBufferStr = "S";
		}
		
		time_point currentTime = clock::now();
		std::chrono::duration<double> timeDifference = currentTime - previousRollingBaseCommandSentTime;
		float deltaTime = timeDifference.count();
		if ((writingBufferStr != previousCommandSent && deltaTime > 0.01f) || deltaTime > 0.25f) {
			//std::cout << "Sending to rolling base: " << writingBufferStr << std::endl;
			returnedCode = bcm2835_i2c_write(writingBufferStr.c_str(), writingBufferStr.size());
			
			previousRollingBaseCommandSentTime = clock::now();
		}
	}
}

void HardwareAbstractionLayer::ownRobotArmRoutine() {
	if (m_world.m_ownRobot.m_periphericalsConfig.armEnabled) {
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
