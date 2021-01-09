// BLAEngine Copyright (C) 2016-2020 Vincent Petrella. All rights reserved.

#include "RenderPassEditorGui.h"

#include "Gui/DevGuiManager.h"
#include "Gui/DevGuiElements.h"
#include "Gui/DevGuiTextEditor.h"
#include "EditorCommands.h"
#include "Core/Scene.h"
#include "Rendering/ShaderGenerator.h"

using namespace BLA;

#define WINDOW_NAME "RenderPass Editor"

#define RENDER_PASS_ELEMENT_GROUP_ID BlaStringId("RenderPassElement")

RenderPassEditorGui::RenderPassEditorGui(EditorCommandManager* m_commandManager)
{
}

void RenderPassEditorGui::UpdateRenderPassEditor()
{
    DevGuiWindow* window = DevGuiManager::GetSingletonInstance()->GetWindow("Scene Graph");

    if (!window)
        return;

    const RenderPassRegistry* registry = RenderPassRegistry::GetSingletonInstanceRead();
    blaVector<blaU32> rpIds;
    registry->GetAllRenderPassIDs(rpIds);

    for(blaU32 id : rpIds)
    {
        if(const Gpu::RenderPassDescriptor* entry = registry->GetRenderPassEntry(id))
        {
            ShaderGenerator s;
            s.m_pDescriptor = entry;

            ShaderCode code;
            s.GenerateVertexShader(code);
            m_textEditor->SetText(code.m_code);
            DevGuiElement* rpElement = new DevGuiCollapsibleElement(blaString(entry->m_name), RENDER_PASS_ELEMENT_GROUP_ID);
            m_renderPassList->AddChild(rpElement);
        }
    }
}

void RenderPassEditorGui::OpenRenderPassEditor()
{
    DevGuiWindow* window = DevGuiManager::GetSingletonInstance()->OpenWindow(WINDOW_NAME);

    DevGuiElement* rootElement = new DevGuiElement("RenderPass", RENDER_PASS_ELEMENT_GROUP_ID);

    window->SetRootElement(rootElement);

    m_textEditor = new DevGuiTextEditor("Shader Editor", RENDER_PASS_ELEMENT_GROUP_ID);
    m_textEditor->SetLanguageDefinition(DevGuiTextEditor::LanguageDefinition::GLSL());

    rootElement->AddChild(m_textEditor);

    m_renderPassList = new DevGuiElement("Render Passes", RENDER_PASS_ELEMENT_GROUP_ID);

    rootElement->AddChild(m_renderPassList);

    UpdateRenderPassEditor();
}
