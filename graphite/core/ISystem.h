#pragma once

// all engine systems to be derived from this class
class ISystem
{
public:
    virtual ~ISystem() = default;
    virtual void Init() = 0;
    virtual void Update() = 0;
    virtual void Shutdown() = 0;
};