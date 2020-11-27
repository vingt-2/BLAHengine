// BLAEngine Copyright (C) 2016-2020 Vincent Petrella. All rights reserved.

#pragma once

#include "Core/GameComponent.h"
#include "Core/InspectableVariables.h"
#include <stack>

//TODO: implement simple RTTI in macros to enable it on a few classes

namespace BLA
{
	class SceneEditor;

	class ValueCommandDelta
	{
		blaVector<char> m_p;
		blaVector<char> m_n;
		
	public:
		ValueCommandDelta(const char* prevV, const char* newV, blaSize size);
		void Invert();
		const char* GetPreviousValue() const { return &m_p[0]; }
		const char* GetNewValue() const { return &m_n[0]; }
		blaSize GetValueSizeBytes() const { return m_p.size(); }
	};
	
	struct EditorCommand
	 {
		enum CommandType
		{
			eGameComponentEditCommand
		};

		CommandType GetType() const;

		virtual void Invert() = 0;
		
	private:
		CommandType m_CommandType;
	protected:
		EditorCommand(CommandType type);
	};

	struct GameComponentEditCommand : EditorCommand
	{
		Core::GameObjectID m_gameObjectId;
		Core::GameComponentID m_editedComponentId;
		blaStringId m_exposedMemberEditedId;
		ValueCommandDelta m_delta;

		GameComponentEditCommand(Core::GameObjectID gameObjectId,
		                         Core::GameComponentID editedComponentId,
		                         blaStringId exposedMemberId,
		                         ValueCommandDelta valueDelta);

		void Invert() override;
	};
	
	class EditorCommandManager
	{
		std::stack<EditorCommand*> m_undoStack;
		std::stack<EditorCommand*> m_redoStack;

		SceneEditor* m_sceneEditor;

		// This is where we delete all editor commands, nowhere else should we delete them
		void ClearUndoStack();
		void ClearRedoStack();
	public:
		EditorCommandManager(SceneEditor* pSceneEditor);
		~EditorCommandManager();
		void Execute(EditorCommand* command, bool internal = false);
		void Undo();
		void Redo();
	};
};