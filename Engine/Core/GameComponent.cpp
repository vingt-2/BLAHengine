#include "GameComponent.h"

#include <System/Console.h>

using namespace BLAengine;

ComponentDescriptor ms_emptyDescriptor{ GameComponent::InitReflection };

GameComponent::GameComponent(GameObject parentObject) : m_parentObject(parentObject)
{}

GameComponent::~GameComponent()
{}

void InitEmptyDesc(ComponentDescriptor* d) { d->m_typeName = INVALID_COMPONENT_ID; };
ComponentDescriptor g_emptyDescriptor{ InitEmptyDesc };

const ComponentDescriptor& GameComponent::GetComponentDescriptor() const
{
    return g_emptyDescriptor;
}

void GameComponentRegistry::__RegisterComponent(blaStringId name, GameComponentRegistryEntry::ComponentFactory factory)
{
    m_componentsPerLibraries[m_currentRegisteringLibrary].push_back(GameComponentRegistry::GameComponentRegistryEntry(name, factory));
}

std::vector<blaString> GameComponentRegistry::ListComponentNames()
{
    std::vector<blaString> names;
    for(auto p : m_componentsPerLibraries) 
    {
        for (auto e : p.second)
        {
            names.push_back(blaString(e.GetName())); // Did you know? At some point, in release, this will just be a number
            // And we're not just a number.
        }
    }
    return names;
}

const ComponentDescriptor& GameComponentRegistry::GetComponentDescriptor(blaStringId name)
{
    for (auto p : m_componentsPerLibraries)
    {
        for (auto e : p.second)
        {
            if (e.GetName() == name)
            {
                GameObject r;
                return e.CreateComponent(r)->GetComponentDescriptor();
            }
        }
    }
    return g_emptyDescriptor;
}

GameComponent* GameComponentRegistry::__CreateComponent(GameComponentID componentName, GameObject objRef)
{
    for (auto p : m_componentsPerLibraries)
    {
        CompEntries::iterator findIt = std::find_if(p.second.begin(), p.second.end(), [componentName](GameComponentRegistryEntry e) { return e.GetName() == componentName; });
        if (findIt != p.second.end())
        {
            return findIt->CreateComponent(objRef);
        }
    }

    return nullptr;
}

void GameComponentRegistry::UnloadLibraryComponents(blaStringId libraryId)
{
    // TODO: Obviously, we need to clean the scene as well ...
    for (auto p : m_componentsPerLibraries)
    {
        if(p.first == libraryId) 
        {
            p.second.clear();
            m_componentsPerLibraries.erase(p.first);
        }
    }
}

BLA_IMPLEMENT_SINGLETON(GameComponentRegistry);

DefineConsoleCommand(int, GetComponentCount)
{
    return GameComponentRegistry::GetSingletonInstance()->ListComponentNames().size();
}

DefineConsoleCommand(void, GetComponentList)
{
	for (auto componentName : GameComponentRegistry::GetSingletonInstance()->ListComponentNames())
	{
		Console::LogMessage(componentName);
	}
}

// TODO: Get Rid of me ...
#include "External/rapidjson/prettywriter.h"
#include "Core/GameObject.h"
class BLASerializeWriter : public rapidjson::PrettyWriter<rapidjson::StringBuffer>
{};

void BLACORE_API ComponentDescriptor::Serialize(void* obj, BLASerializeWriter* writer) const
{
    writer->StartObject();
    for(const ExposedMember& member : m_members) 
    {
        writer->Key(ToString(member.m_name).c_str());
        member.m_type->Serialize((char*)obj + member.m_offset, writer);
    }
    writer->EndObject();
}

BLAInspectableVariables::ExposedVarTypeDescriptor::Deserializer* ComponentDescriptor::TypedDeserializer::StartObject()
{
    return nullptr;
}

BLAInspectableVariables::ExposedVarTypeDescriptor::Deserializer* ComponentDescriptor::TypedDeserializer::Key(const char* str, size_t length, bool copy)
{
    blaStringId varName = FromString(blaString(str, length));
    for(int i = 0; i < m_componentDescriptor->m_members.size(); ++i)
    {
        if(m_componentDescriptor->m_members[i].m_name == varName)
        {
            return m_componentDescriptor->m_members[i].m_type->GetDeserializer((char*)m_obj + m_componentDescriptor->m_members[i].m_offset);
        }
    }
    return DESERIALIZE_ERROR;
}

BLAInspectableVariables::ExposedVarTypeDescriptor::Deserializer* ComponentDescriptor::TypedDeserializer::EndObject(size_t memberCount)
{
    return POP_DESERIALIZER;
}

void ComponentDescriptor::TypedDeserializer::SetComponentDescriptor(const ComponentDescriptor* componentDescriptor)
{
    m_componentDescriptor = componentDescriptor;
}

BLAInspectableVariables::ExposedVarTypeDescriptor::Deserializer* ComponentDescriptor::GetDeserializer(void* obj) const
{
    TypedDeserializer* deserializer = new TypedDeserializer(obj);
    deserializer->SetComponentDescriptor(this);
    return deserializer;
}
