#pragma once
#include "PBRSurfaceComponent.h"
#include "PBRPhotonMap.h"

namespace BLAengine
{
    class PBRRenderer
    {
    public:
        virtual vector<blaVec3> Render(ObjectTransform cameraTransform, glm::vec2 resolution, bool inParallel) = 0;
        
        static std::pair<PBRSurfaceComponent*, ColliderComponent::CollisionContact> IntersectWithScene(Ray r, vector<PBRSurfaceComponent*> &objects);
    };

    class PBRExplicitPathTracer : public PBRRenderer
    {
    public:
        PBRExplicitPathTracer(vector<PBRSurfaceComponent*> sceneObjects)
        {
            m_sceneObjects = sceneObjects;
        }

        vector<blaVec3> Render(ObjectTransform cameraTransform, glm::vec2 resolution, bool inParallel);
    private:

        blaVec3 PathTraceShade(Ray r, int depth);

        vector<PBRSurfaceComponent*> m_sceneObjects;
        vector<PBRSurfaceComponent*> m_lightObjects;
    };

    class PBRPhotonMapping : public PBRRenderer
    {
    public:
        PBRPhotonMapping(vector<PBRSurfaceComponent*> sceneObjects):
            m_photonMap(PhotonMap(0)),
            m_volumetricPhotonMap(PhotonMap(0))
        {
            m_sceneObjects = sceneObjects;
            m_photonsShot = 0;
            m_volumeExtinctionCoeff = 0.05f;
            m_volumeScatterCoeff = 0.05f;
        }

        vector<blaVec3> Render(ObjectTransform cameraTransform, glm::vec2 resolution, bool inParallel);
    private:
        void BuildPhotonMap(bool inParallel, blaU32 numberOfPhotons);
        Ray GeneratePhoton(PBRSurfaceComponent* surface, float &outEmissionProb);
        void TracePhoton(Ray incidentRay, blaVec3 photonPower,int bounce);

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
        vector<PBRSurfaceComponent*> m_sceneObjects;
        vector<PBRSurfaceComponent*> m_lightObjects;
    };

    class BLACORE_API PBRCamera : public GameComponent
    {
    public:

        PBRCamera(GameObjectReference parentObject): GameComponent(parentObject) {};
        ~PBRCamera(){};

        vector<blaVec3> m_renderedImage;
        PBRRenderer* m_renderer;

        void AddObject(PBRSurfaceComponent* mesh)
        {
            m_sceneObjects.push_back(mesh);
        }

        void Render();

        bool m_shouldRender = false;

        void Update() {};
    private:
        vector<PBRSurfaceComponent*> m_sceneObjects;

        bool WriteImageToFile(string filepath, int w, int h);
    };
}