#include "SceneManager.h"
#include "System/Console.h"
#include "Core/GameComponent.h"
#include "External/rapidjson/prettywriter.h"
#include "External/rapidjson/document.h"
#include <stack>

using namespace BLA;

BLA::SceneManager::SceneManager()
{
    m_currentSceneFilePath = "";
}

class BLASerializeWriter : public rapidjson::PrettyWriter<rapidjson::StringBuffer>
{
    using rapidjson::PrettyWriter<rapidjson::StringBuffer>::PrettyWriter;
};

bool SceneManager::SaveScene(blaString filepath, Scene* scene)
{
    rapidjson::StringBuffer sb;
    BLASerializeWriter writer(sb);

    writer.StartObject();
    writer.Key("BLAEngineVersion");
    writer.String(std::to_string(BLA_ENGINE_VERSION).c_str());
    writer.Key((blaString("Scene:") + filepath).c_str()); //TODO: Move to Scene Name
    writer.StartObject();
    for(GameObject gameObject : scene->GetObjects()) 
    {
        writer.SetFormatOptions(rapidjson::kFormatDefault);
        writer.Key(ToString(gameObject.GetId()).c_str());
        writer.StartObject();
        for (GameComponent* c : gameObject.GetAllComponents())
        {
            writer.SetFormatOptions(rapidjson::kFormatDefault);
            writer.Key(ToString(c->GetComponentDescriptor().GetName()).c_str());
            c->GetComponentDescriptor().Serialize(c, &writer);
        }
        writer.EndObject();
    }
    writer.EndObject();
    writer.EndObject();

    std::ofstream fs;
    fs.open(filepath);

    if (!fs.is_open())
    {
        Console::LogMessage("Could not Write on file " + filepath);
        return false;
    }

    fs << sb.GetString();

    m_currentSceneFilePath = filepath;

    return true;
}

#define HANDLE_DESERIALIZER                                  \
    if (deserializer == POP_DESERIALIZER)                    \
    {                                                        \
        delete m_deserializers.top(); m_deserializers.pop(); \
    }                                                        \
    else if(deserializer == DESERIALIZE_ERROR)               \
    {                                                        \
                                                             \
    }                                                        \
    else if(deserializer != nullptr)                         \
    {                                                        \
        m_deserializers.push(deserializer);                  \
    }                                                        \
   

struct SceneParser : public rapidjson::BaseReaderHandler<rapidjson::UTF8<>, SceneParser>
{
    using Deserializer = BLAInspectableVariables::ExposedVarTypeDescriptor::Deserializer;

    SceneParser(Scene* newScene) : m_scene(newScene), m_parseState(States::SceneDesc) {}

