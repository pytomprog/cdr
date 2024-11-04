#include "gui/gui.hpp"

#include <imgui.h>
#include <imgui-SFML.h>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Joystick.hpp>
#include <iostream>

#include "parameters.hpp"

GraphicalUserInterface::GraphicalUserInterface(sf::RenderWindow& window, World& world, std::vector<std::string> strategiesNames): m_window(window), m_world(world) {
	m_strategiesNames = strategiesNames;
	for (size_t strategyIndex = 0; strategyIndex < strategiesNames.size(); strategyIndex++) {
		m_strategiesNamesCharPtr[strategyIndex] = m_strategiesNames[strategyIndex].c_str();
	}
}

void GraphicalUserInterface::homeMenuConfig() {
	const char* runningModesList[] =
	{
		"Test mode",
		"Strategy mode"
	};
	
	ImGui::SetNextWindowSizeConstraints(ImVec2(200, -1), ImVec2(2000, -1));
	ImGui::Begin("Home menu", 0, ImGuiWindowFlags_AlwaysAutoResize);
	static int runningModeChoice = m_world.m_runningMode;
	ImGui::Combo("Running mode", &runningModeChoice, runningModesList, IM_ARRAYSIZE(runningModesList));
	RunningMode newRunningMode = static_cast<RunningMode>(runningModeChoice);
	if (newRunningMode == STRATEGY_MODE) {
		if (newRunningMode != m_world.m_runningMode) {
			m_world.m_strategyStep = 0;
			m_world.m_ownRobot.m_periphericalsConfig = OwnRobotPeriphericalsConfig{ true, true, true };
		}

		static int strategyChoice = std::find(m_strategiesNames.begin(), m_strategiesNames.end(), m_world.m_strategyName) - m_strategiesNames.begin();
		ImGui::Combo("Strategy", &strategyChoice, m_strategiesNamesCharPtr, m_strategiesNames.size());
		std::string newStrategyName = m_strategiesNames[strategyChoice];
		if (newStrategyName != m_world.m_strategyName) {
			m_world.m_strategyName = newStrategyName;
			m_world.m_strategyStep = 0;
			m_world.m_ownRobot.m_periphericalsConfig = OwnRobotPeriphericalsConfig{ true, true, true };
		}
		
		if (ImGui::Button("Restart strategy")) {
			m_world.m_strategyStep = 0;
		}
		
		if (ImGui::Button("Previous step")) {
			m_world.m_strategyStep--;
		}
		
		if (ImGui::Button("Next step")) {
			m_world.m_strategyStep++;
		}

		ImGui::Text("Current step: %.1i", m_world.m_strategyStep);
	}
	m_world.m_runningMode = newRunningMode;
}

void GraphicalUserInterface::periphericalsConfigRoutine() {
	ImGui::SetNextWindowSizeConstraints(ImVec2(200, -1), ImVec2(2000, -1));
    ImGui::Begin("HAL configuration", 0, ImGuiWindowFlags_AlwaysAutoResize);
    ImGui::Checkbox("Enable camera", &m_world.m_ownRobot.m_periphericalsConfig.cameraEnabled);
    ImGui::Text("Camera status: ");
    ImGui::SameLine();
    if (m_world.m_ownRobot.m_cameraConnected) {
		ImGui::TextColored(ImColor(0, 255, 0, 255), "Connected");
	} else {
		ImGui::TextColored(ImColor(255, 0, 0, 255), "Disconnected");
    }
    ImGui::Checkbox("Enable rolling base", &m_world.m_ownRobot.m_periphericalsConfig.rollingBaseEnabled);
    ImGui::Checkbox("Enable arm", &m_world.m_ownRobot.m_periphericalsConfig.armEnabled);
    ImGui::Checkbox("Enable gamepad", &m_gamepadEnabled);
	ImGui::End();
}

