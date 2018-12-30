#pragma once
#include "./Common/StdInclude.h"
#include "./Engine/EngineInstance.h"

namespace BLAengine
{
    class CameraController;
	class GameObject;

    class BLACORE_API AnimationDemo : public EngineInstance
    {
    public:
        AnimationDemo(bool external, bool isFullscreen):
            EngineInstance(external, isFullscreen),
			m_selectedObject(nullptr),
            m_frameIndex(0),
			m_lastTimePlayerInteraction(0.f),
			m_autoPlay(true),
			m_lastIkSolveTime(0.f)
        {}

        void PreEngineUpdate() override;

        bool LoadWorkingScene(std::string filepath) override;

        void TerminateEngine() override;

        ~AnimationDemo() { EngineInstance::~EngineInstance(); };

    private:
        CameraController* m_cameraController;
		GameObject* m_selectedObject;
        float m_frameIndex;
		float m_lastTimePlayerInteraction;
		blaF32 m_lastIkSolveTime;
		bool m_autoPlay;
		bool m_clickState = false;
    };
};