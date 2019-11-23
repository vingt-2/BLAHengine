#pragma once
#include "PBRSurfaceComponent.h"
#include "PBRPhotonMap.h"

namespace BLAengine
{
    class PBRRenderer
    {
    public:
        virtual blaVector<blaVec3> Render(ObjectTransform cameraTransform, glm::vec2 resolution, bool inParallel) = 0;

        static blaPair<PBRSurfaceComponent*, ColliderComponent::CollisionContact> IntersectWithScene(Ray r, blaVector<PBRSurfaceComponent*> &objects);
    };

    class PBRExplicitPathTracer : public PBRRenderer
    {
    public:
        PBRExplicitPathTracer(blaVector<PBRSurfaceComponent*> sceneObjects)
        {
            m_sceneObjects = sceneObjects;
        }

        blaVector<blaVec3> Render(ObjectTransform cameraTransform, glm::vec2 resolution, bool inParallel);
    private:

        blaVec3 PathTraceShade(Ray r, int depth);

        blaVector<PBRSurfaceComponent*> m_sceneObjects;
        blaVector<PBRSurfaceComponent*> m_lightObjects;
    };

    class PBRPhotonMapping : public PBRRenderer
    {
    public:
        PBRPhotonMapping(blaVector<PBRSurfaceComponent*> sceneObjects) :
            m_photonMap(PhotonMap(0)),
            m_volumetricPhotonMap(PhotonMap(0))
        {
            m_sceneObjects = sceneObjects;
            m_photonsShot = 0;
            m_volumeExtinctionCoeff = 0.05f;
            m_volumeScatterCoeff = 0.05f;
        }

        blaVector<blaVec3> Render(ObjectTransform cameraTransform, glm::vec2 resolution, bool inParallel);
    private:
        void BuildPhotonMap(bool inParallel, blaU32 numberOfPhotons);
        Ray GeneratePhoton(PBRSurfaceComponent* surface, float &outEmissionProb);
        void TracePhoton(Ray incidentRay, blaVec3 photonPower, int bounce);

        blaVec3 GatherSurfaceDensity(Ray incidentRay);
        blaVec3 GatherVolumetricDensity(blaVec3 pos, blaVec3 incomingDir);
        blaVec3 MarchIndirectVolumetric(Ray ray, float endOfRay, int numberOfSamples);
        blaVec3 MarchDirectEquiAngular(Ray ray, float endOfRay, int numberOfSamples);
        blaVec3 Shade(Ray incidentRay);

        PhotonMap m_photonMap;
        PhotonMap m_volumetricPhotonMap;

        float m_volumeExtinctionCoeff;
        float m_volumeScatterCoeff;
        float ComputeTransmittance(float distance)
        {
            return expf(-m_volumeExtinctionCoeff * distance);
        }

        int m_photonsShot;
        blaVector<PBRSurfaceComponent*> m_sceneObjects;
        blaVector<PBRSurfaceComponent*> m_lightObjects;
    };

    class BLACORE_API PBRCamera : public GameComponent
    {
    public:

        PBRCamera(GameObjectReference parentObject) : GameComponent(parentObject) {};
        ~PBRCamera() {};

        blaVector<blaVec3> m_renderedImage;
        PBRRenderer* m_renderer;

        void AddObject(PBRSurfaceComponent* mesh)
        {
            m_sceneObjects.push_back(mesh);
        }

        void Render();

        bool m_shouldRender = false;

        void Update() {};
    private:
        blaVector<PBRSurfaceComponent*> m_sceneObjects;

        bool WriteImageToFile(blaString filepath, int w, int h);
    };
}