#include "ServiceLocator.h"

DeviceManager *ServiceLocator::s_DeviceManager = nullptr;
AssetManager *ServiceLocator::s_AssetManager = nullptr;
InputManager *ServiceLocator::s_InputManager = nullptr;