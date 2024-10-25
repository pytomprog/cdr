#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System/Clock.hpp>
#include <imgui.h>
#include <imgui-SFML.h>
#include <thread>
#include <mutex>
#include <chrono>
#include <map>
#include <string>
#include <vector>

#include "parameters.hpp"
#include "gui/gui.hpp"
#include "hal/HardwareAbstractionLayer.hpp"
#include "hal/bcm2835.h"
#include "strategy/strategies.hpp"

//std::mutex halMutex;

void halLoop(HardwareAbstractionLayer& hal) {
    using namespace std::chrono_literals;
    using clock = std::chrono::high_resolution_clock;
    using time_point = std::chrono::time_point<clock>;
    
    if (!bcm2835_init()) {
        std::cout << "bcm2835_init failed. Are you running as root??" << std::endl;
        return;
    }
    if (!bcm2835_i2c_begin()) {
        std::cout << "bcm2835_i2c_begin failed. Are you running as root??" << std::endl;
        return;
    }

    bcm2835_i2c_setClockDivider(I2C_CLOCK_DIVIDER_VALUE);
    
    time_point previousTime = clock::now();
    while (true) {
        time_point currentTime = clock::now();
        std::chrono::duration<double> timeDifference = currentTime - previousTime;
        hal.m_deltaTime = timeDifference.count();
        //std::cout << "hal deltaTime: " << hal.m_deltaTime << std::endl;
        
        //halMutex.lock();
        hal.ownRobotCameraRoutine();
        hal.ownRobotRollingBaseRoutine();
        hal.ownRobotArmRoutine();
        //halMutex.unlock();
        //std::this_thread::sleep_for(20ms);
        
        previousTime = currentTime;
    }

    bcm2835_i2c_end();

    bcm2835_close();
}

int main() {
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Core");
    ImGui::SFML::Init(window);

    sf::Clock deltaClock;

    OwnRobot ownRobot(Pose2f(Vec2f(0.f, 0.f, sf::Color::Green), 0.f), 200.f, false); // TODO: Put it back to true, this is for test purpose only
    Obstacle advRobot(Pose2f(Vec2f(0.f, 0.f, sf::Color::Yellow), 0.f), 500.f);
    World world(STRATEGY_MODE, "Demo", ownRobot, advRobot);

    std::map<std::string, Strategy*> strategies;
    std::vector<std::string> strategiesNames;
    initStrategies(strategies, strategiesNames, world);

    HardwareAbstractionLayer hal(world);
    GraphicalUserInterface gui(window, world, strategiesNames);

    std::thread halThread(halLoop, std::ref(hal));

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

        if (world.m_runningMode == STRATEGY_MODE) {
            strategies[world.m_strategyName]->routine();
        }

        //ImGui::ShowDemoWindow();

        //halMutex.lock();
        gui.update();
        //halMutex.unlock();

        ImGui::SFML::Render(window);
        window.display();
    }

    halThread.join();

    ImGui::SFML::Shutdown();

    return 0;
}