void GraphicalUserInterface::cameraRoutine() {
	ImGui::SetNextWindowSizeConstraints(ImVec2(200, -1), ImVec2(2000, -1));
	ImGui::Begin("Camera", 0, ImGuiWindowFlags_AlwaysAutoResize);
	if (m_world.m_ownRobot.m_periphericalsConfig.cameraEnabled) {
		/*float x = m_ownRobot.m_pose.m_position.m_x;
		float y = m_ownRobot.m_pose.m_position.m_y;
		float orientation = m_ownRobot.m_pose.m_orientation;
		ImGui::SliderFloat("Own robot X", &x, X_MIN_BORDER, X_MAX_BORDER);
		ImGui::SliderFloat("Own robot Y", &y, Y_MIN_BORDER, Y_MAX_BORDER);
		ImGui::SliderFloat("Own robot Theta", &orientation, 0, 2 * PI);*/
		ImGui::Text("Own robot X: %.1f", m_world.m_ownRobot.m_currentPose.m_position.m_x);
		ImGui::Text("Own robot Y: %.1f", m_world.m_ownRobot.m_currentPose.m_position.m_y);
		ImGui::Text("Own robot Theta: %.3f", m_world.m_ownRobot.m_currentPose.m_orientation);
		ImGui::Text("Adv robot X: %.1f", m_world.m_advRobot.m_currentPose.m_position.m_x);
		ImGui::Text("Adv robot Y: %.1f", m_world.m_advRobot.m_currentPose.m_position.m_y);
		ImGui::Text("Adv robot Theta: %.3f", m_world.m_advRobot.m_currentPose.m_orientation);
	} else {
		ImGui::SliderFloat("Own robot X", &m_world.m_ownRobot.m_currentPose.m_position.m_x, X_MIN_BORDER, X_MAX_BORDER, "%.1f");
		ImGui::SliderFloat("Own robot Y", &m_world.m_ownRobot.m_currentPose.m_position.m_y, Y_MIN_BORDER, Y_MAX_BORDER, "%.1f");
		ImGui::SliderFloat("Own robot Theta", &m_world.m_ownRobot.m_currentPose.m_orientation, -PI, PI);
		ImGui::SliderFloat("Adv robot X", &m_world.m_advRobot.m_currentPose.m_position.m_x, X_MIN_BORDER, X_MAX_BORDER, "%.1f");
		ImGui::SliderFloat("Adv robot Y", &m_world.m_advRobot.m_currentPose.m_position.m_y, Y_MIN_BORDER, Y_MAX_BORDER, "%.1f");
		ImGui::SliderFloat("Adv robot Theta", &m_world.m_advRobot.m_currentPose.m_orientation, -PI, PI);
	}
	ImGui::End();
}

