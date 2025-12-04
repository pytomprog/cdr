#include "communication/MqttClientManager.hpp"

#include <spdlog/spdlog.h>

MqttClientManager::MqttClientManager(const std::string& serverURI, const std::string& clientId, const std::string& persistDir, const std::string& ownRobotPositionTopic, int ownRobotPositionQOS, const std::string& advRobotPositionTopic, int advRobotPositionQOS)
	: m_serverURI(serverURI),
	  m_clientId(clientId),
	  m_persistDir(persistDir),
	  m_ownRobotPositionTopic(ownRobotPositionTopic),
	  m_ownRobotPositionQOS(ownRobotPositionQOS),
	  m_client(m_serverURI, m_clientId, m_persistDir),
	  m_isConnected(false),
	  m_advRobotPositionTopic(advRobotPositionTopic),
	  m_advRobotPositionQOS(advRobotPositionQOS)
{
	//GOOGLE_PROTOBUF_VERIFY_VERSION;
}

MqttClientManager::~MqttClientManager() {
	disconnectFromBroker();
}

void MqttClientManager::connectToBroker() {
	spdlog::info("Trying to connect to MQTT broker...");
	try {
		mqtt::connect_options connOpts;
		connOpts.set_clean_session(true);
		m_client.connect(connOpts)->wait();
		m_isConnected = true;
		spdlog::info("Connected to MQTT broker");
	}
	catch (const mqtt::exception& exc) {
		spdlog::error("Error connecting to MQTT broker: {}", exc.what());
		m_isConnected = false;
	}
}

void MqttClientManager::disconnectFromBroker() {
	try {
		m_client.disconnect()->wait();
		m_isConnected = false;
		spdlog::info("Disconnected from MQTT broker");
	}
	catch (const mqtt::exception& exc) {
		spdlog::error("Error disconnecting from MQTT broker: {}", exc.what());
	}
}

bool MqttClientManager::publish(const std::string& topic, const std::string& payload, int qos, bool retained) {
	if (!m_isConnected) {
		spdlog::error("Cannot publish message, MQTT client is not connected");
		return false;
	}
	try {
		auto msg = mqtt::make_message(topic, payload);
		msg->set_qos(qos);
		msg->set_retained(retained);
		m_client.publish(msg)->wait_for(std::chrono::seconds(10));
		return true;
	}
	catch (const mqtt::exception& exc) {
		spdlog::error("Error publishing message to MQTT broker: {}", exc.what());
		return false;
	}
}

bool MqttClientManager::publishOwnRobotPosition(float tx, float ty, float rz) {
	robot_Position ownRobotPosition = robot_Position_init_zero;
	ownRobotPosition.x = tx;
	ownRobotPosition.y = ty;
	ownRobotPosition.theta = rz;
	ownRobotPosition.ts_ms = static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count());

	uint8_t buffer[128];
	pb_ostream_t payload = pb_ostream_from_buffer(buffer, sizeof(buffer));
	pb_encode(&payload, robot_Position_fields, &ownRobotPosition);
	std::string payloadStr(reinterpret_cast<char*>(buffer), payload.bytes_written);
	return publish(m_ownRobotPositionTopic, payloadStr, m_ownRobotPositionQOS, false);
}

bool MqttClientManager::publishAdvRobotPosition(float tx, float ty, float rz) {
	robot_Position advRobotPosition = robot_Position_init_zero;
	advRobotPosition.x = tx;
	advRobotPosition.y = ty;
	advRobotPosition.theta = rz;
	advRobotPosition.ts_ms = static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count());
	
	uint8_t buffer[128];
	pb_ostream_t payload = pb_ostream_from_buffer(buffer, sizeof(buffer));
	pb_encode(&payload, robot_Position_fields, &advRobotPosition);
	std::string payloadStr(reinterpret_cast<char*>(buffer), payload.bytes_written);
	return publish(m_advRobotPositionTopic, payloadStr, m_advRobotPositionQOS, false);
}