#pragma once
#include "..\..\..\Common\StdInclude.h"
#include "../../../Common/Maths.h"
#include "GameComponent.h"

namespace BLAengine
{
    class BLACORE_API Camera : public GameComponent
    {
    public:
        // Members
        Transform    m_viewTransform;
        vec2        m_fovSize;

        // States
        bool m_isControlEnabled;


        // Functions
        void UpdateView();
        void Update();

        Camera();
        ~Camera(void);

    private:
    };


}