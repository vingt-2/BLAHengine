#pragma once
#include "..\..\Common\StdInclude.h"
#include "GameComponents/GameComponent.h"
#include "GameComponents/Transform.h"

namespace BLAengine
{
	class BLACORE_API GameObject
	{
	public:

		void Update();

		void SetParent(GameObject* parent) { m_parent = parent; }
		GameObject* GetParent() { return m_parent; }

		const Transform& GetTransform();
		void SetTransform(const Transform& transform);

		GameObject(string name);
		~GameObject(void);

		void SetName(std::string name) { m_objectName = name; }
		std::string GetName() { return m_objectName; }

		void AddComponent(GameComponent* component);

		vector<GameComponent*> GetAllComponents();

		template<class ComponentType>
		ComponentType* GetComponent();

		template<class ComponentType>
		ComponentType* GetComponentInChildren();

		template<class ComponentType>
		vector<ComponentType*> GetComponents();

		template<class ComponentType>
		vector<ComponentType*> GetComponentsInChildren();

	private:

		string m_objectName;
		GameObject* m_parent;
		Transform* m_transform;

		vector<GameComponent*> m_componentVector;

		void UpdateTransform();
	};

	inline vector<GameComponent*> GameObject::GetAllComponents()
	{
		return m_componentVector;
	}

	template<class ComponentType>
	inline ComponentType* GameObject::GetComponent()
	{
		for (int i = 0; i < m_componentVector.size(); i++)
		{
			GameComponent* gComp = m_componentVector[i];
			if (ComponentType* comp = dynamic_cast<ComponentType*>(gComp))
			{
				return comp;
			}
		}
		return nullptr;
	}

	template<class ComponentType>
	inline vector<ComponentType*> GameObject::GetComponents()
	{
		vector<ComponentType*> comps;
		for (int i = 0; i < m_componentVector.size(); i++)
		{
			GameComponent* gComp = m_componentVector[i];
			if (ComponentType* comp = dynamic_cast<ComponentType*>(gComp))
			{
				comps.push_back(comp);
			}
		}
		return comps;
	}
}
