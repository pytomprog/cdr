#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System/Clock.hpp>
#include <imgui-node-editor/imgui_node_editor.h>

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
    ::Node* node;
    std::string name;
    PinType type;
    PinKind kind;

    Pin(int _id, const char* _name, PinType _type):
        id(_id), node(nullptr), name(_name), type(_type), kind(PinKind::Input) {}
};

struct Node {
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

	DebugGui();
	
	int getNextId();
	NodeEditor::LinkId getNextLinkId();
	Node* findNode(NodeEditor::NodeId id);
	Link* findLink(NodeEditor::LinkId id);
	Pin* findPin(NodeEditor::PinId id);
	void buildNode(Node* node);
	void buildNodes();
	
	Node* spawnFirstNode();
	Node* spawnSecondNode();

    void update(sf::RenderWindow& window);
};
