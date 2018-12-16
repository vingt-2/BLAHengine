#pragma once
#include "../../Common/System.h"
#include "../../Common/StdInclude.h"

namespace BLAengine
{
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

        virtual void GetMouse(double &x, double &y) const = 0;

        virtual void SetMouseXY() = 0;

        virtual bool GetMousePressed(int button) const = 0;
        virtual bool GetKeyPressed(int key) const = 0;

        virtual void SetMouseCursorLockedAndInvisibleOnMouseButtonHeld(int mouseButton) = 0;

        virtual void SetMouseCursorVisibility(bool visibility) = 0;
    };

    class BLACORE_API WPFRenderWindow : public RenderWindow
    {
    public:
        WPFRenderWindow();
        ~WPFRenderWindow();


        virtual void CreateRenderWindow(string windowTitle, int sizeX, int sizeY, bool isFullScreen);
        
        virtual void UpdateWindowAndBuffers();

        virtual void MakeGLContextCurrent();

        virtual void GetSize(int &width, int &height) const;

        virtual void GetMouse(double &x, double &y) const;

        virtual void SetMouseXY();

        virtual bool GetKeyPressed(int key) const;
        virtual bool GetMousePressed(int button) const;

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

        void CreateRenderWindow(string windowTitle, int sizeX, int sizeY, bool isFullScreen) override;

        string GetMaxGLVersion() const override;

        void MakeGLContextCurrent() override;

        void UpdateWindowAndBuffers() override;

        void GetSize(int &width, int &height) const override;

        bool isFullScreen() const override;

        void SetWindowTitle(std::string title) override;
        std::string GetWindowTitle() const override;

        void GetMouse(double &x, double &y) const override;

        void SetMouseXY() override;

        bool GetKeyPressed(int key) const override;
        bool GetMousePressed(int button) const override;

        void SetMouseCursorVisibility(bool visibility) override;

        void SetMouseCursorLockedAndInvisibleOnMouseButtonHeld(int mouseButton) override;

        GLFWRenderWindow();
        ~GLFWRenderWindow();

    private:

        GLFWwindow* m_glfwWindow;

        int m_width, m_height;

        bool m_isFullscreen;

        std::vector<int> m_mouseButtonsThatKillCursorWhenHeld;
    };
#endif
}
