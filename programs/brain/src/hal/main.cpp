#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System/Clock.hpp>
#include <imgui.h>
#include <imgui-SFML.h>

#include "parameters.hpp"
#include "hal/HardwareAbstractionLayer.hpp"

int main() {
    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "HAL");
    ImGui::SFML::Init(window);

    sf::Clock deltaClock;

    Robot ownRobot(Pose2f(Vec2f(0.f, 0.f, sf::Color::Green), 0.f));
    HardwareAbstractionLayer hal(ownRobot);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            ImGui::SFML::ProcessEvent(window, event);

            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }

        ImGui::SFML::Update(window, deltaClock.restart());
        window.clear();

        //ImGui::ShowDemoWindow();
        
        hal.updateConfig();

        // Sensors routines
        hal.cameraRoutine();

        // Actuators routines
        hal.rollingBaseRoutine();
        hal.armRoutine();

        hal.m_ownRobot.m_currentPose.update();
        hal.m_ownRobot.m_currentPose.draw(window);
        if (hal.m_ownRobot.m_periphericalsConfig.rollingBaseConnected) {
            hal.m_ownRobot.m_targetPose.update();
            hal.m_ownRobot.m_targetPose.draw(window);
        }

        ImGui::SFML::Render(window);
        window.display();
    }

    ImGui::SFML::Shutdown();

    return 0;
}
