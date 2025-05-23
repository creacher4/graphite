#include "RenderSystem.h"
#include <cassert>
#include "utils/Logger.h"

#include "managers/DeviceManager.h"
#include "managers/AssetManager.h"
#include "ecs/ECSRegistry.h"
#include "StatsSystem.h"
#include <Windows.h>

RenderSystem::RenderSystem(
    DeviceManager *deviceManager,
    AssetManager *assetManager,
    ECSRegistry *registry,
    Camera *camera,
    HWND hwnd,
    UINT width,
    UINT height)
    : m_DeviceManager(deviceManager),
      m_AssetManager(assetManager),
      m_Registry(registry),
      m_Camera(camera),
      m_Hwnd(hwnd),
      m_Width(width),
      m_Height(height)
{
}

void RenderSystem::Init()
{
    LOG_INFO("Initializing render system...");
    if (!m_DeviceManager || !m_AssetManager || !m_Registry || !m_Hwnd || !m_Camera)
        throw std::runtime_error("RenderSystem dependencies not set");

    LOG_INFO("Render system initialized");

    m_Renderer.Init(
        m_DeviceManager,
        m_Hwnd,
        m_Width,
        m_Height);
}

void RenderSystem::Update(float /*dt*/)
{
    m_Renderer.UpdatePerFrameConstants(
        m_Camera->GetView(),
        m_Camera->GetProjection());
    m_Renderer.BeginFrame();
    m_Renderer.GeometryPass(*m_Registry, *m_AssetManager);
    m_Renderer.EndFrame(m_stats);
}

void RenderSystem::OnResize(UINT width, UINT height)
{
    m_Renderer.OnResize(width, height);
}

void RenderSystem::Shutdown()
{
}