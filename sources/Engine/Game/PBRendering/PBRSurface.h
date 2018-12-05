#pragma once
#include "..\..\..\Common\StdInclude.h"
#include "..\GameComponents\ColliderComponent.h"
#include "../../Assets/Material.h"
#include "../GameComponents/GameComponent.h"

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
            virtual blaVec3 EvaluateBRDF(blaVec3 incomingRadiance, blaVec3 &normal, blaVec3 &inDir, blaVec3 &outDir) = 0;

            /*
                Returns the sample probability and outDir from input argument
            */
            virtual float SampleBRDF(blaVec3 &outDir, blaMat3 &tangentSpace, blaVec3 &inDir) = 0;
            
            virtual bool IsSpecular() = 0;
        };

        class LambertianBRDF : public BRDF
        {
        public:
            blaVec3 EvaluateBRDF(blaVec3 incomingRadiance, blaVec3 &normal, blaVec3 &inDir, blaVec3 &outDir)
            {
                return incomingRadiance / M_PI;
            }

            float SampleBRDF(blaVec3 &outDir, blaMat3&tangentSpace, blaVec3 &inDir);

            bool IsSpecular() { return false;  }
        };

        class MirrorBRDF : public BRDF
        {
        public:
            blaVec3 EvaluateBRDF(blaVec3 incomingRadiance, blaVec3 &normal, blaVec3 &inDir, blaVec3 &outDir)
            {
                blaVec3 mirrorOutDir = 2.0f * glm::dot(normal, inDir)*normal - inDir;
                return glm::dot(mirrorOutDir, outDir) > 0.95f ? incomingRadiance : blaVec3(0);
            }

            float SampleBRDF(blaVec3 &outDir, blaMat3&tangentSpace, blaVec3 &inDir);

            bool IsSpecular() { return true; }
        };
        
        BRDF* m_brdf;
    
    //private:
        blaVec3 m_color;
        blaVec3 m_emissivePower;
    };

    class BLACORE_API PBRSurface : public GameComponent
    {
    public:

        ColliderComponent* m_collider;

        PBRMaterial m_material;

        virtual void SampleSurface(blaVec3 &position, float &prob) = 0;
        virtual void SampleSurfaceWithNormal(blaVec3 &position, blaVec3 &normal, float &prob) = 0;

        virtual float GetSurfaceArea() = 0;

        PBRSurface();
        ~PBRSurface(void);

        void Update();
    };

    class BLACORE_API PBRMesh : public PBRSurface
    {
    public:
        void SampleSurface(blaVec3 &position, float &prob);
        void SampleSurfaceWithNormal(blaVec3 &position, blaVec3 &normal, float &prob);
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
        void SampleSurface(blaVec3 &position, float &prob);
        void SampleSurfaceWithNormal(blaVec3 &position, blaVec3 &normal, float &prob);
        float GetSurfaceArea();

        PBRSphere(float radius);
        ~PBRSphere(void);
    private:

        float m_surfaceArea = -1;
    };
}