#include "gui/gui.hpp"

#include <imgui.h>
#include <imgui-SFML.h>

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
	ImGui::SetNextWindowSizeConstraints(ImVec2(200, -1), ImVec2(2000, -1));
	ImGui::Begin("Rolling base", 0, ImGuiWindowFlags_AlwaysAutoResize);
	if (m_world.m_ownRobot.m_periphericalsConfig.rollingBaseConnected) {
		ImGui::SliderFloat("Taget X", &m_world.m_ownRobot.m_targetPose.m_position.m_x, X_MIN_BORDER, X_MAX_BORDER, "%.1f");
		ImGui::SliderFloat("Target Y", &m_world.m_ownRobot.m_targetPose.m_position.m_y, Y_MIN_BORDER, Y_MAX_BORDER, "%.1f");
		ImGui::SliderFloat("Target Theta", &m_world.m_ownRobot.m_targetPose.m_orientation, 0, 2 * PI);
		ImGui::Button("Goto target");
		//TODO: Complete with communication
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