#pragma once
#include "..\..\..\Common\StdInclude.h"
#include "..\GameComponents\Collider.h"
#include "../../Assets/Material.h"
#include "../GameComponents/Transform.h"
#include "../GameComponents/GameComponent.h"

#define M_PI 3.14159265359f

namespace BLAengine
{
    class BLACORE_API PBRMaterial
    {
    public:

        class BRDF
        {
        public:
            virtual ~BRDF() {}
            
            /*
                Returns outgoing Radiance as a function of incoming and inDir, outDir
            */
            virtual vec3 EvaluateBRDF(vec3 incomingRadiance, vec3 &normal, vec3 &inDir, vec3 &outDir) = 0;

            /*
                Returns the sample probability and outDir from input argument
            */
            virtual float SampleBRDF(vec3 &outDir, mat3 &tangentSpace, vec3 &inDir) = 0;
            
            virtual bool IsSpecular() = 0;
        };

        class LambertianBRDF : public BRDF
        {
        public:
            vec3 EvaluateBRDF(vec3 incomingRadiance, vec3 &normal, vec3 &inDir, vec3 &outDir)
            {
                return incomingRadiance / M_PI;
            }

            float SampleBRDF(vec3 &outDir, mat3&tangentSpace, vec3 &inDir);

            bool IsSpecular() { return false;  }
        };

        class MirrorBRDF : public BRDF
        {
        public:
            vec3 EvaluateBRDF(vec3 incomingRadiance, vec3 &normal, vec3 &inDir, vec3 &outDir)
            {
                vec3 mirrorOutDir = 2.0f * dot(normal, inDir)*normal - inDir;
                return dot(mirrorOutDir, outDir) > 0.95f ? incomingRadiance : vec3(0);
            }

            float SampleBRDF(vec3 &outDir, mat3&tangentSpace, vec3 &inDir);

            bool IsSpecular() { return true; }
        };
        
        BRDF* m_brdf;
    
    //private:
        vec3 m_color;
        vec3 m_emissivePower;
    };

    class BLACORE_API PBRSurface : public GameComponent
    {
    public:

        Collider* m_collider;

        PBRMaterial m_material;

        virtual void SampleSurface(vec3 &position, float &prob) = 0;
        virtual void SampleSurfaceWithNormal(vec3 &position, vec3 &normal, float &prob) = 0;

        virtual float GetSurfaceArea() = 0;

        PBRSurface();
        ~PBRSurface(void);

        void Update();
    };

    class BLACORE_API PBRMesh : public PBRSurface
    {
    public:
        void SampleSurface(vec3 &position, float &prob);
        void SampleSurfaceWithNormal(vec3 &position, vec3 &normal, float &prob);
        float GetSurfaceArea();

        PBRMesh(TriangleMesh* mesh);
        ~PBRMesh(void);
    private:

        void ComputeSurfaceArea(MeshCollider* mesh);
        float m_surfaceArea = -1;
    };

    class BLACORE_API PBRSphere : public PBRSurface
    {
    public:
        void SampleSurface(vec3 &position, float &prob);
        void SampleSurfaceWithNormal(vec3 &position, vec3 &normal, float &prob);
        float GetSurfaceArea();

        PBRSphere(float radius);
        ~PBRSphere(void);
    private:

        float m_surfaceArea = -1;
    };
}