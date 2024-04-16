#include "gui/gui.hpp"

#include <imgui.h>
#include <imgui-SFML.h>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Joystick.hpp>

#include "parameters.hpp"

GraphicalUserInterface::GraphicalUserInterface(sf::RenderWindow& window, World& world): m_window(window), m_world(world) {

}

void GraphicalUserInterface::ownRobotPeriphericalsConfigRoutine() {
    ImGui::Begin("HAL configuration");
    ImGui::Checkbox("Camera connected", &m_world.m_ownRobot.m_periphericalsConfig.cameraConnected);
    ImGui::Checkbox("Rolling base connected", &m_world.m_ownRobot.m_periphericalsConfig.rollingBaseConnected);
    ImGui::Checkbox("Arm connected", &m_world.m_ownRobot.m_periphericalsConfig.armConnected);
    ImGui::End();
}

void GraphicalUserInterface::ownRobotCameraRoutine() {
	ImGui::SetNextWindowSizeConstraints(ImVec2(200, -1), ImVec2(2000, -1));
	ImGui::Begin("Camera", 0, ImGuiWindowFlags_AlwaysAutoResize);
	if (m_world.m_ownRobot.m_periphericalsConfig.cameraConnected) {
		/*float x = m_ownRobot.m_pose.m_position.m_x;
		float y = m_ownRobot.m_pose.m_position.m_y;
		float orientation = m_ownRobot.m_pose.m_orientation;
		ImGui::SliderFloat("Own robot X", &x, X_MIN_BORDER, X_MAX_BORDER);
		ImGui::SliderFloat("Own robot Y", &y, Y_MIN_BORDER, Y_MAX_BORDER);
		ImGui::SliderFloat("Own robot Theta", &orientation, 0, 2 * PI);*/
		ImGui::Text("Own robot X: %.1f", m_world.m_ownRobot.m_currentPose.m_position.m_x);
		ImGui::Text("Own robot Y: %.1f", m_world.m_ownRobot.m_currentPose.m_position.m_y);
		ImGui::Text("Own robot Theta: %.3f", m_world.m_ownRobot.m_currentPose.m_orientation);
	} else {
		//TODO: Complete with communication
		ImGui::SliderFloat("Own robot X", &m_world.m_ownRobot.m_currentPose.m_position.m_x, X_MIN_BORDER, X_MAX_BORDER, "%.1f");
		ImGui::SliderFloat("Own robot Y", &m_world.m_ownRobot.m_currentPose.m_position.m_y, Y_MIN_BORDER, Y_MAX_BORDER, "%.1f");
		ImGui::SliderFloat("Own robot Theta", &m_world.m_ownRobot.m_currentPose.m_orientation, 0, 2 * PI);
	}
	ImGui::End();
}

void GraphicalUserInterface::ownRobotRollingBaseRoutine() {
	static bool gamepadEnabled = false;
	static int mode = 0;
	const char* modesList[] =
	{
		"Individual motor speeds control",
		"dx dy dtheta control",
		"Position control"
	};
	
	ImGui::SetNextWindowSizeConstraints(ImVec2(200, -1), ImVec2(2000, -1));
	ImGui::Begin("Rolling base", 0, ImGuiWindowFlags_AlwaysAutoResize);
	if (m_world.m_ownRobot.m_periphericalsConfig.rollingBaseConnected) {
		ImGui::Checkbox("Enable motors", &m_world.m_ownRobot.m_motorsEnabled);
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
				ImGui::Checkbox("Enable gamepad", &gamepadEnabled);
				if (gamepadEnabled){
					m_world.m_ownRobot.m_dXTarget = sf::Joystick::getAxisPosition(0, sf::Joystick::V);
					m_world.m_ownRobot.m_dYTarget = sf::Joystick::getAxisPosition(0, sf::Joystick::U);
					m_world.m_ownRobot.m_dThetaTarget = sf::Joystick::getAxisPosition(0, sf::Joystick::X);
				}
				ImGui::SliderInt("Taget dX", &m_world.m_ownRobot.m_dXTarget, -100, 100);
				ImGui::SliderInt("Target dY", &m_world.m_ownRobot.m_dYTarget, -100, 100);
				ImGui::SliderInt("Target dTheta", &m_world.m_ownRobot.m_dThetaTarget, -100, 100);
				if (ImGui::Button("Stop") || sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
					m_world.m_ownRobot.m_dXTarget = 0;
					m_world.m_ownRobot.m_dYTarget = 0;
					m_world.m_ownRobot.m_dThetaTarget = 0;
				}

				break;
			case POSE_CONTROL:
				ImGui::SliderFloat("Taget X", &m_world.m_ownRobot.m_targetPose.m_position.m_x, X_MIN_BORDER, X_MAX_BORDER, "%.1f");
				ImGui::SliderFloat("Target Y", &m_world.m_ownRobot.m_targetPose.m_position.m_y, Y_MIN_BORDER, Y_MAX_BORDER, "%.1f");
				ImGui::SliderFloat("Target Theta", &m_world.m_ownRobot.m_targetPose.m_orientation, 0, 2 * PI);
				ImGui::Button("Goto target");

				break;
		}
	} else {

	}
	ImGui::End();
}

void GraphicalUserInterface::ownRobotArmRoutine() {
	static bool immediate = 0;
	static int command = 0, argument = 0;
	const char* commandsList[] =
	{
		"Stop / Wait",
		"Goto pos mot 1",
		"Goto pos mot 2",
		"Goto pos mot 3",
		"Goto memorized position",
		"Run memorized sequence",
	};

	ImGui::SetNextWindowSizeConstraints(ImVec2(200, -1), ImVec2(2000, -1));
	ImGui::Begin("Arm", 0, ImGuiWindowFlags_AlwaysAutoResize);
	if (m_world.m_ownRobot.m_periphericalsConfig.armConnected) {
		ImGui::Checkbox("Immediate command", &immediate);
		ImGui::Combo("Command", &command, commandsList, IM_ARRAYSIZE(commandsList));
		ImGui::SliderInt("Argument", &argument, 0, 15);
		ImGui::Button("Send command");
		//TODO: Complete with communication
	} else {

	}
	ImGui::End();
}

void GraphicalUserInterface::update() {
	ownRobotPeriphericalsConfigRoutine();
	
	// Sensors routines
	ownRobotCameraRoutine();

	// Actuators routines
	ownRobotRollingBaseRoutine();
	ownRobotArmRoutine();

	m_world.m_ownRobot.m_currentPose.update();
	m_world.m_ownRobot.m_currentPose.draw(m_window);
	if (m_world.m_ownRobot.m_periphericalsConfig.rollingBaseConnected) {
		m_world.m_ownRobot.m_targetPose.update();
		m_world.m_ownRobot.m_targetPose.draw(m_window);
	}
}