#pragma once
#include "..\..\..\Common\StdInclude.h"
#include "..\..\Assets\PolygonalMesh.h"
#include "../GameAlgebra/Ray.h"

// Collision detection Library !
#include "../../Ext/libcoldet/coldet.h"

#include "GameComponent.h"

namespace BLAengine
{
	class BLACORE_API Collider : public GameComponent
	{
	public:
		struct RayCollision
		{
			bool m_isValid;
			vec3 m_colPositionL;
			vec3 m_colPositionW;
			vec3 m_colNormalW;
			vec3 m_colNormalL;
			float m_t;
		};

		Collider() {};
		~Collider() {};

		virtual RayCollision CollideWithRay(Ray &ray) = 0;
		virtual RayCollision CollideWithCollider(Collider& collider) = 0;
		virtual float GetBoundingRadius() = 0;
	};

	class BLACORE_API MeshCollider : public Collider
	{
	public:
		MeshCollider(TriangleMesh* mesh);
		~MeshCollider();

		void Update() {};

		RayCollision CollideWithRay(Ray &ray);
		RayCollision CollideWithCollider(Collider& collider);
		float GetBoundingRadius() { return m_boundingRadius; }

		CollisionModel3D* m_collisionMesh;
		vector<uint32_t>* m_vertPosIndices;
		vector<uint32_t>* m_vertNormalIndices;
		vector<vec3>* m_triVertices;
		vector<vec3>* m_triNormals;
	private:
		float m_boundingRadius;

		void GenerateBoundingRadius();
		void GenerateCollisionModel();
	};

	class BLACORE_API SphereCollider : public Collider
	{
	public:
		SphereCollider(float size) 
		{
			m_boundingRadius = size;
		}

		~SphereCollider() {};

		void Update() {};

		float GetBoundingRadius() { return m_boundingRadius; }

		RayCollision CollideWithRay(Ray &ray);
		RayCollision CollideWithCollider(Collider& collider);
	private:
		float m_boundingRadius;
	};
}
