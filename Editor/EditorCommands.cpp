// BLAEngine Copyright (C) 2016-2020 Vincent Petrella. All rights reserved.

#include "EditorCommands.h"
#include "SceneEditor.h"
#include "System/Console.h"

using namespace BLA;

EditorCommand::EditorCommand(CommandType type) : m_CommandType(type) {}

ValueCommandDelta::ValueCommandDelta(const char* prevV, const char* newV, blaSize size)
{
    m_p = blaVector<char>(prevV, prevV + size);
    m_n = blaVector<char>(newV, newV + size);
}

void ValueCommandDelta::Invert()
{
    blaVector<char> t = m_p;
    m_p = m_n;
    m_n = t;
}

EditorCommand::CommandType EditorCommand::GetType() const
{
	return m_CommandType;
}

GameComponentEditCommand::GameComponentEditCommand(Core::GameObjectID gameObjectId,
    Core::GameComponentID editedComponentId, blaStringId exposedMemberId, ValueCommandDelta valueDelta) :

	EditorCommand(eGameComponentEditCommand),
	m_gameObjectId(gameObjectId),
	m_editedComponentId(editedComponentId),
	m_exposedMemberEditedId(exposedMemberId),
	m_delta(valueDelta)
{}

void GameComponentEditCommand::Invert()
{
    m_delta.Invert();
}

void EditorCommandManager::ClearUndoStack()
{
    while (!m_undoStack.empty())
    {
        delete m_undoStack.top();
        m_undoStack.pop();
    }
}

void EditorCommandManager::ClearRedoStack()
{
	while(!m_redoStack.empty())
	{
        delete m_redoStack.top();
        m_redoStack.pop();
	}
}

EditorCommandManager::EditorCommandManager(SceneEditor* sceneEditor) : m_sceneEditor(sceneEditor) {}

EditorCommandManager::~EditorCommandManager()
{
    ClearRedoStack();
    ClearUndoStack();
}

void EditorCommandManager::Execute(EditorCommand* command, bool internal)
{
    bool validCommand = true;
    switch (command->GetType())
    {
        case EditorCommand::eGameComponentEditCommand:
        {
            const GameComponentEditCommand* compEditCommand = static_cast<const GameComponentEditCommand*>(command);
            validCommand = m_sceneEditor->TemporaryComponentEdit(compEditCommand);
        }
        break;
    }

	if(!internal && validCommand)
	{
        m_undoStack.push(command);
        ClearRedoStack();
	}
}

void EditorCommandManager::Undo()
{
    if (m_undoStack.empty())
        return;

    EditorCommand* command = m_undoStack.top();
    command->Invert();
    Execute(command, true);
    m_undoStack.pop();
    m_redoStack.push(command);
}

void EditorCommandManager::Redo()
{
    if (m_redoStack.empty())
        return;

    EditorCommand* command = m_redoStack.top();
    command->Invert();
    Execute(command, true);
    m_redoStack.pop();
    m_undoStack.push(command);
}

DefineConsoleCommand(void, Undo)
{
    EngineInstance* engine = SceneEditor::GetSingletonInstance();
	if(SceneEditor* editor = dynamic_cast<SceneEditor*>(engine))
	{
        editor->GetCommandManager()->Undo();
	}
}

DefineConsoleCommand(void, Redo)
{
    EngineInstance* engine = SceneEditor::GetSingletonInstance();
    if (SceneEditor* editor = dynamic_cast<SceneEditor*>(engine))
    {
        editor->GetCommandManager()->Redo();
    }
}