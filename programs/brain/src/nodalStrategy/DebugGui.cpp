#define IMGUI_DEFINE_MATH_OPERATORS
#include "nodalStrategy/DebugGui.hpp"

#include <vector>
#include <string>
#include <iostream>
#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_stdlib.h>
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

DebugGui::DebugGui(StrategyStateMachine& stateMachine): m_stateMachine(stateMachine) {
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
		input.nodeId = node->id;
		input.kind = PinKind::Input;
	}

	for (auto& output : node->outputs) {
		output.nodeId = node->id;
		output.kind = PinKind::Output;
	}
}

void DebugGui::buildNodes() {
	for (auto& node : m_nodes)
		buildNode(&node);
}

Node* DebugGui::spawnIdleStateNode(std::shared_ptr<IdleState> statePtr) {
	m_nodes.emplace_back(getNextId(), "Idle state", ImColor(255, 128, 128));
	Node& node = m_nodes.back();
	node.type = NodeType::Blueprint;
	node.inputs.emplace_back(getNextId(), "In flow", PinType::Flow);

	buildNode(&node);
	
	if (statePtr == nullptr) {
		statePtr = std::make_shared<IdleState>(&m_stateMachine);
		m_stateMachine.addState(statePtr);
	}
	node.m_statePtr = statePtr;
	
	return &node;
}

