#include "strategy/strategies.hpp"

#include <iostream>
#include <chrono>

#include "hal/bcm2835.h"
#include "parameters.hpp"

using namespace std::chrono_literals;

Strategy::Strategy(World& world): m_world(world) {

}

void initStrategies(std::map<std::string, Strategy*>& strategies, std::vector<std::string>& strategiesNames, World& world) {
    strategies.insert(std::make_pair("Blue homologation", new BlueHomologation(world)));
    strategies.insert(std::make_pair("Yellow homologation", new YellowHomologation(world)));
    strategies.insert(std::make_pair("Arm test", new ArmTest(world)));
    strategies.insert(std::make_pair("Demo", new Demo(world)));

    strategiesNames.clear();
    for(auto const& strategy: strategies) {
        strategiesNames.push_back(strategy.first);
    }
}

BlueHomologation::BlueHomologation(World& world): Strategy(world) {}

void BlueHomologation::routine() {
    bool value;
    switch (m_world.m_strategyStep) {
        case 0:
            if (!bcm2835_init())
                return;

            // Set RPI pin P1-15 to be an input
            bcm2835_gpio_fsel(PIN_TIRRETE, BCM2835_GPIO_FSEL_INPT);
            //  with a pullup
            bcm2835_gpio_set_pud(PIN_TIRRETE, BCM2835_GPIO_PUD_UP); 
            m_world.m_ownRobot.m_ownRobotRollingBaseMode = POSE_CONTROL;

            std::cout << "Waiting for tirette push ..." << std::endl;   
            m_world.m_strategyStep++;
            break;
        case 1:
            value = bcm2835_gpio_lev(PIN_TIRRETE);
            //std::cout << "Value: " << value << std::endl;
            if (!value) {
                std::cout << "Waiting for tirette pull ..." << std::endl;
                m_world.m_strategyStep++;
            }
            break;
        case 2:
            value = bcm2835_gpio_lev(PIN_TIRRETE);
            //std::cout << "Value: " << value << std::endl;
            if (value) {
                std::cout << "Match started" << std::endl;
                m_world.m_strategyStep++;
            }
            break;  
        case 3:
            m_world.m_ownRobot.m_targetPose = Pose2f(Vec2f(2800.f, 200.f), 0.f);
            m_world.m_ownRobot.m_motorsEnabled = true;
            m_world.m_strategyStep++;
            break;
        case 4:
            if (getDistance(m_world.m_ownRobot.m_currentPose.m_position, m_world.m_ownRobot.m_targetPose.m_position) < 20) {
                m_world.m_ownRobot.m_motorsEnabled = false;
                std::cout << "Match finished" << std::endl;
                m_world.m_strategyStep++;
            }
            break;    
    }
}

YellowHomologation::YellowHomologation(World& world): Strategy(world) {}

void YellowHomologation::routine() {
    bool value;
    switch (m_world.m_strategyStep) {
        case 0:
            if (!bcm2835_init())
                return;

            // Set RPI pin P1-15 to be an input
            bcm2835_gpio_fsel(PIN_TIRRETE, BCM2835_GPIO_FSEL_INPT);
            //  with a pullup
            bcm2835_gpio_set_pud(PIN_TIRRETE, BCM2835_GPIO_PUD_UP);
            m_world.m_ownRobot.m_ownRobotRollingBaseMode = POSE_CONTROL;
            
            std::cout << "Waiting for tirette push ..." << std::endl;
            m_world.m_strategyStep++;
            break;
        case 1:
            value = bcm2835_gpio_lev(PIN_TIRRETE);
            //std::cout << "Value: " << value << std::endl;
            if (!value) {
                std::cout << "Waiting for tirette pull ..." << std::endl;
                m_world.m_strategyStep++;
            }
            break;
        case 2:
            value = bcm2835_gpio_lev(PIN_TIRRETE);
            //std::cout << "Value: " << value << std::endl;
            if (value) {
                std::cout << "Match started" << std::endl;
                m_world.m_strategyStep++;
            }
            break;  
        case 3:
            m_world.m_ownRobot.m_targetPose = Pose2f(Vec2f(200.f, 200.f), 0.f);
            m_world.m_ownRobot.m_motorsEnabled = true;
            m_world.m_strategyStep++;
            break;
        case 4:
            if (getDistance(m_world.m_ownRobot.m_currentPose.m_position, m_world.m_ownRobot.m_targetPose.m_position) < 20) {
                m_world.m_ownRobot.m_motorsEnabled = false;
                std::cout << "Match finished" << std::endl;
                m_world.m_strategyStep++;
            }
            break;
    }
}

