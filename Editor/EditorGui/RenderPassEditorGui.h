// BLAEngine Copyright (C) 2016-2020 Vincent Petrella. All rights reserved.

#pragma once

#include "Renderer/RenderPass.h"

namespace BLA
{
    class EditorCommandManager;
    class DevGuiElement;
    class DevGuiTextEditor;
    struct DevGuiElementEventPayload;

    class RenderPassEditorGui
    {
    public:
        RenderPassEditorGui(EditorCommandManager* m_commandManager);

        void UpdateRenderPassEditor();

        void OpenRenderPassEditor();

    private:
        typedef blaMap<blaStringId, DevGuiElement*> ElementMap;

        ElementMap m_elementMap;

        DevGuiTextEditor* m_textEditor = nullptr;
        DevGuiElement* m_renderPassList = nullptr;
    };
}
