#pragma once
#include <System.h>
#include <StdInclude.h>
#include <RenderBackend.h>
#include <Maths/Maths.h>

namespace BLA
{
    typedef blaVector<blaString> DragAndDropPayloadDontStore;

    typedef void(*DragAndDropCallback) (const void* DragAndDropPayload);

    class BLACORE_API RenderWindow
    {
    public:
        RenderWindow() = default;
        virtual ~RenderWindow() {};

        virtual void CreateRenderWindow(blaString windowTitle, int sizeX, int sizeY, bool isFullScreen) = 0;
        virtual blaString GetMaxGLVersion() const = 0;

        virtual void MakeGLContextCurrent() = 0;
        virtual void UpdateWindowAndBuffers() = 0;

        virtual void GetSize(int &width, int &height) const = 0;

        virtual bool isFullScreen() const = 0;

        virtual void SetWindowTitle(blaString title) = 0;
        virtual blaString GetWindowTitle() const = 0;

        virtual void SetMouseXY() = 0;

        virtual void SetMouseCursorLockedAndInvisibleOnMouseButtonHeld(int mouseButton) = 0;

        virtual void SetMouseCursorVisibility(bool visibility) = 0;

        virtual blaVec2 GetMousePointerScreenSpaceCoordinates() = 0;

        virtual blaIVec2 GetMousePositionInWindow() = 0;

        virtual void SetDragAndDropCallback(DragAndDropCallback dragandDropCallback) = 0;

        virtual bool HasCapturedMouse() = 0;
    };

#ifdef GLFW_INTERFACE
    class BLACORE_API GLFWRenderWindow : public RenderWindow
    {
    public:

        void CreateRenderWindow(blaString windowTitle, int sizeX, int sizeY, bool isFullScreen) override;

        blaString GetMaxGLVersion() const override;

        void MakeGLContextCurrent() override;

        void UpdateWindowAndBuffers() override;

        void GetSize(int &width, int &height) const override;

        bool isFullScreen() const override;

        void SetWindowTitle(blaString title) override;
        blaString GetWindowTitle() const override;

        void SetMouseCursorVisibility(bool visibility) override;

        void SetMouseXY() override {};

        void SetMouseCursorLockedAndInvisibleOnMouseButtonHeld(int mouseButton) override;

        void SetDragAndDropCallback(DragAndDropCallback dragAndDropCallback) override;

        //static void InternalDragAndDropCallback

        blaVec2 GetMousePointerScreenSpaceCoordinates() override;

        blaIVec2 GetMousePositionInWindow() override;

        // GLFW Window specific stuff ...

        GLFWwindow* GetWindowPointer() const { return m_glfwWindow; }

        GLFWRenderWindow();
        ~GLFWRenderWindow() override;

        static blaVector<GLFWRenderWindow*> ms_glfwRenderWindowInstanced;

        static void GLFWDragAndDropCallBack(GLFWwindow* glfwWindow, int argc, char** paths);

        bool HasCapturedMouse() override { return !m_cursorVisibility; }

    private:

        GLFWwindow* m_glfwWindow;

        int m_width, m_height;

        bool m_cursorVisibility;

        bool m_isFullscreen;

        DragAndDropCallback m_registeredDragAndDropCallback;

        blaVector<int> m_mouseButtonsThatKillCursorWhenHeld;
    };

#elif defined(WPF_INTERFACE)
    //TODO: Implement MousWheel
    class BLACORE_API WPFRenderWindow : public RenderWindow
    {
    public:
        WPFRenderWindow();
        ~WPFRenderWindow();


        virtual void CreateRenderWindow(blaString windowTitle, int sizeX, int sizeY, bool isFullScreen);

        virtual void UpdateWindowAndBuffers();

        virtual void MakeGLContextCurrent();

        virtual void GetSize(int &width, int &height) const;

        virtual blaString GetMaxGLVersion() const;

        void WriteSize(int x, int y);
        void WriteMousePos(int x, int y);

        virtual bool isFullScreen() const;

        virtual void SetWindowTitle(blaString title);
        virtual blaString GetWindowTitle() const;

        bool ShouldUpdateWindow() const;
        void SetWindowUpdated();

        bool ShouldMakeGLCurrent() const;
        void SetMadeGLCurrent();

        void SetMouseCursorVisibility(bool visibility) override {};

        virtual void SetMouseCursorLockedAndInvisibleOnMouseButtonHeld(int mouseButton) override {};

        blaVec2 GetMousePointerScreenSpaceCoordinates() { return blaVec2(0.f); }

        void SetDragAndDropCallback(DragAndDropCallback dragandDropCallback) override {};

        unsigned char m_mouseDownState;
        bool m_keyPressed[100];

    private:

        int m_width, m_height, m_mousePosX, m_mousePosY;

        bool m_makeGLCurrentRequest;
        bool m_updateWindowRequest;

        DragAndDropCallback m_registeredDragAndDropCallback;

        blaString m_glVersion;
    };
#endif

}
