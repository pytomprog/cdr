#include "hal/HardwareAbstractionLayer.hpp"

#include <imgui.h>
#include <imgui-SFML.h>

#include "parameters.hpp"

HardwareAbstractionLayer::HardwareAbstractionLayer(World& world): m_world(world) {

}