#include "Engine.h"
#include "core/ServiceLocator.h"
#include "managers/DeviceManager.h"
#include "managers/AssetManager.h"
#include "input/InputManager.h"
#include "systems/RenderSystem.h"
#include "systems/StatsSystem.h"
#include "systems/CameraController.h"
#include "utils/Logger.h"

Engine::~Engine() = default; // Define destructor here

void Engine::Init(HWND hwnd, UINT width, UINT height)
{
    m_Width = width;
    m_Height = height;

    InitServices(hwnd, width, height);

    m_SceneManager = std::make_unique<SceneManager>();
    m_SceneManager->Init(width, height);

    InitSystems();

    m_SceneManager->InitScene();
}

void Engine::Update(float deltaTime)
{
    m_SystemManager->UpdateAll(deltaTime);
}

void Engine::Shutdown()
{
    m_SystemManager->ShutdownAll();
    m_AssetManager->Shutdown();
    m_DeviceManager->Shutdown();
}

void Engine::OnResize(int width, int height)
{
    ServiceLocator::GetDeviceManager().ResizeSwapChain(width, height);
    m_SceneManager->OnResize(width, height);
    m_SystemManager->OnResize(width, height);
}

void Engine::InitServices(HWND hwnd, UINT width, UINT height)
{
    // create and provide services
    m_DeviceManager = std::make_unique<DeviceManager>();
    m_DeviceManager->InitDevice(hwnd, width, height);
    ServiceLocator::Provide(m_DeviceManager.get());

    m_AssetManager = std::make_unique<AssetManager>();
    m_AssetManager->SetDeviceManager(m_DeviceManager.get());
    ServiceLocator::Provide(m_AssetManager.get());

    m_InputManager = std::make_unique<InputManager>();
    ServiceLocator::Provide(m_InputManager.get());
}

void Engine::InitSystems()
{
    m_SystemManager = std::make_unique<SystemManager>();

    // create systems
    // they'll fetch dependencies from ServiceLocator or be passed SceneManager
    auto renderSystem = std::make_unique<RenderSystem>(m_SceneManager.get());
    auto statsSystem = std::make_unique<StatsSystem>(renderSystem.get(), m_SceneManager.get());
    auto cameraController = std::make_unique<CameraController>(m_SceneManager.get());

    // the order of registration matters for update order
    m_SystemManager->RegisterSystem(std::move(cameraController));
    m_SystemManager->RegisterSystem(std::move(statsSystem));
    m_SystemManager->RegisterSystem(std::move(renderSystem));

    // initialize all registered systems
    m_SystemManager->InitAll();
}