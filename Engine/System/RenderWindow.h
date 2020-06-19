// BLAEngine Copyright (C) 2016-2020 Vincent Petrella. All rights reserved.

#pragma once
#include "System.h"
#include "StdInclude.h"

#include "Maths/Maths.h"

struct GLFWwindow;
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

        virtual void UpdateWindowAndBuffers() = 0;

        virtual void GetSize(int &width, int &height) const = 0;

        virtual bool isFullScreen() const = 0;

        virtual void SetWindowTitle(blaString title) = 0;
        virtual blaString GetWindowTitle() const = 0;

        virtual void SetMouseXY() = 0;

        virtual void SetMouseCursorVisibility(bool visibility) = 0;

        virtual blaVec2 GetMousePointerScreenSpaceCoordinates() = 0;

        virtual blaIVec2 GetMousePositionInWindow() = 0;

        virtual void SetDragAndDropCallback(DragAndDropCallback dragandDropCallback) = 0;
    };

    class BLACORE_API GLFWRenderWindow : public RenderWindow
    {
    public:

        static void InitGLFW();
    	
        static void ShutdownGLFW();
    
        void UpdateWindowAndBuffers() override;

        void GetSize(int &width, int &height) const override;

        bool isFullScreen() const override;

        void SetWindowTitle(blaString title) override;
        blaString GetWindowTitle() const override;

        void SetMouseCursorVisibility(bool visibility) override;

        void SetMouseXY() override {};

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

    protected:

        GLFWwindow* m_glfwWindow;

        int m_width, m_height;

        bool m_cursorVisibility;

        bool m_isFullscreen;

        DragAndDropCallback m_registeredDragAndDropCallback;
    };

    void GLFWKeyboardCallBack(GLFWwindow* window, int keyCode, int scandone, int action, int mods);
    void GLFWMouseButtonCallBack(GLFWwindow* window, int button, int action, int mods);
    void GLFWMouseCursorPosCallBack(GLFWwindow* window, double xpos, double ypos);
    void GLFWMouseWheelCallback(GLFWwindow* window, double xAxisScroll, double yAxisScroll);

#if NEW_VULKAN_RENDERER
    struct VulkanContext;
    struct VulkanWindowInfo;
    class BLACORE_API GLFWVulkanRenderWindow : public GLFWRenderWindow
    {
    public:
        virtual ~GLFWVulkanRenderWindow();

        void CreateRenderWindow(blaString windowTitle, int sizeX, int sizeY, bool isFullScreen) override;
        void UpdateWindowAndBuffers() override;
        const VulkanContext* GetVulkanContext() const;
        VulkanWindowInfo* GetVulkanWindowInfo() const;
    private:
        void CreateSwapChain();
        void CreateSwapChainCommandBuffers();

        void DestroySwapChainAndCommandBuffers();

        const VulkanContext* m_vulkanContext;
        VulkanWindowInfo* m_vulkanWindowInfo;

        static const VulkanContext* SetupVulkanContext(const char** extensions, uint32_t extensions_count);
    };
#else
    class BLACORE_API GLFWOpenGLRenderWindow : public GLFWRenderWindow
    {
    public:
        void CreateRenderWindow(blaString windowTitle, int sizeX, int sizeY, bool isFullScreen) override;
        void UpdateWindowAndBuffers() override;
        void MakeGLContextCurrent();
    };
#endif
}
