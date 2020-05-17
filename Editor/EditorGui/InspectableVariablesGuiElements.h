#pragma once

#include "Core/InspectableVariables.h"
#include "Gui/GuiElements.h"

/*
 * The descriptors in InspectableVariables.h used to carry a function to create the (editor specific) GuiElements used to edit each type.
 * This required include BLAGuiElements.h into each translation unit using InspectableVariables (there's a lot of them).
 * I've decoupled this, moved the concept of GuiElementsEditor outside of core and not used in BLAEngine.dll (eventhough the debug gui supporting it does ship in BLAEngine.dll)
 * So now we have an extra level of indirection interfacing with the variable TypeId. (Same will be done (or is already done) with serializer / deserializers)
 */

namespace BLA
{
	struct InspectableVariablesEditorGuiElementFactoryManager // You read that right. Don't judge me. It is what is
	{
		static BlaGuiElement* MakeEditGuiElement(const blaString& variableName, blaStringId groupId, const BLAInspectableVariables::ExposedVarTypeDescriptor* typeDescriptor, void* obj)
		{
			Factories::const_iterator it = ms_factories.find(typeDescriptor->m_typeID);
			if(it != ms_factories.end())
			{
				return it->second(variableName, groupId, obj);
			}
			return new BlaGuiSimpleTextElement(variableName, groupId, "Unknown Type");
		}
		typedef BlaGuiElement* (*GetEditorFactory)(const blaString&, blaStringId, void*);

		static void __RegisterFactory(blaStringId type, GetEditorFactory factory);
		
	private:
		
		typedef blaMap<blaStringId, GetEditorFactory> Factories;

		static Factories ms_factories;
	};
}