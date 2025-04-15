#define IMGUI_DEFINE_MATH_OPERATORS
#include "nodalStrategy/DebugGui.hpp"

#include <vector>
#include <string>
#include <iostream>
#include <imgui.h>
#include <imgui_internal.h>
#include <imgui-SFML.h>

#include "parameters.hpp"

ImColor getPinTextColor(PinType type) {
	switch (type) {
		default:
		case PinType::Flow:     return ImColor(255, 255, 255);
		case PinType::Bool:     return ImColor(220,  48,  48);
		case PinType::Int:      return ImColor( 68, 201, 156);
		case PinType::Float:    return ImColor(147, 226,  74);
		case PinType::String:   return ImColor(124,  21, 153);
		case PinType::Object:   return ImColor( 51, 150, 215);
		case PinType::Function: return ImColor(218,   0, 183);
	}
};

DebugGui::DebugGui() {
	NodeEditor::Config config;
	config.SettingsFile = "Simple.json";
	m_contextPtr = NodeEditor::CreateEditor(&config);
	if (m_contextPtr == nullptr) {
		std::cerr << "contextPtr pointing to nullptr" << std::endl;
	}
	
	/*for (int nodeId = 0; nodeId < 5; nodeId++) {
		spawnFirstNode();
	}*/
}

int DebugGui::getNextId() {
	return m_nextId++; 
}

NodeEditor::LinkId DebugGui::getNextLinkId() {
	return NodeEditor::LinkId(getNextId());
}

Node* DebugGui::findNode(NodeEditor::NodeId id) {
	for (auto& node : m_nodes)
		if (node.id == id)
			return &node;
	
	return nullptr;
}

Link* DebugGui::findLink(NodeEditor::LinkId id) {
	for (auto& link : m_links)
		if (link.id == id)
			return &link;
	
	return nullptr;
}

Pin* DebugGui::findPin(NodeEditor::PinId id) {
	if (!id)
		return nullptr;

	for (auto& node : m_nodes){
		for (auto& pin : node.inputs)
			if (pin.id == id)
				return &pin;

		for (auto& pin : node.outputs)
			if (pin.id == id)
				return &pin;
	}
	
	return nullptr;
}

void DebugGui::buildNode(Node* node) {
	for (auto& input : node->inputs) {
		input.node = node;
		input.kind = PinKind::Input;
	}

	for (auto& output : node->outputs) {
		output.node = node;
		output.kind = PinKind::Output;
	}
}

void DebugGui::buildNodes() {
	for (auto& node : m_nodes)
		buildNode(&node);
}

Node* DebugGui::spawnFirstNode() {
	m_nodes.emplace_back(getNextId(), "First Node", ImColor(255, 128, 128));
	Node& node = m_nodes.back();
	node.type = NodeType::Blueprint;
	node.inputs.emplace_back(getNextId(), "In flow", PinType::Flow);
	node.inputs.emplace_back(getNextId(), "Param 1", PinType::Float);
	node.inputs.emplace_back(getNextId(), "Param 2", PinType::Bool);
	node.outputs.emplace_back(getNextId(), "Out flow 1", PinType::Flow);
	node.outputs.emplace_back(getNextId(), "Out flow 2", PinType::Flow);

	buildNode(&node);

	return &node;
}

Node* DebugGui::spawnSecondNode() {
	m_nodes.emplace_back(getNextId(), "Second Node", ImColor(255, 128, 128));
	Node& node = m_nodes.back();
	node.type = NodeType::Simple;
	node.outputs.emplace_back(getNextId(), "Param 1", PinType::Float);
	node.outputs.emplace_back(getNextId(), "Param 2", PinType::Bool);

	buildNode(&node);

	return &node;
}

