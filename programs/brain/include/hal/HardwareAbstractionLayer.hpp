#pragma once

#include "world/World.hpp"

#include <cstring> 
#include <iostream> 
#include <netinet/in.h> 
#include <sys/socket.h> 
#include <unistd.h> 

class HardwareAbstractionLayer {
public:
	World& m_world;

	int m_cameraServerSocket; 
    sockaddr_in m_cameraServerAddress;
	int m_cameraClientSocket;
	bool m_cameraClientConnected;

	HardwareAbstractionLayer(World& world);
	
	void updateConfig();

	void ownRobotCameraRoutine();
	void ownRobotRollingBaseRoutine();
	void ownRobotArmRoutine();
};