#pragma once
#include "PBRSurface.h"
#include "PBRPhotonMap.h"

namespace BLAengine
{
    class PBRRenderer
    {
    public:
        virtual vector<vec3> Render(Transform cameraTransform, vec2 resolution, bool inParallel) = 0;
    };

    class PBRExplicitPathTracer : public PBRRenderer
    {
    public:
        PBRExplicitPathTracer(vector<PBRSurface*> sceneObjects)
        {
            m_sceneObjects = sceneObjects;
        }

        vector<vec3> Render(Transform cameraTransform, vec2 resolution, bool inParallel);
    private:
        vec3 PathTraceShade(Ray r, int depth);

        vector<PBRSurface*> m_sceneObjects;
        vector<PBRSurface*> m_lightObjects;
    };

    class PBRPhotonMapping : public PBRRenderer
    {
    public:
        PBRPhotonMapping(vector<PBRSurface*> sceneObjects):
            m_photonMap(PhotonMap(0)),
            m_volumetricPhotonMap(PhotonMap(0))
        {
            m_sceneObjects = sceneObjects;
            m_photonsShot = 0;
            m_volumeExtinctionCoeff = 0.05;
            m_volumeScatterCoeff = 0.05;
        }

        vector<vec3> Render(Transform cameraTransform, vec2 resolution, bool inParallel);
    private:
        void BuildPhotonMap(bool inParallel, uint numberOfPhotons);
        Ray GeneratePhoton(PBRSurface* surface, float &outEmissionProb);
        void TracePhoton(Ray incidentRay, vec3 photonPower,int bounce);

        vec3 GatherSurfaceDensity(Ray incidentRay);
        vec3 GatherVolumetricDensity(vec3 pos, vec3 incomingDir);
        vec3 MarchIndirectVolumetric(Ray ray, float endOfRay, int numberOfSamples);
        vec3 MarchDirectEquiAngular(Ray ray, float endOfRay, int numberOfSamples);
        vec3 Shade(Ray incidentRay);

        PhotonMap m_photonMap;
        PhotonMap m_volumetricPhotonMap;

        float m_volumeExtinctionCoeff;
        float m_volumeScatterCoeff;
        float ComputeTransmittance(float distance)
        {
            return expf(-m_volumeExtinctionCoeff * distance);
        }

        int m_photonsShot;
        vector<PBRSurface*> m_sceneObjects;
        vector<PBRSurface*> m_lightObjects;
    };

    static std::pair<PBRSurface*, Collider::RayCollision> IntersectWithScene(Ray r, vector<PBRSurface*> &objects);

    class BLACORE_API PBRCamera : public GameComponent
    {
    public:
        vector<vec3> m_renderedImage;
        PBRRenderer* m_renderer;

        void AddObject(PBRSurface* mesh)
        {
            m_sceneObjects.push_back(mesh);
        }

        void Render();

        bool m_shouldRender = false;

        void Update() {};
    private:
        vector<PBRSurface*> m_sceneObjects;

        bool WriteImageToFile(string filepath, int w, int h);
    };
}