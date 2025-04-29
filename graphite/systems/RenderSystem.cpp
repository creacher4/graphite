#include "RenderSystem.h"
#include <cassert>

#include "managers/DeviceManager.h"
#include "managers/AssetManager.h"
#include "ecs/ECSRegistry.h"
#include "StatsSystem.h"
#include <Windows.h>

void RenderSystem::Init()
{
    if (!m_DeviceManager || !m_AssetManager || !m_Registry || !m_Hwnd || !m_Camera)
        throw std::runtime_error("RenderSystem::Init precondition failed: one or more dependencies are null");

    OutputDebugStringA("[RenderSystem] Initialized.\n");

    m_Renderer.Init(
        m_DeviceManager,
        m_Hwnd,
        m_Width,
        m_Height);
}

void RenderSystem::Update(float /*dt*/)
{
    /* OutputDebugStringA("[RenderSystem] Updated.");*/
    m_Renderer.UpdatePerFrameConstants(
        m_Camera->GetView(),
        m_Camera->GetProjection());
    m_Renderer.BeginFrame();
    m_Renderer.GeometryPass(*m_Registry, *m_AssetManager);
    m_Renderer.EndFrame(m_stats);
}

void RenderSystem::OnResize(UINT width, UINT height)
{
    OutputDebugStringA("[RenderSystem] OnResize\n");
    m_Renderer.OnResize(width, height);
}

void RenderSystem::Shutdown()
{
    OutputDebugStringA("[RenderSystem] Shutdown.\n");
}