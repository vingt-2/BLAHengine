#pragma once

#include "Core/GameComponent.h"
#include "Core/InspectableVariables.h"
#include <stack>

//TODO: implement simple RTTI in macros to enable it on a few classes

namespace BLA
{
	class SceneEditor;

	struct ValueCommandDelta
	{
		virtual void* GetPreviousValue() = 0;
		virtual void* GetNewValue() = 0;
		virtual blaIndex GetValueSizeBytes() = 0;
	};

	template<typename T>
	struct TypedValueCommandDelta : ValueCommandDelta
	{
		T m_prevValue;
		T m_newValue;
		
		void* GetPreviousValue() override { return &m_prevValue; }
		void* GetNewValue() override { return &m_newValue; }

		blaIndex GetValueSizeBytes() override { return sizeof(T); }
	};
	
	struct EditorCommand
	 {
		enum CommandType
		{
			eGameComponentEditCommand
		};

		CommandType GetType() const;
		
	private:
		CommandType m_CommandType;
	protected:
		EditorCommand(CommandType type);
	};

	struct GameComponentEditCommand : EditorCommand
	{
		GameObjectID m_gameObjectId;
		const ComponentDescriptor* m_editedComponentDescriptor;
		const ComponentDescriptor::ExposedMember* m_exposedMemberEdited;
		const ValueCommandDelta* m_delta;

        GameComponentEditCommand();
		~GameComponentEditCommand();
	};
	
	class EditorCommandManager
	{
		std::stack<const EditorCommand*> m_undoStack;
		std::stack<const EditorCommand*> m_redoStack;

		SceneEditor* m_sceneEditor;
	public:
		EditorCommandManager(SceneEditor* pSceneEditor);
		void Execute(const EditorCommand*);
	};
};