ArmTest::ArmTest(World& world): Strategy(world) {}

void ArmTest::routine() {
    bool value;
    static std::chrono::time_point<std::chrono::high_resolution_clock> waitingStartTime;
    switch (m_world.m_strategyStep) {
        case 0:
            if (!bcm2835_init())
                return;

            // Set RPI pin P1-15 to be an input
            bcm2835_gpio_fsel(PIN_TIRRETE, BCM2835_GPIO_FSEL_INPT);
            //  with a pullup
            bcm2835_gpio_set_pud(PIN_TIRRETE, BCM2835_GPIO_PUD_UP);
            m_world.m_ownRobot.m_ownRobotRollingBaseMode = POSE_CONTROL;
            
            std::cout << "Waiting for tirette push ..." << std::endl;
            m_world.m_strategyStep++;
            break;
        case 1:
            value = bcm2835_gpio_lev(PIN_TIRRETE);
            //std::cout << "Value: " << value << std::endl;
            if (!value) {
                std::cout << "Waiting for tirette pull ..." << std::endl;
                m_world.m_strategyStep++;
            }
            break;
        case 2:
            value = bcm2835_gpio_lev(PIN_TIRRETE);
            //std::cout << "Value: " << value << std::endl;
            if (value) {
                std::cout << "Match started" << std::endl;
                m_world.m_strategyStep++;
            }
            break;  
        case 3:
            if (!m_world.m_ownRobot.m_arm1.m_isCommandToSend) {
                m_world.m_ownRobot.m_arm1.m_command.immediate = true;
                m_world.m_ownRobot.m_arm1.m_command.commandType = 1;
                m_world.m_ownRobot.m_arm1.m_command.argument = 7;
                m_world.m_ownRobot.m_arm1.m_isCommandToSend = true;
                waitingStartTime = std::chrono::high_resolution_clock::now();
                m_world.m_strategyStep++;
            }
            break;
        case 4:
            if ((std::chrono::high_resolution_clock::now() - waitingStartTime) >= 1000ms) {
                m_world.m_strategyStep++;
            }
            break;
        case 5:
            if (!m_world.m_ownRobot.m_arm1.m_isCommandToSend) {
                m_world.m_ownRobot.m_arm1.m_command.immediate = true;
                m_world.m_ownRobot.m_arm1.m_command.commandType = 2;
                m_world.m_ownRobot.m_arm1.m_command.argument = 6;
                m_world.m_ownRobot.m_arm1.m_isCommandToSend = true;
                m_world.m_strategyStep++;
            }
            break;
        case 6:
            if (!m_world.m_ownRobot.m_arm1.m_isCommandToSend) {
                m_world.m_ownRobot.m_arm1.m_command.immediate = false;
                m_world.m_ownRobot.m_arm1.m_command.commandType = 3;
                m_world.m_ownRobot.m_arm1.m_command.argument = 11;
                m_world.m_ownRobot.m_arm1.m_isCommandToSend = true;
                waitingStartTime = std::chrono::high_resolution_clock::now();
                m_world.m_strategyStep++;
            }
            break;
        case 7:
            if ((std::chrono::high_resolution_clock::now() - waitingStartTime) >= 1000ms) {
                m_world.m_strategyStep++;
            }
            break;
        case 8:
            if (!m_world.m_ownRobot.m_arm1.m_isCommandToSend) {
                m_world.m_ownRobot.m_arm1.m_command.immediate = true;
                m_world.m_ownRobot.m_arm1.m_command.commandType = 1;
                m_world.m_ownRobot.m_arm1.m_command.argument = 13;
                m_world.m_ownRobot.m_arm1.m_isCommandToSend = true;
                waitingStartTime = std::chrono::high_resolution_clock::now();
                m_world.m_strategyStep++;
            }
            break;
        case 9:
            if ((std::chrono::high_resolution_clock::now() - waitingStartTime) >= 3000ms) {
                m_world.m_strategyStep++;
            }
            break;
        case 10:
            if (!m_world.m_ownRobot.m_arm1.m_isCommandToSend) {
                m_world.m_ownRobot.m_arm1.m_command.immediate = true;
                m_world.m_ownRobot.m_arm1.m_command.commandType = 2;
                m_world.m_ownRobot.m_arm1.m_command.argument = 12;
                m_world.m_ownRobot.m_arm1.m_isCommandToSend = true;
                m_world.m_strategyStep++;
            }
            break;
        case 11:
            if (!m_world.m_ownRobot.m_arm1.m_isCommandToSend) {
                m_world.m_ownRobot.m_arm1.m_command.immediate = false;
                m_world.m_ownRobot.m_arm1.m_command.commandType = 3;
                m_world.m_ownRobot.m_arm1.m_command.argument = 5;
                m_world.m_ownRobot.m_arm1.m_isCommandToSend = true;
                m_world.m_strategyStep++;
            }
            break;
        case 12:
            std::cout << "Match finished" << std::endl;
            m_world.m_strategyStep++;
            break;
    }
}

