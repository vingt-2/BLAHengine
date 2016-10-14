#pragma once
#include "../Engine/Game/Scene.h"

#include <cereal\cereal.hpp>
#include <cereal\types\polymorphic.hpp>
#include <cereal\types\vector.hpp>
#include <cereal\types\string.hpp>
#include <cereal\archives\xml.hpp>

class TransformSerializer
{
public:

	TransformSerializer() = default;

	void FromTransform(BLAengine::Transform* transform)
	{
		m_position.FillData(transform->m_position);
		m_rotation.FillData(transform->m_rotation);
		m_scale.FillData(transform->m_scale);
	}
	void ToTransform(BLAengine::Transform* transform)
	{
		m_position.LoadData(transform->m_position);
		m_rotation.LoadData(transform->m_rotation);
		m_scale.LoadData(transform->m_scale);
	}

private:
	friend class cereal::access;

	vec3serializer m_position;
	mat3serializer m_rotation;
	vec3serializer m_scale;

	template <class Archive>
	void serialize(Archive & archive)
	{
		archive
		(
			cereal::make_nvp("position", m_position),
			cereal::make_nvp("rotation", m_rotation),
			cereal::make_nvp("scale", m_scale)
		);
	}
};

class MeshRendererSerializer
{
public:

	MeshRendererSerializer() = default;

	void FromGameObject(BLAengine::MeshRenderer* meshRender)
	{
		m_triangleMeshName = meshRender->m_mesh->GetName();
	}
	void ToGameObject(BLAengine::MeshRenderer* meshRender)
	{
		//m_triangleMeshName = meshRender->m_mesh->GetName();
	}

private:
	friend class cereal::access;

	string m_triangleMeshName;
	vector<string> m_materialNames;

	template <class Archive>
	void serialize(Archive & archive)
	{
		archive
		(
			cereal::make_nvp("triangleMesh", m_triangleMeshName),
			cereal::make_nvp("material", m_materialNames)
		);
	}
};

class GameObjectSerializer
{
public:

	GameObjectSerializer() = default;

	virtual void FromGameObject(BLAengine::GameObject* gobject)
	{
		m_transform.FromTransform(gobject->m_transform);
		m_objectName = gobject->m_objectName;
	}
	virtual void ToGameObject(BLAengine::GameObject* gobject)
	{
		m_transform.ToTransform(gobject->m_transform);
		m_objectName = gobject->m_objectName;
	}

	string m_objectName;
	TransformSerializer m_transform;


	friend class cereal::access;

	template <class Archive>
	void serialize(Archive & archive)
	{
		archive
		(
			cereal::make_nvp("name", m_objectName),
			cereal::make_nvp("transform", m_transform)
		);
	}

};

class GameCharSerializer : public GameObjectSerializer
{
public:

	GameCharSerializer() = default;

	void FromGameObject(BLAengine::GameChar* gobject)
	{
		GameObjectSerializer::FromGameObject(gobject);
		m_meshRendererSerializer.FromGameObject(gobject->m_meshRenderer);
	}
	void ToGameObject(BLAengine::GameChar* gobject)
	{
		GameObjectSerializer::ToGameObject(gobject);
		m_meshRendererSerializer.ToGameObject(gobject->m_meshRenderer);
	}

private:
	friend class cereal::access;

	MeshRendererSerializer m_meshRendererSerializer;

	template <class Archive>
	void serialize(Archive & archive)
	{
		archive
		(
			cereal::make_nvp("gameObject", cereal::base_class<GameObjectSerializer>(this)),
			cereal::make_nvp("meshRenderer", m_meshRendererSerializer)
		);
	}
};

class DirectionalLightSerializer : public GameObjectSerializer
{
public:

	DirectionalLightSerializer() = default;

	void FromGameObject(BLAengine::DirectionalLight* gobject)
	{
		GameObjectSerializer::FromGameObject(gobject);
		m_lightDirection.FillData(gobject->GetDirection());
	}
	void ToGameObject(BLAengine::DirectionalLight* gobject)
	{
		GameObjectSerializer::ToGameObject(gobject);
		vec3 dir;
		m_lightDirection.LoadData(dir);
		gobject->SetDirection(dir);
	}

private:
	friend class cereal::access;

	vec3serializer m_lightDirection;

	template <class Archive>
	void serialize(Archive & archive)
	{
		archive
		(
			cereal::make_nvp("gameObject", cereal::base_class<GameObjectSerializer>(this)),
			cereal::make_nvp("lightDirection", m_lightDirection)
		);
	}
};

CEREAL_REGISTER_TYPE(GameCharSerializer)
CEREAL_REGISTER_TYPE(DirectionalLightSerializer)

CEREAL_REGISTER_POLYMORPHIC_RELATION(GameObjectSerializer, GameCharSerializer)
CEREAL_REGISTER_POLYMORPHIC_RELATION(GameObjectSerializer, DirectionalLightSerializer)

class SceneSerializer
{
public:

	SceneSerializer() = default;

	void FromScene(BLAengine::Scene* scene)
	{
		std::vector<BLAengine::GameObject*> objVec = scene->GetObjects();
		
		for (int i = 0; i < objVec.size(); i++)
		{
			BLAengine::GameObject* obj = objVec[i];

			std::shared_ptr<GameObjectSerializer> gObjSerializer;

			if (BLAengine::DirectionalLight* dirLightPtr = dynamic_cast<BLAengine::DirectionalLight*>(obj))
			{
				DirectionalLightSerializer dirLightSerial;
				dirLightSerial.FromGameObject(dirLightPtr);

				gObjSerializer = std::make_shared<DirectionalLightSerializer>();
			}

			if (BLAengine::GameChar* gameCharPtr = dynamic_cast<BLAengine::GameChar*>(obj))
			{
				GameCharSerializer dirLightSerial;
				dirLightSerial.FromGameObject(gameCharPtr);

				gObjSerializer = std::make_shared<GameCharSerializer>();
			}

			m_objectsVector.push_back(gObjSerializer);
		}
	}
	void ToGameObject(BLAengine::GameObject* gobject)
	{
		// Todo: Implement!
	}

private:
	friend class cereal::access;

	std::vector<std::shared_ptr<GameObjectSerializer>> m_objectsVector;
	std::string sceneName;

	template <class Archive>
	void serialize(Archive & archive)
	{
		archive
		(
			cereal::make_nvp("objects", m_objectsVector)
		);
	}
};