void DebugGui::update(sf::RenderWindow& window) {
	static Pin* newLinkPin = nullptr;
	
    ImGui::SFML::Update(window, m_deltaClock.restart());
	
	ImGui::ShowDemoWindow();
	
	ImGui::Begin("Nodal strategy");
		ImGui::Text("Hi there");
		
		NodeEditor::SetCurrentEditor(m_contextPtr);
		NodeEditor::Begin("My editor");
		
		ImFont titleFont = *ImGui::GetFont();
		titleFont.Scale *= 1.2;
		
		for (auto& node : m_nodes) {
			const auto isSimple = node.type == NodeType::Simple;
			
			NodeEditor::BeginNode(node.id);
			
			if (!isSimple) {
				ImGui::PushFont(&titleFont);
				ImGui::TextUnformatted(node.name.c_str());
				ImGui::PopFont();
				//ImGui::Separator();
			}
				
			if (node.inputs.size() > 0) {
				ImGui::BeginGroup();			
					for (auto& input : node.inputs) {
						NodeEditor::BeginPin(input.id, NodeEditor::PinKind::Input);
						ImGui::PushStyleColor(ImGuiCol_Text, (ImVec4)getPinTextColor(input.type));
						if (!input.name.empty()) {
							ImGui::TextUnformatted(input.name.c_str());
						}
						if (input.type == PinType::Bool) {
							ImGui::SameLine();
							ImGui::Button("Hello");
						}
						ImGui::PopStyleColor();
						NodeEditor::EndPin();
					}
				ImGui::EndGroup();
			}
			
			if (isSimple) {
				if (node.inputs.size() > 0) {
					ImGui::SameLine();
				}
				ImGui::PushFont(&titleFont);
				ImGui::TextUnformatted(node.name.c_str());
				ImGui::PopFont();
			}
			
			
			if (node.outputs.size() > 0) {
				if (node.inputs.size() > 0 || isSimple) {
					ImGui::SameLine();
				}
				ImGui::BeginGroup();
					for (auto& output : node.outputs) {
						NodeEditor::BeginPin(output.id, NodeEditor::PinKind::Output);
						ImGui::PushStyleColor(ImGuiCol_Text, (ImVec4)getPinTextColor(output.type));
						if (output.type == PinType::String) {
							static char buffer[128] = "Edit Me\nMultiline!";
							static bool wasActive = false;

							ImGui::PushItemWidth(100.0f);
							ImGui::InputText("##edit", buffer, 127);
							ImGui::PopItemWidth();
							if (ImGui::IsItemActive() && !wasActive) {
								NodeEditor::EnableShortcuts(false);
								wasActive = true;
							}
							else if (!ImGui::IsItemActive() && wasActive) {
								NodeEditor::EnableShortcuts(true);
								wasActive = false;
							}
						}
						if (!output.name.empty()) {
							if (output.type == PinType::String)
								ImGui::SameLine();
							ImGui::TextUnformatted(output.name.c_str());
						}
						ImGui::PopStyleColor();
						NodeEditor::EndPin();
					}
				ImGui::EndGroup();
			}
				
			NodeEditor::EndNode();
		}
				
		for (auto& link : m_links)
			NodeEditor::Link(link.id, link.startPinId, link.endPinId, link.color, 2.0f);
		
				
		// See exemple basic-interaction-example.cpp in the node editor repo on github
		// Handle creation action, returns true if editor want to create new object (node or link)
		if (NodeEditor::BeginCreate()) {
			auto showLabel = [](const char* label, ImColor color) {
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() - ImGui::GetTextLineHeight());
				auto size = ImGui::CalcTextSize(label);

				auto padding = ImGui::GetStyle().FramePadding;
				auto spacing = ImGui::GetStyle().ItemSpacing;

				ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(spacing.x, -spacing.y));

				auto rectMin = ImGui::GetCursorScreenPos() - padding;
				auto rectMax = ImGui::GetCursorScreenPos() + size + padding;

				auto drawList = ImGui::GetWindowDrawList();
				drawList->AddRectFilled(rectMin, rectMax, color, size.y * 0.15f);
				ImGui::TextUnformatted(label);
			};
			
			NodeEditor::PinId startPinId = 0, endPinId = 0;
			if (NodeEditor::QueryNewLink(&startPinId, &endPinId)) {
				auto startPin = findPin(startPinId);
				auto endPin = findPin(endPinId);
				newLinkPin = startPin ? startPin : endPin;

				if (startPin->kind == PinKind::Input) {
					std::swap(startPin, endPin);
					std::swap(startPinId, endPinId);
				}

				if (startPin && endPin) {
					if (endPin == startPin) {
						NodeEditor::RejectNewItem(ImColor(255, 0, 0), 2.0f);
					} else if (endPin->kind == startPin->kind) {
						showLabel("x Incompatible Pin Kind", ImColor(45, 32, 32, 180));
						NodeEditor::RejectNewItem(ImColor(255, 0, 0), 2.0f);
					} else if (endPin->node == startPin->node) {
					    showLabel("x Cannot connect to self", ImColor(45, 32, 32, 180));
					    NodeEditor::RejectNewItem(ImColor(255, 0, 0), 1.0f);
					} else if (endPin->type != startPin->type) {
						showLabel("x Incompatible Pin Type", ImColor(45, 32, 32, 180));
						NodeEditor::RejectNewItem(ImColor(255, 128, 128), 1.0f);
					} else {
						showLabel("+ Create Link", ImColor(32, 45, 32, 180));
						if (NodeEditor::AcceptNewItem(ImColor(128, 255, 128), 4.0f)) {
							m_links.emplace_back(Link(getNextLinkId(), startPinId, endPinId));
							m_links.back().color = getPinTextColor(startPin->type);
						}
					}
				}
			}
		}
		NodeEditor::EndCreate();
		
		if (NodeEditor::BeginDelete()) {
			NodeEditor::LinkId deletedLinkId;
			while (NodeEditor::QueryDeletedLink(&deletedLinkId)) {
				if (NodeEditor::AcceptDeletedItem()) {
					auto id = std::find_if(m_links.begin(), m_links.end(), [deletedLinkId](auto& link) { return link.id == deletedLinkId; });
					if (id != m_links.end())
						m_links.erase(id);
				}
			}
		}
		NodeEditor::EndDelete();
		
		
		auto openPopupPosition = ImGui::GetMousePos();
        NodeEditor::Suspend();
        if (NodeEditor::ShowBackgroundContextMenu()) {
            ImGui::OpenPopup("Create New Node");
        }
        NodeEditor::Resume();
        
        NodeEditor::Suspend();
        if (ImGui::BeginPopup("Create New Node")) {
            auto newNodePostion = openPopupPosition;

            //auto drawList = ImGui::GetWindowDrawList();
            //drawList->AddCircleFilled(ImGui::GetMousePosOnOpeningCurrentPopup(), 10.0f, 0xFFFF00FF);

            Node* node = nullptr;
            if (ImGui::MenuItem("First Node"))
                node = spawnFirstNode();
            if (ImGui::MenuItem("Second Node"))
                node = spawnSecondNode();
            
            if (node) {
                NodeEditor::SetNodePosition(node->id, newNodePostion);
            }

            ImGui::EndPopup();
        }
        NodeEditor::Resume();

		NodeEditor::End();
	ImGui::End();
	
	NodeEditor::SetCurrentEditor(nullptr);
}