    bool Null()
    {
        if(!m_deserializers.empty())  
        {
            Deserializer* deserializer = m_deserializers.top()->Null();
            HANDLE_DESERIALIZER
        }
        return true;
    };
    bool Bool(bool b)
    {
        if (!m_deserializers.empty()) 
        {
            Deserializer* deserializer = m_deserializers.top()->Bool(b);
            HANDLE_DESERIALIZER
        }
        return true;
    };
    bool Int(int i)
    {
        if (!m_deserializers.empty()) 
        {
            Deserializer* deserializer = m_deserializers.top()->Int(i);
            HANDLE_DESERIALIZER
        }
        return true;
    };
    bool Uint(unsigned i)
    {
        if (!m_deserializers.empty()) 
        {
            Deserializer* deserializer = m_deserializers.top()->Uint(i);
            HANDLE_DESERIALIZER
        }
        return true;
    };
    bool Int64(int64_t i) 
    {
        if (!m_deserializers.empty()) 
        {
            Deserializer* deserializer = m_deserializers.top()->Int64(i);
            HANDLE_DESERIALIZER
        }
        return true;
    };
    bool Uint64(uint64_t i)
    {
        if (!m_deserializers.empty()) 
        {
            Deserializer* deserializer = m_deserializers.top()->Uint64(i);
            HANDLE_DESERIALIZER
        }
        return true;
    };
    bool Double(double d)
    {
        if (!m_deserializers.empty()) 
        {
            Deserializer* deserializer = m_deserializers.top()->Double(d);
            HANDLE_DESERIALIZER
        }
        return true;
    };
    bool RawNumber(const char* str, size_t length, bool copy)
    {
        if (!m_deserializers.empty()) 
        {
            Deserializer* deserializer = m_deserializers.top()->RawNumber(str,length,copy);
            HANDLE_DESERIALIZER
        }
        return true;
    };
    //TODO: If detected an error in current parsing, do nothing until end object ...
    bool String(const char* str, size_t  length, bool copy)
    {
        if (!m_deserializers.empty()) 
        {
            Deserializer* deserializer = m_deserializers.top()->String(str, length, copy);
            HANDLE_DESERIALIZER
        }
        return true;
    };
    bool StartObject()
    {
        if (!m_deserializers.empty()) 
        {
            Deserializer* deserializer = m_deserializers.top()->StartObject();
            HANDLE_DESERIALIZER
        }
        return true;
    }
    bool Key(const char* str, size_t  length, bool copy)
    {
        if(!m_deserializers.empty())  
        {
            Deserializer* deserializer = m_deserializers.top()->Key(str, length, copy);
            HANDLE_DESERIALIZER
            return true;
        }
        
        if (m_parseState == SceneDesc)
        {
            if(blaString(str, length).substr(0, 6) == "Scene:") 
            {
                m_parseState = ObjectEnum;
            }
            return true;
        }

        if(m_parseState == ObjectEnum) 
        {
            m_currentObjectID = FromString(blaString(str, length));
            m_parseState = ComponentEnum;
            return true;
        }

        if (m_parseState == ComponentEnum)
        {
            blaStringId componentId = FromString(blaString(str, length));
            const ComponentDescriptor& componentDescriptor = GameComponentRegistry::GetSingletonInstance()->GetComponentDescriptor(componentId);
            if(componentDescriptor.GetName() != INVALID_COMPONENT_ID) 
            {
                void* obj = m_scene->AddComponent(GameObject(m_currentObjectID), componentId);
                m_deserializers.push(componentDescriptor.GetDeserializer(obj));
                return true;
            }
        }
        return false;
    }
    bool EndObject(size_t memberCount)
    {
        if (!m_deserializers.empty()) 
        {
            Deserializer* deserializer = m_deserializers.top()->EndObject(memberCount);
            HANDLE_DESERIALIZER
            return true;
        }

        if (m_parseState == SceneDesc)
        {
            Console::LogMessage("End parse scene succesfully");
            return true;
        }

        if (m_parseState == ObjectEnum)
        {
            m_parseState = SceneDesc;
            return true;
        }

        if (m_parseState == ComponentEnum)
        {
            m_parseState = ObjectEnum;
            return true;
        }

        return false;
    }
    bool StartArray()
    {
        if (!m_deserializers.empty()) 
        {
            Deserializer* deserializer = m_deserializers.top()->StartArray();
            HANDLE_DESERIALIZER
            return true;
        }
        return false;
    }
    bool EndArray(size_t elementCount)
    {
        if (!m_deserializers.empty()) 
        {
            Deserializer* deserializer = m_deserializers.top()->EndArray(elementCount);
            HANDLE_DESERIALIZER
            return true;
        }
        return false;
    }

private:
    std::stack<Deserializer*> m_deserializers;
    Scene* m_scene;
    GameObjectID m_currentObjectID;

    enum States
    {
        SceneDesc,
        ObjectEnum,
        ComponentEnum
    } m_parseState;
};

void SceneManager::LoadScene(blaString filepath)
{
    using namespace rapidjson;

    std::ifstream fs;
    fs.open(filepath);

    std::ifstream ifs(filepath);
    blaString json((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());

    StringStream a(json.c_str());

    Scene::GetSingletonInstance()->Clear();

    Reader reader;
    SceneParser sceneParser(Scene::GetSingletonInstance());
    reader.Parse(a, sceneParser);

    m_currentSceneFilePath = filepath;
}