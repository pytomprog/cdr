#include "DebugGui.hpp"

#include <vector>
#include <string>
#include <iostream>
#include <imgui.h>
#include <imgui-SFML.h>

#include "parameters.hpp"
#include "pathfinder.hpp"

void DebugGui::update(sf::RenderWindow& window, Vec2f& startPoint, Vec2f& endPoint, std::vector<LineSegment>& path, std::vector<Circle>& obstacles) {
    ImGui::SFML::Update(window, m_deltaClock.restart());

	//ImGui::ShowDemoWindow();
	
	ImGui::Begin("My tools");
	ImGui::SliderFloat("Start point X", &startPoint.m_x, X_MIN_BORDER, X_MAX_BORDER);
	ImGui::SliderFloat("Start point Y", &startPoint.m_y, Y_MIN_BORDER, Y_MAX_BORDER);
	ImGui::SliderFloat("End point X", &endPoint.m_x, X_MIN_BORDER, X_MAX_BORDER);
	ImGui::SliderFloat("End point Y", &endPoint.m_y, Y_MIN_BORDER, Y_MAX_BORDER);
	if (ImGui::Button("Recalculate path")) {
		path = findPath(startPoint, endPoint, obstacles);
	}
	ImGui::End();
	
	
	
	ImGui::Begin("Obstacles");
	ImGui::ColorEdit4("Obstacles Color", &m_obstaclesColor.r);
	
	if (ImGui::Button("Add obstacle")) {
		obstacles.push_back(Circle(Vec2f(100.f, 100.f, sf::Color::Magenta), 100.f));
	}
	
	if (ImGui::BeginTable("##split", 2, ImGuiTableFlags_BordersOuter | ImGuiTableFlags_Resizable | ImGuiTableFlags_ScrollY)) {
        ImGui::TableSetupScrollFreeze(0, 1);
        ImGui::TableSetupColumn("Obstacle");
        ImGui::TableSetupColumn("Value");
        ImGui::TableHeadersRow();
        
        for (int uid=0; uid<obstacles.size(); uid++) {
			ImGui::PushID(uid);
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::AlignTextToFramePadding();
			bool nodeOpen = ImGui::TreeNode("Object", "%s %u", "Obstacle", uid);
			ImGui::TableSetColumnIndex(1);
			//ImGui::Text("SOME TEXT");

			if (nodeOpen) {
				std::vector<std::string> propertiesName { "X", "Y", "Radius" };
				for (int i = 0; i < 3; i++) {
					ImGui::PushID(i); // Use field index as identifier.
					// Here we use a TreeNode to highlight on hover (we could use e.g. Selectable as well)
					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex(0);
					ImGui::AlignTextToFramePadding();
					ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Bullet;
					
					ImGui::TreeNodeEx("Property", flags, &propertiesName[i][0]);
					ImGui::TableSetColumnIndex(1);
					ImGui::SetNextItemWidth(-FLT_MIN);
					
					switch (i) {
						case 0:
							ImGui::SliderFloat("##value", &obstacles[uid].m_center.m_x, X_MIN_BORDER, X_MAX_BORDER);
							break;
						case 1:
							ImGui::SliderFloat("##value", &obstacles[uid].m_center.m_y, Y_MIN_BORDER, Y_MAX_BORDER);
							break;
						case 2:
							ImGui::SliderFloat("##value", &obstacles[uid].m_radius, 0, MAX_OBSTACLE_RADIUS);
							break;
					}
					
					ImGui::NextColumn();
					ImGui::PopID();
				}
				
				ImGui::PushID(4); // Use field index as identifier.
				// Here we use a TreeNode to highlight on hover (we could use e.g. Selectable as well)
				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);
				ImGui::AlignTextToFramePadding();
				ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Bullet;
					
				if (ImGui::Button("Remove")) {
					obstacles.erase(obstacles.begin()+uid);
				}
				
				ImGui::TreePop();
			}
			ImGui::PopID();
		}
        ImGui::EndTable();
    }
	ImGui::End();
}
