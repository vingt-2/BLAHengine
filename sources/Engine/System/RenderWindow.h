#pragma once
#include <Common/System.h>
#include <Common/StdInclude.h>
#include <Common/RenderBackend.h>
#include <Common/Maths/Maths.h>

namespace BLAengine
{
    
    typedef std::vector<string> DragAndDropPayloadDontStore;
    

    typedef void (*DragAndDropCallback) (const void* DragAndDropPayload);

    class BLACORE_API RenderWindow
    {
    public:
        RenderWindow() {};
        ~RenderWindow() {};

        virtual void CreateRenderWindow(string windowTitle, int sizeX, int sizeY, bool isFullScreen) = 0;
        virtual string GetMaxGLVersion() const = 0;

        virtual void  MakeGLContextCurrent() = 0;
        virtual void UpdateWindowAndBuffers() = 0;

        virtual void GetSize(int &width, int &height) const = 0;

        virtual bool isFullScreen() const = 0;

        virtual void SetWindowTitle(std::string title) = 0;
        virtual std::string GetWindowTitle() const = 0;

        virtual void SetMouseXY() = 0;

        virtual void SetMouseCursorLockedAndInvisibleOnMouseButtonHeld(int mouseButton) = 0;

        virtual void SetMouseCursorVisibility(bool visibility) = 0;

        virtual blaVec2 GetMousePointerScreenSpaceCoordinates() = 0;

        virtual void SetDragAndDropCallback(DragAndDropCallback dragandDropCallback) = 0;
    };

    //TODO: Implement MousWheel
    class BLACORE_API WPFRenderWindow : public RenderWindow
    {
    public:
        WPFRenderWindow();
        ~WPFRenderWindow();


        virtual void CreateRenderWindow(string windowTitle, int sizeX, int sizeY, bool isFullScreen);
        
        virtual void UpdateWindowAndBuffers();

        virtual void MakeGLContextCurrent();

        virtual void GetSize(int &width, int &height) const;

        virtual string GetMaxGLVersion() const;

        void WriteSize(int x, int y);
        void WriteMousePos(int x, int y);

        virtual bool isFullScreen() const;

        virtual void SetWindowTitle(string title);
        virtual string GetWindowTitle() const;

        bool ShouldUpdateWindow() const;
        void SetWindowUpdated();

        bool ShouldMakeGLCurrent() const;
        void SetMadeGLCurrent();

        void SetMouseCursorVisibility(bool visibility) override {};

        virtual void SetMouseCursorLockedAndInvisibleOnMouseButtonHeld(int mouseButton) override {};

        blaVec2 GetMousePointerScreenSpaceCoordinates(){ return blaVec2(0.f); }

        void SetDragAndDropCallback(DragAndDropCallback dragandDropCallback) override {};
    
        unsigned char m_mouseDownState;
        bool m_keyPressed[100];

    private:

        int m_width, m_height, m_mousePosX, m_mousePosY;
        
        bool m_makeGLCurrentRequest;
        bool m_updateWindowRequest;

        DragAndDropCallback m_registeredDragAndDropCallback;

        string m_glVersion;
    };

#ifdef GLFW_INTERFACE
    class BLACORE_API GLFWRenderWindow : public RenderWindow
    {
    public:

        void CreateRenderWindow(string windowTitle, int sizeX, int sizeY, bool isFullScreen) override;

        string GetMaxGLVersion() const override;

        void MakeGLContextCurrent() override;

        void UpdateWindowAndBuffers() override;

        void GetSize(int &width, int &height) const override;

        bool isFullScreen() const override;

        void SetWindowTitle(std::string title) override;
        std::string GetWindowTitle() const override;

        void SetMouseCursorVisibility(bool visibility) override;

        void SetMouseXY() override {};

        void SetMouseCursorLockedAndInvisibleOnMouseButtonHeld(int mouseButton) override;

        void SetDragAndDropCallback(DragAndDropCallback dragAndDropCallback) override;

        //static void InternalDragAndDropCallback

        blaVec2 GetMousePointerScreenSpaceCoordinates() override;

        // GLFW Window specific stuff ...

        GLFWwindow* GetWindowPointer() { return m_glfwWindow; }

        GLFWRenderWindow();
        ~GLFWRenderWindow();

        static std::vector<GLFWRenderWindow*> ms_glfwRenderWindowInstanced;

        static void GLFWDragAndDropCallBack(GLFWwindow* glfwWindow, int argc, char** paths);

    private:

        GLFWwindow* m_glfwWindow;

        int m_width, m_height;

        bool m_isFullscreen;

        DragAndDropCallback m_registeredDragAndDropCallback;

        std::vector<int> m_mouseButtonsThatKillCursorWhenHeld;
    };
#endif
}
