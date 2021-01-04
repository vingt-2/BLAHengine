// BLAEngine Copyright (C) 2016-2020 Vincent Petrella. All rights reserved.

#pragma once
#include "PBRSurfaceComponent.h"
#include "PBRPhotonMap.h"
#include "Core/BehaviorComponent.h"

namespace BLA
{
    class MeshRendererComponent;
    class PBRRenderer
    {
    public:
        virtual blaVector<blaVec3> Render(blaPosQuat cameraTransform, glm::vec2 resolution, bool inParallel) = 0;

        static blaPair<MeshRendererComponent*, ColliderComponent::CollisionContact> IntersectWithScene(Ray r, blaVector<MeshRendererComponent*> &objects);
    };

    class PBRExplicitPathTracer : public PBRRenderer
    {
    public:
        PBRExplicitPathTracer(blaVector<MeshRendererComponent*> sceneObjects)
        {
            m_sceneObjects = sceneObjects;
        }

        blaVector<blaVec3> Render(blaPosQuat cameraTransform, glm::vec2 resolution, bool inParallel);
    private:

        blaVec3 PathTraceShade(Ray r, int depth);

        blaVector<MeshRendererComponent*> m_sceneObjects;
        blaVector<MeshRendererComponent*> m_lightObjects;
    };

    class PBRPhotonMapping : public PBRRenderer
    {
    public:
        PBRPhotonMapping(blaVector<MeshRendererComponent*> sceneObjects) :
            m_photonMap(PhotonMap(0)),
            m_volumetricPhotonMap(PhotonMap(0))
        {
            m_sceneObjects = sceneObjects;
            m_photonsShot = 0;
            m_volumeExtinctionCoeff = 0.05f;
            m_volumeScatterCoeff = 0.05f;
        }

        blaVector<blaVec3> Render(blaPosQuat cameraTransform, glm::vec2 resolution, bool inParallel);
    private:
        void BuildPhotonMap(bool inParallel, blaU32 numberOfPhotons);
        Ray GeneratePhoton(MeshRendererComponent* surface, float &outEmissionProb);
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
        blaVector<MeshRendererComponent*> m_sceneObjects;
        blaVector<MeshRendererComponent*> m_lightObjects;
    };

    BeginBehaviorDeclaration(BLADemos, PBRCamera)
        blaVector<blaVec3> m_renderedImage;
	    PBRRenderer* m_renderer;

	    void Render();

	    bool m_shouldRender = false;
		
	private:

	    bool WriteImageToFile(blaString filepath, int w, int h);
	EndBehaviorDeclaration()
}