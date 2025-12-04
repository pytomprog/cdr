#pragma once

#ifndef CDR_MQTTCLIENTMANAGER_HPP
#define CDR_MQTTCLIENTMANAGER_HPP

#include <atomic>
#include <chrono>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>
#include <thread>

#include <mqtt/async_client.h>
#include "RobotPosition.pb.h"
#include <pb.h>
#include <pb_encode.h>
#include <pb_decode.h>

class MqttClientManager {
private:
	const std::string m_serverURI;
	const std::string m_clientId;
	const std::string m_persistDir;
	const std::string m_ownRobotPositionTopic;
	const int m_ownRobotPositionQOS;
	const std::string m_advRobotPositionTopic;
	const int m_advRobotPositionQOS;

	mqtt::async_client m_client = mqtt::async_client(m_serverURI, m_clientId, m_persistDir);
	bool m_isConnected = false;

public:
	MqttClientManager(const std::string& serverURI, const std::string& clientId, const std::string& persistDir, const std::string& ownRobotPositionTopic, int ownRobotPositionQOS, const std::string& advRobotPositionTopic, int advRobotPositionQOS);
	~MqttClientManager();

	void connectToBroker();
	void disconnectFromBroker();
	inline bool isConnected() const { return m_isConnected; }

	bool publishOwnRobotPosition(float tx, float ty, float rz);
	bool publishAdvRobotPosition(float tx, float ty, float rz);
private:
	bool publish(const std::string& topic, const std::string& payload, int qos = 0, bool retained = false);
};

#endif //CDR_MQTTCLIENTMANAGER_HPP