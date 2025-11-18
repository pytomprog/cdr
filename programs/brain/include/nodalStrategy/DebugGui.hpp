#pragma once

#ifndef DEBUG_GUI
#define DEBUG_GUI

#include <iostream>
#include <string>
#include <memory>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System/Clock.hpp>
#include <imgui-node-editor/imgui_node_editor.h>

#include "nodalStrategy/StrategyStateMachine.hpp"
#include "nodalStrategy/StrategyStates.hpp"

namespace NodeEditor = ax::NodeEditor;

enum class PinType {
    Flow,
    Bool,
    Int,
    Float,
    String,
    Object,
    Function,
};

enum class PinKind {
    Output,
    Input
};

enum class NodeType {
    Blueprint,
    Simple
};

struct Node;

struct Pin {
    NodeEditor::PinId id;
    NodeEditor::NodeId nodeId;
    std::string name;
    PinType type;
    PinKind kind;

    Pin(int _id, const char* _name, PinType _type):
        id(_id), nodeId(0), name(_name), type(_type), kind(PinKind::Input) {}
};

struct Node {
	std::shared_ptr<StrategyState> m_statePtr = nullptr;
	
    NodeEditor::NodeId id;
    std::string name;
    std::vector<Pin> inputs;
    std::vector<Pin> outputs;
    ImColor color;
    NodeType type;
    ImVec2 size;

    std::string state;
    std::string savedState;

    Node(int _id, const char* _name, ImColor _color = ImColor(255, 255, 255)):
        id(_id), name(_name), color(_color), type(NodeType::Blueprint), size(0, 0) {}
};

struct Link {
    NodeEditor::LinkId id;

    NodeEditor::PinId startPinId;
    NodeEditor::PinId endPinId;

    ImColor color;

    Link(NodeEditor::LinkId _id, NodeEditor::PinId _startPinId, NodeEditor::PinId _endPinId):
        id(id), startPinId(_startPinId), endPinId(_endPinId), color(255, 255, 255) {}
};

struct LinkInfo {
	NodeEditor::LinkId id;
	NodeEditor::PinId inputId;
	NodeEditor::PinId outputId;
};

ImColor getPinTextColor(PinType type);

class DebugGui {
public:
	sf::Clock m_deltaClock;
	
	NodeEditor::EditorContext* m_contextPtr = nullptr;
	int m_nextId = 1;
	std::vector<Node> m_nodes;
	std::vector<Link> m_links;
	int m_nextLinkId = 1000;
	
	StrategyStateMachine& m_stateMachine;
	std::string m_strategyFilename = "testStrategy.yaml";

	DebugGui(StrategyStateMachine& stateMachine);
	
	int getNextId();
	NodeEditor::LinkId getNextLinkId();
	Node* findNode(NodeEditor::NodeId id);
	Link* findLink(NodeEditor::LinkId id);
	Pin* findPin(NodeEditor::PinId id);
	void buildNode(Node* node);
	void buildNodes();
	
	Node* spawnDemoNode();
	Node* spawnIdleStateNode(std::shared_ptr<IdleState> statePtr = nullptr);
	Node* spawnTestStateNode(std::shared_ptr<TestState> statePtr = nullptr);
	Node* spawnStartStateNode(std::shared_ptr<StartState> statePtr = nullptr);
	Node* spawnTiretteStateNode(std::shared_ptr<TiretteState> statePtr = nullptr);
	Node* spawnMovetoStateNode(std::shared_ptr<MovetoState> statePtr = nullptr);
	Node* spawnMoveThroughStateNode(std::shared_ptr<MoveThroughState> statePtr = nullptr);
	Node* spawnChangeVacuumStateStateNode(std::shared_ptr<ChangeVacuumStateState> statePtr = nullptr);
	Node* spawnSetStepperPosStateNode(std::shared_ptr<SetStepperPosState> statePtr = nullptr);
	Node* spawnSetServoPosStateNode(std::shared_ptr<SetServoPosState> statePtr = nullptr);
	Node* spawnWaitStateNode(std::shared_ptr<WaitState> statePtr = nullptr);

    void update(sf::RenderWindow& window);
    
    void loadNodes();
};

#endif //DEBUG_GUI