Node* DebugGui::spawnDemoNode() {
	m_nodes.emplace_back(getNextId(), "Demo Node", ImColor(255, 128, 128));
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

Node* DebugGui::spawnTestStateNode(std::shared_ptr<TestState> statePtr) {
	m_nodes.emplace_back(getNextId(), "Test state", ImColor(255, 128, 128));
	Node& node = m_nodes.back();
	node.type = NodeType::Blueprint;
	node.inputs.emplace_back(getNextId(), "In flow", PinType::Flow);
	node.inputs.emplace_back(getNextId(), "Start message", PinType::String);
	node.inputs.emplace_back(getNextId(), "Loop message", PinType::String);
	node.outputs.emplace_back(getNextId(), "Out flow", PinType::Flow);

	buildNode(&node);
	
	if (statePtr == nullptr) {
		statePtr = std::make_shared<TestState>(&m_stateMachine);
		m_stateMachine.addState(statePtr);
	}
	node.m_statePtr = statePtr;

	return &node;
}

Node* DebugGui::spawnStartStateNode(std::shared_ptr<StartState> statePtr) {
	m_nodes.emplace_back(getNextId(), "Start state", ImColor(255, 128, 128));
	Node& node = m_nodes.back();
	node.type = NodeType::Blueprint;
	node.outputs.emplace_back(getNextId(), "Out flow", PinType::Flow);

	buildNode(&node);
	
	if (statePtr == nullptr) {
		statePtr = std::make_shared<StartState>(&m_stateMachine);
		m_stateMachine.addState(statePtr);
	}
	node.m_statePtr = statePtr;
	
	return &node;
}

Node* DebugGui::spawnTiretteStateNode(std::shared_ptr<TiretteState> statePtr) {
	m_nodes.emplace_back(getNextId(), "Tirette state", ImColor(255, 128, 128));
	Node& node = m_nodes.back();
	node.type = NodeType::Blueprint;
	node.inputs.emplace_back(getNextId(), "In flow", PinType::Flow);
	node.outputs.emplace_back(getNextId(), "Out flow", PinType::Flow);

	buildNode(&node);
	
	if (statePtr == nullptr) {
		statePtr = std::make_shared<TiretteState>(&m_stateMachine);
		m_stateMachine.addState(statePtr);
	}
	node.m_statePtr = statePtr;
	
	return &node;
}

Node* DebugGui::spawnMovetoStateNode(std::shared_ptr<MovetoState> statePtr) {
	m_nodes.emplace_back(getNextId(), "Moveto state", ImColor(255, 128, 128));
	Node& node = m_nodes.back();
	node.type = NodeType::Blueprint;
	node.inputs.emplace_back(getNextId(), "In flow", PinType::Flow);
	node.inputs.emplace_back(getNextId(), "x", PinType::Float);
	node.inputs.emplace_back(getNextId(), "y", PinType::Float);
	node.inputs.emplace_back(getNextId(), "theta", PinType::Float);
	node.outputs.emplace_back(getNextId(), "Out flow", PinType::Flow);

	buildNode(&node);
	
	if (statePtr == nullptr) {
		statePtr = std::make_shared<MovetoState>(&m_stateMachine);
		m_stateMachine.addState(statePtr);
	}
	node.m_statePtr = statePtr;
	
	return &node;
}

Node* DebugGui::spawnMoveThroughStateNode(std::shared_ptr<MoveThroughState> statePtr) {
	m_nodes.emplace_back(getNextId(), "Move through state", ImColor(255, 128, 128));
	Node& node = m_nodes.back();
	node.type = NodeType::Blueprint;
	node.inputs.emplace_back(getNextId(), "In flow", PinType::Flow);
	node.inputs.emplace_back(getNextId(), "x", PinType::Float);
	node.inputs.emplace_back(getNextId(), "y", PinType::Float);
	node.inputs.emplace_back(getNextId(), "theta", PinType::Float);
	node.outputs.emplace_back(getNextId(), "Out flow", PinType::Flow);

	buildNode(&node);
	
	if (statePtr == nullptr) {
		statePtr = std::make_shared<MoveThroughState>(&m_stateMachine);
		m_stateMachine.addState(statePtr);
	}
	node.m_statePtr = statePtr;
	
	return &node;
}

Node* DebugGui::spawnChangeVacuumStateStateNode(std::shared_ptr<ChangeVacuumStateState> statePtr) {
	m_nodes.emplace_back(getNextId(), "Change vacuum state state", ImColor(255, 128, 128));
	Node& node = m_nodes.back();
	node.type = NodeType::Blueprint;
	node.inputs.emplace_back(getNextId(), "In flow", PinType::Flow);
	node.inputs.emplace_back(getNextId(), "outer", PinType::Bool);
	node.inputs.emplace_back(getNextId(), "inner", PinType::Bool);
	node.inputs.emplace_back(getNextId(), "planck", PinType::Bool);
	node.inputs.emplace_back(getNextId(), "newState", PinType::Bool);
	node.outputs.emplace_back(getNextId(), "Out flow", PinType::Flow);

	buildNode(&node);
	
	if (statePtr == nullptr) {
		statePtr = std::make_shared<ChangeVacuumStateState>(&m_stateMachine);
		m_stateMachine.addState(statePtr);
	}
	node.m_statePtr = statePtr;
	
	return &node;
}

Node* DebugGui::spawnSetStepperPosStateNode(std::shared_ptr<SetStepperPosState> statePtr) {
	m_nodes.emplace_back(getNextId(), "Set stepper pos state", ImColor(255, 128, 128));
	Node& node = m_nodes.back();
	node.type = NodeType::Blueprint;
	node.inputs.emplace_back(getNextId(), "In flow", PinType::Flow);
	node.inputs.emplace_back(getNextId(), "pos", PinType::Int);
	node.outputs.emplace_back(getNextId(), "Out flow", PinType::Flow);

	buildNode(&node);
	
	if (statePtr == nullptr) {
		statePtr = std::make_shared<SetStepperPosState>(&m_stateMachine);
		m_stateMachine.addState(statePtr);
	}
	node.m_statePtr = statePtr;
	
	return &node;
}

Node* DebugGui::spawnSetServoPosStateNode(std::shared_ptr<SetServoPosState> statePtr) {
	m_nodes.emplace_back(getNextId(), "Set servo pos state", ImColor(255, 128, 128));
	Node& node = m_nodes.back();
	node.type = NodeType::Blueprint;
	node.inputs.emplace_back(getNextId(), "In flow", PinType::Flow);
	node.inputs.emplace_back(getNextId(), "pos", PinType::Int);
	node.outputs.emplace_back(getNextId(), "Out flow", PinType::Flow);

	buildNode(&node);
	
	if (statePtr == nullptr) {
		statePtr = std::make_shared<SetServoPosState>(&m_stateMachine);
		m_stateMachine.addState(statePtr);
	}
	node.m_statePtr = statePtr;
	
	return &node;
}

Node* DebugGui::spawnWaitStateNode(std::shared_ptr<WaitState> statePtr) {
	m_nodes.emplace_back(getNextId(), "Wait state", ImColor(255, 128, 128));
	Node& node = m_nodes.back();
	node.type = NodeType::Blueprint;
	node.inputs.emplace_back(getNextId(), "In flow", PinType::Flow);
	node.inputs.emplace_back(getNextId(), "duration", PinType::Float);
	node.outputs.emplace_back(getNextId(), "Out flow", PinType::Flow);

	buildNode(&node);
	
	if (statePtr == nullptr) {
		statePtr = std::make_shared<WaitState>(&m_stateMachine);
		m_stateMachine.addState(statePtr);
	}
	node.m_statePtr = statePtr;
	
	return &node;
}

void DebugGui::update(sf::RenderWindow& window) {
	static Pin* newLinkPin = nullptr;
	
	//ImGui::ShowDemoWindow();
	
	ImGui::Begin("Nodal strategy");
		ImGui::PushItemWidth(250);
		ImGui::InputText("Strategy filename", &m_strategyFilename);
		ImGui::SameLine();
		if (ImGui::Button("Load strategy")) {
			loadNodes();
		}
		ImGui::SameLine();
		if (ImGui::Button("Save strategy")) {
			m_stateMachine.saveStatesToYaml(m_strategyFilename);
		}
		ImGui::SameLine();
		if (ImGui::Button("Load blue match")) {
			m_strategyFilename = "blue.yaml";
			loadNodes();
		}
		ImGui::SameLine();
		if (ImGui::Button("Load yellow match")) {
			m_strategyFilename = "yellow.yaml";
			loadNodes();
		}
		
		NodeEditor::SetCurrentEditor(m_contextPtr);
		NodeEditor::Begin("My editor");
		
		ImFont titleFont = *ImGui::GetFont();
		titleFont.Scale *= 1.4;
		
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
						bool noElement = false;
						if (input.type == PinType::Bool) {
							ImGui::SameLine();
							ImGui::Button("Hello");
						} else if (input.type == PinType::String) {
							static char buffer[128] = "Edit Me multiline!";
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
						} else if (input.type == PinType::Float) {
							static float val = 0.f;
							static bool wasActive = false;

							ImGui::PushItemWidth(100.0f);
							ImGui::InputFloat("", &val, 0.f, 3000.f);
							ImGui::PopItemWidth();
							if (ImGui::IsItemActive() && !wasActive) {
								NodeEditor::EnableShortcuts(false);
								wasActive = true;
							}
							else if (!ImGui::IsItemActive() && wasActive) {
								NodeEditor::EnableShortcuts(true);
								wasActive = false;
							}
						} else {
							noElement = true;
						}
						if (!input.name.empty()) {
							if (!noElement)
								ImGui::SameLine();
							ImGui::TextUnformatted(input.name.c_str());
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
		
		std::cout << m_links.size() << std::endl;
		for (auto& link : m_links) {
			std::cout << link.startPinId.AsPointer() << "->" << findPin(link.startPinId) << "->" << findNode(findPin(link.startPinId)->nodeId)->name  <<  ", " << link.endPinId.AsPointer() << "->" << findPin(link.endPinId) << "->" << findNode(findPin(link.endPinId)->nodeId)->name << std::endl;
			if (!link.startPinId.AsPointer() || !link.endPinId.AsPointer()) {
				std::cout << "Link pin not working" << std::endl;
			}
			NodeEditor::Link(link.id, link.startPinId, link.endPinId, link.color, 2.0f);
		}
		
				
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
					} else if (endPin->nodeId == startPin->nodeId) {
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
							Node* startNode = findNode(startPin->nodeId);
							Node* endNode = findNode(endPin->nodeId);
							std::string startStateType = startNode->m_statePtr->getTypeString();
							if (startStateType == "TestState") {
								std::static_pointer_cast<TestState>(startNode->m_statePtr)->m_nextStatePtr = endNode->m_statePtr;
							} else if (startStateType == "StartState") {
								std::static_pointer_cast<StartState>(startNode->m_statePtr)->m_nextStatePtr = endNode->m_statePtr;
							} else if (startStateType == "TiretteState") {
								std::static_pointer_cast<TiretteState>(startNode->m_statePtr)->m_nextStatePtr = endNode->m_statePtr;
							} else if (startStateType == "MovetoState") {
								std::static_pointer_cast<MovetoState>(startNode->m_statePtr)->m_nextStatePtr = endNode->m_statePtr;
							} else if (startStateType == "MoveThroughState") {
								std::static_pointer_cast<MoveThroughState>(startNode->m_statePtr)->m_nextStatePtr = endNode->m_statePtr;
							} else if (startStateType == "ChangeVacuumStateState") {
								std::static_pointer_cast<ChangeVacuumStateState>(startNode->m_statePtr)->m_nextStatePtr = endNode->m_statePtr;
							} else if (startStateType == "SetStepperPosState") {
								std::static_pointer_cast<SetStepperPosState>(startNode->m_statePtr)->m_nextStatePtr = endNode->m_statePtr;
							} else if (startStateType == "SetServoPosState") {
								std::static_pointer_cast<SetServoPosState>(startNode->m_statePtr)->m_nextStatePtr = endNode->m_statePtr;
							} else if (startStateType == "WaitState") {
								std::static_pointer_cast<WaitState>(startNode->m_statePtr)->m_nextStatePtr = endNode->m_statePtr;
							}
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
            if (ImGui::MenuItem("Demo node"))
                node = spawnDemoNode();
            if (ImGui::MenuItem("Idle state"))
                node = spawnIdleStateNode();
            if (ImGui::MenuItem("Test state"))
                node = spawnTestStateNode();
            if (ImGui::MenuItem("Start state"))
                node = spawnStartStateNode();
            if (ImGui::MenuItem("Tirette state"))
                node = spawnTiretteStateNode();
            if (ImGui::MenuItem("Moveto state"))
                node = spawnMovetoStateNode();
            if (ImGui::MenuItem("Move through state"))
                node = spawnMoveThroughStateNode();
			if (ImGui::MenuItem("Change vacuum state state"))
				node = spawnChangeVacuumStateStateNode();
			if (ImGui::MenuItem("Set stepper pos state"))
				node = spawnSetStepperPosStateNode();
			if (ImGui::MenuItem("Set servo pos state"))
				node = spawnSetServoPosStateNode();
			if (ImGui::MenuItem("Wait state"))
				node = spawnWaitStateNode();
            
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

void DebugGui::loadNodes() {
	m_nodes.clear();
	m_links.clear();
	
	m_stateMachine.loadStatesFromYaml(m_strategyFilename);
	
	for (std::shared_ptr<StrategyState> state: m_stateMachine.m_statePtrs) {
		Node* node = nullptr;
		std::string type = state->getTypeString();
		if (type == "IdleState") {
			node = spawnIdleStateNode(std::static_pointer_cast<IdleState>(state));
		} else if (type == "TestState") {
			node = spawnTestStateNode(std::static_pointer_cast<TestState>(state));
		} else if (type == "StartState") {
			node = spawnStartStateNode(std::static_pointer_cast<StartState>(state));
		} else if (type == "TiretteState") {
			node = spawnTiretteStateNode(std::static_pointer_cast<TiretteState>(state));
		} else if (type == "MovetoState") {
			node = spawnMovetoStateNode(std::static_pointer_cast<MovetoState>(state));
		} else if (type == "MoveThroughState") {
			node = spawnMoveThroughStateNode(std::static_pointer_cast<MoveThroughState>(state));
		} else if (type == "ChangeVacuumStateState") {
			node = spawnChangeVacuumStateStateNode(std::static_pointer_cast<ChangeVacuumStateState>(state));
		} else if (type == "SetStepperPosState") {
			node = spawnSetStepperPosStateNode(std::static_pointer_cast<SetStepperPosState>(state));
		} else if (type == "SetServoPosState") {
			node = spawnSetServoPosStateNode(std::static_pointer_cast<SetServoPosState>(state));
		} else if (type == "WaitState") {
			node = spawnWaitStateNode(std::static_pointer_cast<WaitState>(state));
		} else {
			std::cerr << "Unknown state type: " << type << std::endl;
			continue;
		}
	}
	
	for (auto& currentNode : m_nodes){
		if (currentNode.m_statePtr == nullptr)
			continue;
		
		std::string type = currentNode.m_statePtr->getTypeString();
		if (type == "TestState") {
			std::shared_ptr<TestState> currentStatePtr = std::static_pointer_cast<TestState>(currentNode.m_statePtr);
			if (currentStatePtr->m_nextStatePtr == nullptr)
				continue;
			
			Node* nextNodePtr = nullptr;
			for (auto& node : m_nodes) {
				if (node.m_statePtr == currentStatePtr->m_nextStatePtr)
					nextNodePtr = &node;
			}
			
			m_links.emplace_back(Link(getNextLinkId(), currentNode.outputs[0].id, nextNodePtr->inputs[0].id));
			m_links.back().color = getPinTextColor(currentNode.outputs[0].type);
		} else if (type == "StartState") {
			std::shared_ptr<StartState> currentStatePtr = std::static_pointer_cast<StartState>(currentNode.m_statePtr);
			if (currentStatePtr->m_nextStatePtr == nullptr)
				continue;
			
			Node* nextNodePtr = nullptr;
			for (auto& node : m_nodes) {
				if (node.m_statePtr == currentStatePtr->m_nextStatePtr)
					nextNodePtr = &node;
			}
			
			m_links.emplace_back(Link(getNextLinkId(), currentNode.outputs[0].id, nextNodePtr->inputs[0].id));
			m_links.back().color = getPinTextColor(currentNode.outputs[0].type);
		} else if (type == "TiretteState") {
			std::shared_ptr<TiretteState> currentStatePtr = std::static_pointer_cast<TiretteState>(currentNode.m_statePtr);
			if (currentStatePtr->m_nextStatePtr == nullptr)
				continue;
			
			Node* nextNodePtr = nullptr;
			for (auto& node : m_nodes) {
				if (node.m_statePtr == currentStatePtr->m_nextStatePtr)
					nextNodePtr = &node;
			}
			
			m_links.emplace_back(Link(getNextLinkId(), currentNode.outputs[0].id, nextNodePtr->inputs[0].id));
			m_links.back().color = getPinTextColor(currentNode.outputs[0].type);
		} else if (type == "MovetoState") {
			std::shared_ptr<MovetoState> currentStatePtr = std::static_pointer_cast<MovetoState>(currentNode.m_statePtr);
			if (currentStatePtr->m_nextStatePtr == nullptr)
				continue;
			
			Node* nextNodePtr = nullptr;
			for (auto& node : m_nodes) {
				if (node.m_statePtr == currentStatePtr->m_nextStatePtr)
					nextNodePtr = &node;
			}
			
			m_links.emplace_back(Link(getNextLinkId(), currentNode.outputs[0].id, nextNodePtr->inputs[0].id));
			m_links.back().color = getPinTextColor(currentNode.outputs[0].type);
		} else if (type == "MoveThroughState") {
			std::shared_ptr<MoveThroughState> currentStatePtr = std::static_pointer_cast<MoveThroughState>(currentNode.m_statePtr);
			if (currentStatePtr->m_nextStatePtr == nullptr)
				continue;
			
			Node* nextNodePtr = nullptr;
			for (auto& node : m_nodes) {
				if (node.m_statePtr == currentStatePtr->m_nextStatePtr)
					nextNodePtr = &node;
			}
			
			m_links.emplace_back(Link(getNextLinkId(), currentNode.outputs[0].id, nextNodePtr->inputs[0].id));
			m_links.back().color = getPinTextColor(currentNode.outputs[0].type);
		} else if (type == "ChangeVacuumStateState") {
			std::shared_ptr<ChangeVacuumStateState> currentStatePtr = std::static_pointer_cast<ChangeVacuumStateState>(currentNode.m_statePtr);
			if (currentStatePtr->m_nextStatePtr == nullptr)
				continue;
			
			Node* nextNodePtr = nullptr;
			for (auto& node : m_nodes) {
				if (node.m_statePtr == currentStatePtr->m_nextStatePtr)
					nextNodePtr = &node;
			}
			
			m_links.emplace_back(Link(getNextLinkId(), currentNode.outputs[0].id, nextNodePtr->inputs[0].id));
			m_links.back().color = getPinTextColor(currentNode.outputs[0].type);
		} else if (type == "SetStepperPosState") {
			std::shared_ptr<SetStepperPosState> currentStatePtr = std::static_pointer_cast<SetStepperPosState>(currentNode.m_statePtr);
			if (currentStatePtr->m_nextStatePtr == nullptr)
				continue;
			
			Node* nextNodePtr = nullptr;
			for (auto& node : m_nodes) {
				if (node.m_statePtr == currentStatePtr->m_nextStatePtr)
					nextNodePtr = &node;
			}
			
			m_links.emplace_back(Link(getNextLinkId(), currentNode.outputs[0].id, nextNodePtr->inputs[0].id));
			m_links.back().color = getPinTextColor(currentNode.outputs[0].type);
		} else if (type == "SetServoPosState") {
			std::shared_ptr<SetServoPosState> currentStatePtr = std::static_pointer_cast<SetServoPosState>(currentNode.m_statePtr);
			if (currentStatePtr->m_nextStatePtr == nullptr)
				continue;
			
			Node* nextNodePtr = nullptr;
			for (auto& node : m_nodes) {
				if (node.m_statePtr == currentStatePtr->m_nextStatePtr)
					nextNodePtr = &node;
			}
			
			m_links.emplace_back(Link(getNextLinkId(), currentNode.outputs[0].id, nextNodePtr->inputs[0].id));
			m_links.back().color = getPinTextColor(currentNode.outputs[0].type);
		} else if (type == "WaitState") {
			std::shared_ptr<WaitState> currentStatePtr = std::static_pointer_cast<WaitState>(currentNode.m_statePtr);
			if (currentStatePtr->m_nextStatePtr == nullptr)
				continue;
			
			Node* nextNodePtr = nullptr;
			for (auto& node : m_nodes) {
				if (node.m_statePtr == currentStatePtr->m_nextStatePtr)
					nextNodePtr = &node;
			}
			
			m_links.emplace_back(Link(getNextLinkId(), currentNode.outputs[0].id, nextNodePtr->inputs[0].id));
			m_links.back().color = getPinTextColor(currentNode.outputs[0].type);
		}
	}
}
