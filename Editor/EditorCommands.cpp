#include "EditorCommands.h"
#include "SceneEditor.h"

using namespace BLA;

EditorCommand::EditorCommand(CommandType type) : m_CommandType(type) {}

EditorCommand::CommandType EditorCommand::GetType() const
{
	return m_CommandType;
}

GameComponentEditCommand::GameComponentEditCommand(): EditorCommand(eGameComponentEditCommand)
{
}

GameComponentEditCommand::~GameComponentEditCommand()
{
	delete m_delta;
}


EditorCommandManager::EditorCommandManager(SceneEditor* sceneEditor) : m_sceneEditor(sceneEditor)
{
}

void EditorCommandManager::Execute(const EditorCommand* command)
{
    switch (command->GetType())
    {
        case EditorCommand::eGameComponentEditCommand:
        {
            
        }
        break;
    }
}
