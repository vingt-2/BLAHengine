#include "InspectableVariablesGuiElements.h"
#include "Maths/Maths.h"

using namespace BLA;

InspectableVariablesEditorGuiElementFactoryManager::Factories InspectableVariablesEditorGuiElementFactoryManager::ms_factories;

void InspectableVariablesEditorGuiElementFactoryManager::__RegisterFactory(blaStringId type, GetEditorFactory factory)
{
	ms_factories.insert(std::make_pair(type, factory));
}

struct InspectableVariablesEditorGuiElementFactoryRegistrator
{
	InspectableVariablesEditorGuiElementFactoryRegistrator(blaStringId typeId, InspectableVariablesEditorGuiElementFactoryManager::GetEditorFactory factory)
	{
		InspectableVariablesEditorGuiElementFactoryManager::__RegisterFactory(typeId, factory);
	}
};

template<typename T>
BlaGuiElement* MakeEditGuiElement(const blaString& variableName, blaStringId groupId, void* obj)
{
	return new BlaGuiEditElement<T>(variableName, groupId, static_cast<T*>(obj));
}

#define RegisterInspectableVariableGuiElementEditorFactory(Type) \
	InspectableVariablesEditorGuiElementFactoryRegistrator g_TypeGuiElementEditorFactory##Type##Registartor(BLAInspectableVariables::TypeResolver<Type>::get()->GetTypeID(), MakeEditGuiElement<Type>);


RegisterInspectableVariableGuiElementEditorFactory(blaBool);
RegisterInspectableVariableGuiElementEditorFactory(blaS32);
RegisterInspectableVariableGuiElementEditorFactory(blaF32);
RegisterInspectableVariableGuiElementEditorFactory(blaString);
RegisterInspectableVariableGuiElementEditorFactory(GameObject);
RegisterInspectableVariableGuiElementEditorFactory(blaVec2)
RegisterInspectableVariableGuiElementEditorFactory(blaVec3)
RegisterInspectableVariableGuiElementEditorFactory(blaQuat)
RegisterInspectableVariableGuiElementEditorFactory(blaPosQuat)
RegisterInspectableVariableGuiElementEditorFactory(blaScaledTransform);