void GraphicalUserInterface::ownRobotRollingBaseRoutine() {
	int mode = (int)m_world.m_ownRobot.m_ownRobotRollingBaseMode;
	const char* modesList[] =
	{
		"Individual motor speeds control",
		"dx dy dtheta control",
		"Gamepad control",
		"Position control"
	};
	
	ImGui::SetNextWindowSizeConstraints(ImVec2(200, -1), ImVec2(2000, -1));
	ImGui::Begin("Rolling base", 0, ImGuiWindowFlags_AlwaysAutoResize);
	if (m_world.m_ownRobot.m_periphericalsConfig.rollingBaseEnabled) {
		ImGui::Checkbox("Enable motors", &m_world.m_ownRobot.m_motorsEnabled);
		ImGui::Checkbox("Enable obstacle avoidance", &m_world.m_ownRobot.m_obstacleAvoidance);
		ImGui::Combo("Modes", &mode, modesList, IM_ARRAYSIZE(modesList));
		m_world.m_ownRobot.m_ownRobotRollingBaseMode = OwnRobotRollingBaseMode(mode);
		switch (m_world.m_ownRobot.m_ownRobotRollingBaseMode) {
			case INDIVIDUAL_MOTOR_SPEEDS_CONTROL:
				ImGui::SliderInt("Motor 1 target speed", &m_world.m_ownRobot.m_motor1TargetSpeedPercentage, -100, 100);
				ImGui::SliderInt("Motor 2 target speed", &m_world.m_ownRobot.m_motor2TargetSpeedPercentage, -100, 100);
				ImGui::SliderInt("Motor 3 target speed", &m_world.m_ownRobot.m_motor3TargetSpeedPercentage, -100, 100);
				if (ImGui::Button("Stop") || sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
					m_world.m_ownRobot.m_motor1TargetSpeedPercentage = 0;
					m_world.m_ownRobot.m_motor2TargetSpeedPercentage = 0;
					m_world.m_ownRobot.m_motor3TargetSpeedPercentage = 0;
				}
				break;
			case DX_DY_DTHETA_CONTROL:
				ImGui::SliderInt("Max speed percentage", &m_world.m_ownRobot.m_maxSpeedPercentage, 0, 100);
				ImGui::SliderInt("Taget dX", &m_world.m_ownRobot.m_dXTarget, -1000, 1000);
				ImGui::SliderInt("Target dY", &m_world.m_ownRobot.m_dYTarget, -1000, 1000);
				ImGui::SliderInt("Target dTheta", &m_world.m_ownRobot.m_dThetaTarget, -20, 20);
				if (ImGui::Button("Stop") || sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
					m_world.m_ownRobot.m_dXTarget = 0;
					m_world.m_ownRobot.m_dYTarget = 0;
					m_world.m_ownRobot.m_dThetaTarget = 0;
				}

				break;
			case GAMEPAD_CONTROL:
				if (m_gamepadEnabled){
					m_world.m_ownRobot.m_gamepadPrimaryAxisSpeedTarget = (sf::Joystick::getAxisPosition(0, sf::Joystick::R) - sf::Joystick::getAxisPosition(0, sf::Joystick::Z))/2.f*10.f;
					m_world.m_ownRobot.m_gamepadSecondaryAxisSpeedTarget = -sf::Joystick::getAxisPosition(0, sf::Joystick::X)*10.f;
					m_world.m_ownRobot.m_dThetaTarget = -sf::Joystick::getAxisPosition(0, sf::Joystick::U)/50.f;
				}
				ImGui::SliderInt("Gamepad max speed percentage", &m_world.m_ownRobot.m_gamepadMaxSpeedPercentage, 0, 100);
				ImGui::SliderInt("Primary axis speed", &m_world.m_ownRobot.m_gamepadPrimaryAxisSpeedTarget, -1000, 1000);
				ImGui::SliderInt("Secondary axis speed", &m_world.m_ownRobot.m_gamepadSecondaryAxisSpeedTarget, -1000, 1000);
				ImGui::SliderInt("Target dTheta", &m_world.m_ownRobot.m_dThetaTarget, -20, 20);
				if (ImGui::Button("Stop") || sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
					m_world.m_ownRobot.m_gamepadPrimaryAxisSpeedTarget = 0;
					m_world.m_ownRobot.m_gamepadSecondaryAxisSpeedTarget = 0;
					m_world.m_ownRobot.m_dThetaTarget = 0;
				}

				break;
			case POSE_CONTROL:
				ImGui::SliderInt("Max speed percentage", &m_world.m_ownRobot.m_maxSpeedPercentage, 0, 100);
				ImGui::SliderFloat("Taget X", &m_world.m_ownRobot.m_targetPose.m_position.m_x, X_MIN_BORDER, X_MAX_BORDER, "%.1f");
				ImGui::SliderFloat("Target Y", &m_world.m_ownRobot.m_targetPose.m_position.m_y, Y_MIN_BORDER, Y_MAX_BORDER, "%.1f");
				ImGui::SliderFloat("Target Theta", &m_world.m_ownRobot.m_targetPose.m_orientation, -PI, PI);
				
				ImGui::Text("Coefficients");
				ImGui::Text("X & Y");
				ImGui::InputFloat("xyP", &m_world.m_ownRobot.m_xyPCoefficient, -1000.f, 1000.f, "%.6f");
				ImGui::InputFloat("xyI", &m_world.m_ownRobot.m_xyICoefficient, -1000.f, 1000.f, "%.6f");
				ImGui::InputFloat("xyD", &m_world.m_ownRobot.m_xyDCoefficient, -1000.f, 1000.f, "%.6f");
				ImGui::Text("Theta");
				ImGui::InputFloat("thetaP", &m_world.m_ownRobot.m_thetaPCoefficient, -1000.f, 1000.f, "%.6f");
				ImGui::InputFloat("thetaI", &m_world.m_ownRobot.m_thetaICoefficient, -1000.f, 1000.f, "%.6f");
				ImGui::InputFloat("thetaD", &m_world.m_ownRobot.m_thetaDCoefficient, -1000.f, 1000.f, "%.6f");
				
				if (ImGui::Button("Reset error integration")) {
					m_world.m_ownRobot.m_errorPoseIntegrated = Pose2f(Vec2f(0.f, 0.f), 0.f);
				}

				break;
		}
	} else {

	}
	ImGui::End();
}

