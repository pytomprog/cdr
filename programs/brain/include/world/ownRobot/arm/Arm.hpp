#pragma once

struct ArmCommand {
    bool immediate;
    int commandType;
    int argument;
};

class Arm {
public:
	bool m_isCommandToSend;
    ArmCommand m_command;

	Arm();
};