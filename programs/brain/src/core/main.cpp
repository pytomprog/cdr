#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System/Clock.hpp>
#include <imgui.h>
#include <imgui-SFML.h>
#include <thread>
#include <mutex>
#include <chrono>
#include <vector>

#include "parameters.hpp"
#include "gui/gui.hpp"
#include "hal/HardwareAbstractionLayer.hpp"

//std::mutex halMutex;

std::vector<std::thread> testThreads;

void halLoop(HardwareAbstractionLayer& hal) {
    using namespace std::chrono_literals;
    while (true) {
        //halMutex.lock();
        static int direction = 1;
        Pose2f oldPose = hal.m_world.m_ownRobot.m_currentPose;
        if (oldPose.m_position.m_x < X_MIN_BORDER || oldPose.m_position.m_x > X_MAX_BORDER) {
            direction *= -1;
        }
        Pose2f newPose = Pose2f(oldPose.m_position + Vec2f(direction*0.01, 0), oldPose.m_orientation);
        hal.m_world.m_ownRobot.m_currentPose = newPose;
        //halMutex.unlock();
        //std::this_thread::sleep_for(100ms);
    }
}

int main() {
    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Core");
    ImGui::SFML::Init(window);

    sf::Clock deltaClock;

    OwnRobot ownRobot(Pose2f(Vec2f(0.f, 0.f, sf::Color::Green), 0.f));
    World world(ownRobot);

    HardwareAbstractionLayer hal(world);
    GraphicalUserInterface gui(window, world);

    std::thread halThread(halLoop, std::ref(hal));
    for (int i = 0; i < 100; i++) {
        testThreads.push_back(std::thread(halLoop, std::ref(hal)));
    }

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

        //halMutex.lock();
        gui.update();
        //halMutex.unlock();

        ImGui::SFML::Render(window);
        window.display();
    }

    halThread.join();
    for (int i = 0; i < 100; i++) {
        testThreads[i].join();
    }

    ImGui::SFML::Shutdown();

    return 0;
}