void GraphicalUserInterface::ownRobotArmRoutine() {
	const char* commandsList[] =
	{
		"Stop / Wait",
		"Goto pos mot 1",
		"Goto pos mot 2",
		"Goto pos mot 3",
		"Goto memorized position",
		"Run memorized sequence",
	};

	static bool button0Pressed = 0, button1Pressed = 0, button2Pressed = 0, button3Pressed = 0;
	static bool button0PreviouslyPressed = 0, button1PreviouslyPressed = 0, button2PreviouslyPressed = 0, button3PreviouslyPressed = 0;

	ImGui::SetNextWindowSizeConstraints(ImVec2(200, -1), ImVec2(2000, -1));
	ImGui::Begin("Arm", 0, ImGuiWindowFlags_AlwaysAutoResize);
	if (m_world.m_ownRobot.m_periphericalsConfig.armEnabled) {
		ImGui::Text("Arm 1:");
		ImGui::Checkbox("Immediate command 1", &m_world.m_ownRobot.m_arm1.m_command.immediate);
		ImGui::Combo("Command 1", &m_world.m_ownRobot.m_arm1.m_command.commandType, commandsList, IM_ARRAYSIZE(commandsList));
		ImGui::SliderInt("Argument 1", &m_world.m_ownRobot.m_arm1.m_command.argument, 0, 15);
		if (ImGui::Button("Send command arm 1")) {
			m_world.m_ownRobot.m_arm1.m_isCommandToSend = true;
		}
		ImGui::Checkbox("Immediate command 2", &m_world.m_ownRobot.m_arm2.m_command.immediate);
		ImGui::Combo("Command 2", &m_world.m_ownRobot.m_arm2.m_command.commandType, commandsList, IM_ARRAYSIZE(commandsList));
		ImGui::SliderInt("Argument 2", &m_world.m_ownRobot.m_arm2.m_command.argument, 0, 15);
		if (ImGui::Button("Send command arm 2")) {
			m_world.m_ownRobot.m_arm2.m_isCommandToSend = true;
		}
		if (m_gamepadEnabled) {
			button0Pressed = sf::Joystick::isButtonPressed(0, 0);
			button1Pressed = sf::Joystick::isButtonPressed(0, 1);
			button2Pressed = sf::Joystick::isButtonPressed(0, 2);
			button3Pressed = sf::Joystick::isButtonPressed(0, 3);

			if (button1Pressed && !button1PreviouslyPressed) {
				m_world.m_ownRobot.m_arm1.m_command.immediate = true;
                m_world.m_ownRobot.m_arm1.m_command.commandType = 2;
                m_world.m_ownRobot.m_arm1.m_command.argument = 6;
                m_world.m_ownRobot.m_arm1.m_isCommandToSend = true;
			} else if (button2Pressed && !button2PreviouslyPressed) {
				m_world.m_ownRobot.m_arm1.m_command.immediate = true;
                m_world.m_ownRobot.m_arm1.m_command.commandType = 2;
                m_world.m_ownRobot.m_arm1.m_command.argument = 12;
                m_world.m_ownRobot.m_arm1.m_isCommandToSend = true;
			}

			if (button3Pressed && !button3PreviouslyPressed) {
                m_world.m_ownRobot.m_arm1.m_command.immediate = true;
                m_world.m_ownRobot.m_arm1.m_command.commandType = 1;
                m_world.m_ownRobot.m_arm1.m_command.argument = 13;
                m_world.m_ownRobot.m_arm1.m_isCommandToSend = true;
			} else if (button0Pressed && !button0PreviouslyPressed) {
				m_world.m_ownRobot.m_arm1.m_command.immediate = true;
                m_world.m_ownRobot.m_arm1.m_command.commandType = 1;
                m_world.m_ownRobot.m_arm1.m_command.argument = 7;
                m_world.m_ownRobot.m_arm1.m_isCommandToSend = true;
			}

			button0PreviouslyPressed = button0Pressed;
			button1PreviouslyPressed = button1Pressed;
			button2PreviouslyPressed = button2Pressed;
			button3PreviouslyPressed = button3Pressed;
		}
	}
	ImGui::End();
}

