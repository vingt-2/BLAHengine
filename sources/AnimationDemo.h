#pragma once
#include "./Common/StdInclude.h"
#include "./Engine/EngineInstance.h"

namespace BLAengine
{
    class CameraController;

    class BLACORE_API AnimationDemo : public EngineInstance
    {
    public:
        AnimationDemo(bool external, bool isFullscreen):
            EngineInstance(external, isFullscreen),
            m_frameIndex(0)
        {}

        void PreEngineUpdate() override;

        bool LoadWorkingScene(std::string filepath) override;

        void TerminateEngine() override;

        ~AnimationDemo() { EngineInstance::~EngineInstance(); };

    private:
        CameraController* m_cameraController;
        float m_frameIndex;
    };
};