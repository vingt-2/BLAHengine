#pragma once
#include "../../Common/System.h"
#include "../../Common/StdInclude.h"

namespace BLAengine
{
    class BLACORE_API RenderWindow
    {
    public:
        virtual void CreateRenderWindow(string windowTitle, int sizeX, int sizeY, bool isFullScreen) = 0;
        virtual string GetMaxGLVersion() = 0;

        virtual void  MakeGLContextCurrent() = 0;
        virtual void UpdateWindowAndBuffers() = 0;

        virtual void GetSize(int &width, int &height) = 0;

        virtual bool isFullScreen() = 0;

        virtual void SetWindowTitle(std::string title) = 0;
        virtual std::string GetWindowTitle() = 0;

        virtual void GetMouse(double &x, double &y) = 0;

        virtual void SetMouseXY() = 0;

        virtual bool GetMousePressed(int button) = 0;
        virtual bool GetKeyPressed(int key) = 0;

		virtual void SetMouseCursorVisibility(bool visibility) = 0;
    };

    class BLACORE_API WPFRenderWindow : public RenderWindow
    {
    public:
        WPFRenderWindow();

        virtual void CreateRenderWindow(string windowTitle, int sizeX, int sizeY, bool isFullScreen);
        
        virtual void UpdateWindowAndBuffers();

        virtual void MakeGLContextCurrent();

        virtual void GetSize(int &width, int &height);

        virtual void GetMouse(double &x, double &y);

        virtual void SetMouseXY();

        virtual bool GetKeyPressed(int key);
        virtual bool GetMousePressed(int button);

        virtual string GetMaxGLVersion();

        void WriteSize(int x, int y);
        void WriteMousePos(int x, int y);

        virtual bool isFullScreen();

        virtual void SetWindowTitle(string title);
        virtual string GetWindowTitle();

        bool ShouldUpdateWindow();
        void SetWindowUpdated();

        bool ShouldMakeGLCurrent();
        void SetMadeGLCurrent();

		void SetMouseCursorVisibility(bool visibility) override {};
    
        unsigned char m_mouseDownState;
        bool m_keyPressed[100];

    private:

        int m_width, m_height, m_mousePosX, m_mousePosY;
        
        bool m_makeGLCurrentRequest;
        bool m_updateWindowRequest;

        string m_glVersion;
    };

#ifdef GLFW_INTERFACE
    class BLACORE_API GLFWRenderWindow : public RenderWindow
    {
    public:

        virtual void CreateRenderWindow(string windowTitle, int sizeX, int sizeY, bool isFullScreen);
        virtual string GetMaxGLVersion();
        virtual void MakeGLContextCurrent();
        virtual void UpdateWindowAndBuffers();
        virtual void GetSize(int &width, int &height);

        virtual bool isFullScreen();

        virtual void SetWindowTitle(std::string title);
        virtual std::string GetWindowTitle();

        virtual void GetMouse(double &x, double &y);

        virtual void SetMouseXY();

        virtual bool GetKeyPressed(int key);
        virtual bool GetMousePressed(int button);

		void SetMouseCursorVisibility(bool visibility) override;

        GLFWRenderWindow();
        ~GLFWRenderWindow();

    private:

        GLFWwindow* m_glfwWindow;

        int m_width, m_height;

        bool m_isFullscreen;

    };
#endif
}