void GraphicalUserInterface::update() {
	homeMenuConfig();
	
	periphericalsConfigRoutine();
	/*if (m_world.m_runningMode != STRATEGY_MODE) {
		periphericalsConfigRoutine();
	}*/
	
	// Sensors routines
	cameraRoutine();

	// Actuators routines
	ownRobotRollingBaseRoutine();
	ownRobotArmRoutine();
	
	// TODO: Make it cleaner: Load texture only one time, not every frame
	/*sf::Texture boardTexture;
	boardTexture.loadFromFile("vinyle.png");
	sf::RectangleShape boardShape(sf::Vector2f(WIDTH, HEIGHT));
	boardShape.setTexture(&boardTexture);
	m_window.draw(boardShape);*/

	if (m_world.m_ownRobot.m_periphericalsConfig.rollingBaseEnabled) {
		m_world.m_ownRobot.m_targetPose.update();
		m_world.m_ownRobot.m_targetPose.draw(m_window);
	}
	
	sf::Vertex commandSpeedVectorShape[2];
	commandSpeedVectorShape[0] = sf::Vertex(m_world.m_ownRobot.m_currentPose.m_position.toSfmlVector(), sf::Color::Yellow);
	commandSpeedVectorShape[1] = sf::Vertex((m_world.m_ownRobot.m_currentPose.m_position + m_world.m_ownRobot.m_commandSpeed.m_position).toSfmlVector(), sf::Color::Yellow);
	m_window.draw(commandSpeedVectorShape, 2, sf::Lines);
	
	sf::Vertex currentSpeedVectorShape[2]; // TODO: Make it cleaner; eg: create a Vec2f method
	currentSpeedVectorShape[0] = sf::Vertex(m_world.m_ownRobot.m_currentPose.m_position.toSfmlVector(), sf::Color::Yellow);
	currentSpeedVectorShape[1] = sf::Vertex((m_world.m_ownRobot.m_currentPose.m_position + m_world.m_ownRobot.m_currentSpeed.m_position).toSfmlVector(), sf::Color::Green);
	m_window.draw(currentSpeedVectorShape, 2, sf::Lines);
	
	m_world.m_ownRobot.m_currentPose.update();
	m_world.m_ownRobot.m_currentPose.draw(m_window);
	
	
	m_world.m_advRobot.m_currentPose.update();
	m_world.m_advRobot.m_currentPose.draw(m_window);
}