Demo::Demo(World& world): Strategy(world) {}

void Demo::routine() {
    bool value;
    static std::chrono::time_point<std::chrono::high_resolution_clock> waitingStartTime;
    switch (m_world.m_strategyStep) {
        case 0:
            m_world.m_ownRobot.m_motorsEnabled = false;
            if (!bcm2835_init())
                return;

            // Set RPI pin P1-15 to be an input
            bcm2835_gpio_fsel(PIN_TIRRETE, BCM2835_GPIO_FSEL_INPT);
            //  with a pullup
            bcm2835_gpio_set_pud(PIN_TIRRETE, BCM2835_GPIO_PUD_UP); 
            m_world.m_ownRobot.m_ownRobotRollingBaseMode = POSE_CONTROL;

            std::cout << "Waiting for tirette push ..." << std::endl;   
            m_world.m_strategyStep++;
            break;
        case 1:
            if (!m_world.m_ownRobot.m_arm1.m_isCommandToSend) {
                m_world.m_ownRobot.m_arm1.m_command.immediate = true;
                m_world.m_ownRobot.m_arm1.m_command.commandType = 1;
                m_world.m_ownRobot.m_arm1.m_command.argument = 13;
                m_world.m_ownRobot.m_arm1.m_isCommandToSend = true;
                m_world.m_strategyStep++;
            }
            break;
        case 2:
            if (!m_world.m_ownRobot.m_arm1.m_isCommandToSend) {
                m_world.m_ownRobot.m_arm1.m_command.immediate = true;
                m_world.m_ownRobot.m_arm1.m_command.commandType = 2;
                m_world.m_ownRobot.m_arm1.m_command.argument = 12;
                m_world.m_ownRobot.m_arm1.m_isCommandToSend = true;
                m_world.m_strategyStep++;
            }
            break;
        case 3:
            if (!m_world.m_ownRobot.m_arm1.m_isCommandToSend) {
                m_world.m_ownRobot.m_arm1.m_command.immediate = false;
                m_world.m_ownRobot.m_arm1.m_command.commandType = 3;
                m_world.m_ownRobot.m_arm1.m_command.argument = 5;
                m_world.m_ownRobot.m_arm1.m_isCommandToSend = true;
                m_world.m_strategyStep++;
            }
            break;
        case 4:
            value = bcm2835_gpio_lev(PIN_TIRRETE);
            //std::cout << "Value: " << value << std::endl;
            if (!value) {
                std::cout << "Waiting for tirette pull ..." << std::endl;
                m_world.m_strategyStep++;
            }
            break;
        case 5:
            value = bcm2835_gpio_lev(PIN_TIRRETE);
            //std::cout << "Value: " << value << std::endl;
            if (value) {
                std::cout << "Match started" << std::endl;
                m_world.m_strategyStep++;
            }
            break;  
        case 6:
            m_world.m_ownRobot.m_targetPose = Pose2f(Vec2f(1200.f, 500.f), 2.09f);
            m_world.m_ownRobot.m_motorsEnabled = true;
            m_world.m_strategyStep++;
            break;
        case 7:
            if (getDistance(m_world.m_ownRobot.m_currentPose.m_position, m_world.m_ownRobot.m_targetPose.m_position) < 50 && abs(m_world.m_ownRobot.m_currentPose.m_orientation -  m_world.m_ownRobot.m_targetPose.m_orientation) < 0.05f) {
                m_world.m_ownRobot.m_targetPose = Pose2f(Vec2f(1000.f, 500.f), 2.09f);
                m_world.m_ownRobot.m_motorsEnabled = true;
                m_world.m_strategyStep++;
            }
            break;
        case 8:
            if (getDistance(m_world.m_ownRobot.m_currentPose.m_position, m_world.m_ownRobot.m_targetPose.m_position) < 20 && abs(m_world.m_ownRobot.m_currentPose.m_orientation -  m_world.m_ownRobot.m_targetPose.m_orientation) < 0.02f) {
                m_world.m_ownRobot.m_motorsEnabled = false;
                if (!m_world.m_ownRobot.m_arm1.m_isCommandToSend) {
                    m_world.m_ownRobot.m_arm1.m_command.immediate = true;
                    m_world.m_ownRobot.m_arm1.m_command.commandType = 1;
                    m_world.m_ownRobot.m_arm1.m_command.argument = 7;
                    m_world.m_ownRobot.m_arm1.m_isCommandToSend = true;
                    waitingStartTime = std::chrono::high_resolution_clock::now();
                    m_world.m_strategyStep++;
                }
            }
            break;
        case 9:
            if ((std::chrono::high_resolution_clock::now() - waitingStartTime) >= 300ms) {
                m_world.m_strategyStep++;
            }
            break;
        case 10:
            if (!m_world.m_ownRobot.m_arm1.m_isCommandToSend) {
                m_world.m_ownRobot.m_arm1.m_command.immediate = true;
                m_world.m_ownRobot.m_arm1.m_command.commandType = 2;
                m_world.m_ownRobot.m_arm1.m_command.argument = 6;
                m_world.m_ownRobot.m_arm1.m_isCommandToSend = true;
                m_world.m_strategyStep++;
            }
            break;
        case 11:
            if (!m_world.m_ownRobot.m_arm1.m_isCommandToSend) {
                m_world.m_ownRobot.m_arm1.m_command.immediate = false;
                m_world.m_ownRobot.m_arm1.m_command.commandType = 3;
                m_world.m_ownRobot.m_arm1.m_command.argument = 11;
                m_world.m_ownRobot.m_arm1.m_isCommandToSend = true;
                waitingStartTime = std::chrono::high_resolution_clock::now();
                m_world.m_strategyStep++;
            }
            break;
        case 12:
            if ((std::chrono::high_resolution_clock::now() - waitingStartTime) >= 300ms) {
                m_world.m_strategyStep++;
            }
            break;
        case 13:
            if (!m_world.m_ownRobot.m_arm1.m_isCommandToSend) {
                m_world.m_ownRobot.m_arm1.m_command.immediate = true;
                m_world.m_ownRobot.m_arm1.m_command.commandType = 1;
                m_world.m_ownRobot.m_arm1.m_command.argument = 13;
                m_world.m_ownRobot.m_arm1.m_isCommandToSend = true;
                m_world.m_strategyStep++;
            }
            break;
        case 14:
            m_world.m_ownRobot.m_targetPose = Pose2f(Vec2f(250.f, 500.f), 2.09f);
            m_world.m_ownRobot.m_motorsEnabled = true;
            m_world.m_strategyStep++;
            break;
         case 15:
            if (getDistance(m_world.m_ownRobot.m_currentPose.m_position, m_world.m_ownRobot.m_targetPose.m_position) < 40 && abs(m_world.m_ownRobot.m_currentPose.m_orientation -  m_world.m_ownRobot.m_targetPose.m_orientation) < 0.04f) {
                m_world.m_ownRobot.m_motorsEnabled = false;
                if (!m_world.m_ownRobot.m_arm1.m_isCommandToSend) {
                    m_world.m_ownRobot.m_arm1.m_command.immediate = true;
                    m_world.m_ownRobot.m_arm1.m_command.commandType = 1;
                    m_world.m_ownRobot.m_arm1.m_command.argument = 7;
                    m_world.m_ownRobot.m_arm1.m_isCommandToSend = true;
                    waitingStartTime = std::chrono::high_resolution_clock::now();
                    m_world.m_strategyStep++;
                }
            }
            break;
        case 16:
            if ((std::chrono::high_resolution_clock::now() - waitingStartTime) >= 300ms) {
                m_world.m_strategyStep++;
            }
            break;
        case 17:
            if (!m_world.m_ownRobot.m_arm1.m_isCommandToSend) {
                m_world.m_ownRobot.m_arm1.m_command.immediate = true;
                m_world.m_ownRobot.m_arm1.m_command.commandType = 2;
                m_world.m_ownRobot.m_arm1.m_command.argument = 12;
                m_world.m_ownRobot.m_arm1.m_isCommandToSend = true;
                m_world.m_strategyStep++;
            }
            break;
        case 18:
            if (!m_world.m_ownRobot.m_arm1.m_isCommandToSend) {
                m_world.m_ownRobot.m_arm1.m_command.immediate = false;
                m_world.m_ownRobot.m_arm1.m_command.commandType = 3;
                m_world.m_ownRobot.m_arm1.m_command.argument = 5;
                m_world.m_ownRobot.m_arm1.m_isCommandToSend = true;
                waitingStartTime = std::chrono::high_resolution_clock::now();
                m_world.m_strategyStep++;
            }
            break;
        case 19:
            if ((std::chrono::high_resolution_clock::now() - waitingStartTime) >= 300ms) {
                m_world.m_strategyStep++;
            }
            break;
        case 20:
            if (!m_world.m_ownRobot.m_arm1.m_isCommandToSend) {
                m_world.m_ownRobot.m_arm1.m_command.immediate = true;
                m_world.m_ownRobot.m_arm1.m_command.commandType = 1;
                m_world.m_ownRobot.m_arm1.m_command.argument = 13;
                m_world.m_ownRobot.m_arm1.m_isCommandToSend = true;
                waitingStartTime = std::chrono::high_resolution_clock::now();
                m_world.m_strategyStep++;
            }
            break;
        case 21:
            if ((std::chrono::high_resolution_clock::now() - waitingStartTime) >= 300ms) {
                m_world.m_strategyStep++;
            }
            break;
        case 22:
            m_world.m_ownRobot.m_targetPose = Pose2f(Vec2f(1850.f, 500.f), 0.f);
            m_world.m_ownRobot.m_motorsEnabled = true;
            m_world.m_strategyStep++;
            break;
        case 23:
            if (getDistance(m_world.m_ownRobot.m_currentPose.m_position, m_world.m_ownRobot.m_targetPose.m_position) < 20 && abs(m_world.m_ownRobot.m_currentPose.m_orientation -  m_world.m_ownRobot.m_targetPose.m_orientation) < 0.02f) {
                m_world.m_ownRobot.m_motorsEnabled = false;
                std::cout << "Match finished" << std::endl;
                m_world.m_strategyStep++;
            }
            break;   
    }
}
