#include "PBRRenderer.h"
#include <random>
#include <ppl.h>
#include <concurrent_vector.h>

using namespace BLAengine;
using namespace concurrency;

inline float ClampOne(float x) { return x < 0 ? 0 : x > 1 ? 1 : x; }
inline int ToDisplayValue(float x) { return int(pow(ClampOne(x), 1.0 / 2.2) * 255 + .5); }
inline mat3 GetTangentSpace(vec3 normal)
{
	vec3 trash(sqrt(3) / 3, sqrt(3) / 3, sqrt(3) / 3);
	vec3 orthYU = cross(normal, trash);
	vec3 orthY = normalize(orthYU);
	if (dot(orthY, orthYU) < 0.1f)
	{
		vec3 trash2(1, 0, 0);
		orthY = normalize(cross(normal, trash2));
	}
	vec3 orthX = cross(normal, orthY);

	return mat3(normal, orthX, orthY);
}

std::random_device rd2;
std::mt19937 dgen(rd2());
std::uniform_real_distribution<float> zeroToOneDist(0, 1);

void BLAengine::PBRCamera::Render()
{
	vec2 resolution(1920,1080);
	if (m_renderer == nullptr)
	{
		m_renderer = new PBRPhotonMapping(m_sceneObjects);
	}
	m_renderedImage = m_renderer->Render(GetObjectTransform(), resolution, PARALLEL_RENDER);
	WriteImageToFile("image.ppm", resolution.x, resolution.y);
	m_shouldRender = false;
}

bool BLAengine::PBRCamera::WriteImageToFile(string filepath, int w, int h)
{
	// hint: Google the PPM image format
	FILE *f = fopen(filepath.data(), "w");
	if (f == nullptr)
	{
		return false;
	}
	fprintf(f, "P3\n%d %d\n%d\n", w, h, 255);
	for (int p = 0; p < w * h; p++)
	{
		fprintf(f, "%d %d %d ", ToDisplayValue(m_renderedImage[p].x), ToDisplayValue(m_renderedImage[p].y), ToDisplayValue(m_renderedImage[p].z));
	}
	fclose(f);
	return true;
}

std::pair<PBRSurface*, Collider::RayCollision> BLAengine::IntersectWithScene(Ray ray, vector<PBRSurface*> &objects)
{
	float minDistance = INFINITY;
	PBRSurface* pickedObject = nullptr;
	Collider::RayCollision closestContact;
	for (auto obj : objects)
	{
		Collider* collider = obj->m_collider;

		if (collider == nullptr)
			continue;

		Collider::RayCollision contactPoint = collider->CollideWithRay(ray);

		if (!contactPoint.m_isValid)
			continue;

		float distance = dot(contactPoint.m_colPositionW - ray.m_origin, ray.m_direction);

		if (distance > 0 && distance < minDistance)
		{
			minDistance = distance;
			pickedObject = obj;
			closestContact = contactPoint;
		}
	}

	return std::pair<PBRSurface*, Collider::RayCollision>(pickedObject, closestContact);
}

vector<vec3> BLAengine::PBRExplicitPathTracer::Render(Transform cameraTransform, vec2 resolution, bool inParallel)
{
	for (PBRSurface* surface : m_sceneObjects)
	{
		if (length(surface->m_material.m_emissivePower) > 0.0f)
		{
			m_lightObjects.push_back(surface);
		}
	}

	int w = resolution.x, h = resolution.y;

	float horizontalFov = 100;

	float fovFactor = tan((125 / 180.0f) * M_PI);

	vec3 cameraForward = cameraTransform.LocalDirectionToWorld(vec3(0, 0, -1));

	vec3 cx = vec3(w * fovFactor / h, 0.f, 0.f); // hint : tan( 30 / 180.0 * M_PI ) == 0.57735
	vec3 cy = cross(cx, cameraForward) * fovFactor * (h / (float)w);


	vec3* concurrent_img_vector = (vec3*)malloc(sizeof(vec3)*h*w);

	vector<vec3> renderedImage;
	renderedImage.reserve(w*h);

	if (inParallel)
	{
		size_t ht = h;
		int progress = 0;
		parallel_for(size_t(0), ht, [&](size_t yp)
		{
			int y = (int)yp;
			fprintf(stderr, "\r%5.2f%%", 100.*progress / (h - 1));

			for (int x = 0; x < w; x++)
			{
				int idx = (h - y - 1) * w + x;
				vec3 pixelValue(0);
				vec3 cameraRayDir = cx * (x / (float)w - 0.5f) + cy * (y / (float)h - 0.5f) + cameraForward;
				Ray ray = Ray(cameraTransform.m_position, cameraRayDir, INFINITY);
				pixelValue = this->PathTraceShade(ray, 0);
				concurrent_img_vector[idx] = vec3(ClampOne(pixelValue.x), ClampOne(pixelValue.y), ClampOne(pixelValue.z));
			}
			progress++;
		});
	}
	else
	{
		for (int y = 0; y < h; y++)
		{
			fprintf(stderr, "\r%5.2f%%", 100.*y / (h - 1));
			for (int x = 0; x < w; x++) {
				int idx = (h - y - 1) * w + x;
				vec3 pixelValue = vec3(0);
				vec3 cameraRayDir = cx * (x / (float)w - 0.5f) + cy * (y / (float)h - 0.5f) + cameraForward;
				Ray ray = Ray(cameraTransform.m_position, cameraRayDir, INFINITY);
				pixelValue = this->PathTraceShade(ray, 0);
				concurrent_img_vector[idx] = vec3(ClampOne(pixelValue.x), ClampOne(pixelValue.y), ClampOne(pixelValue.z));
			}
		}
	}

	fprintf(stderr, "\n");

	for (int i = 0; i <h*w; i++)
	{
		renderedImage[i] = concurrent_img_vector[i];
	}
	free(concurrent_img_vector);

	return renderedImage;
}

vec3 BLAengine::PBRExplicitPathTracer::PathTraceShade(Ray incidentRay, int depth)
{
	if (depth > 1)
	{
		return vec3(0);
	}

	auto p = IntersectWithScene(incidentRay, m_sceneObjects);

	if (p.first == nullptr)
	{
		return vec3(0);
	}

	Collider::RayCollision shadeRayCollision = p.second;

	PBRMaterial* m = &(p.first->m_material);

	if (length(m->m_emissivePower) > 0.0f)
	{
		if (depth < 1)
			return m->m_emissivePower / p.first->GetSurfaceArea();
		else
			return vec3(0);
	}

	vec3 lightRadiance(0);
	for (PBRSurface* lightSurface : m_lightObjects)
	{
		vec3 montecarloRadiance = vec3(0, 0, 0);
		float mc_samples = 20;
		for (int mc = 0; mc < mc_samples; mc++)
		{

			vec3 samplePoint;
			float samplePDF;

			lightSurface->SampleSurface(samplePoint, samplePDF);

			vec3 directionToLightSample = samplePoint - shadeRayCollision.m_colPositionW;
			float sqrDistToSamplePoint = dot(directionToLightSample, directionToLightSample);
			directionToLightSample /= sqrt(sqrDistToSamplePoint);

			Ray toLightRay(shadeRayCollision.m_colPositionW + 0.001f * shadeRayCollision.m_colNormalW, directionToLightSample, INFINITY);

			auto toLightRayCollision = IntersectWithScene(toLightRay, m_sceneObjects);

			vec3 lightIrradiance(0);
			if (toLightRayCollision.first == lightSurface)
			{
				lightIrradiance = lightSurface->m_material.m_emissivePower / lightSurface->GetSurfaceArea();
			}
			else
			{
				continue;
			}
			// The diffuse Brdf...
			vec3 evaluatedBRDF = m->m_brdf->EvaluateBRDF(lightIrradiance, shadeRayCollision.m_colNormalW, directionToLightSample, -incidentRay.m_direction);

			// The geometric term for the change of coordinates (from directions to point on surfaces...)
			float geometricTerm = abs(dot(toLightRayCollision.second.m_colNormalW, directionToLightSample)) / sqrDistToSamplePoint;
			// You now the drill
			vec3 sampleContribution = evaluatedBRDF * geometricTerm * dot(shadeRayCollision.m_colNormalW, directionToLightSample);

			montecarloRadiance += sampleContribution * (1.0f / samplePDF);
		}
		lightRadiance += montecarloRadiance * (1.0f / mc_samples);
	}

	vec3 n = shadeRayCollision.m_colNormalW;

	mat3 tangentSpace = GetTangentSpace(n);

	std::uniform_real_distribution<> randDist(-1, 1);

	vec3 indirectRadiance(0);
	vec3 mcIndirectRadiance(0);
	float mc_indirectSamples = 10;
	for (int i = 0; i < mc_indirectSamples; i++)
	{
		vec3 outDir;
		float samplePdf = m->m_brdf->SampleBRDF(outDir, tangentSpace, incidentRay.m_direction);

		Ray outRay(shadeRayCollision.m_colPositionW + 0.001f*n, outDir, INFINITY);

		vec3 indirectIrradiance = this->PathTraceShade(outRay, depth + 1);

		vec3 sampleContribution = m->m_brdf->EvaluateBRDF(indirectIrradiance, n, incidentRay.m_direction, outDir);

		mcIndirectRadiance = mcIndirectRadiance + sampleContribution;// *(1.0f / samplePdf);
	}
	if (mc_indirectSamples)
		indirectRadiance = mcIndirectRadiance * (1.0f / mc_indirectSamples);

	vec3 sampleRadiance = lightRadiance + indirectRadiance;

	return m->m_color * sampleRadiance;
}

void BLAengine::PBRPhotonMapping::BuildPhotonMap(bool inParallel, uint numberOfPhotons)
{
	std::cout << "Building Photon Map:\n";
	m_photonMap = PhotonMap(numberOfPhotons);

	std::uniform_real_distribution<float> uniformDist(0, 1);

	float totalPower = 0;
	for (int i = 0; i < m_lightObjects.size(); i++)
	{
		PBRSurface* lightSurface = m_lightObjects[i];

		totalPower += length(lightSurface->m_material.m_emissivePower);
	}

	std::vector<float> lightSamplingPDF;
	for (int i = 0; i < m_lightObjects.size(); i++)
	{
		PBRSurface* lightSurface = m_lightObjects[i];
		lightSamplingPDF.push_back(length(lightSurface->m_material.m_emissivePower) / totalPower);
	}

	if (!inParallel)
	{
		for (int photonIndex = 0; photonIndex < numberOfPhotons; photonIndex++)
		{
			fprintf(stderr, "\r%5.2f%%", 100.* (photonIndex / (float)numberOfPhotons));
			for (int i = 0; i < m_lightObjects.size(); i++)
			{
				if (zeroToOneDist(dgen) >= lightSamplingPDF[i])
					continue;

				PBRSurface* lightSurface = m_lightObjects[i];

				float emissionProb;
				Ray ray = GeneratePhoton(lightSurface, emissionProb);

				TracePhoton(ray, lightSurface->m_material.m_emissivePower / lightSamplingPDF[i], 0);
				break;
			}
			m_photonsShot++;
		}
	}
	else
	{
		size_t ht = numberOfPhotons;
		int progress = 0;
		parallel_for(size_t(0), ht, [&](size_t yp)
		{
			fprintf(stderr, "\r%5.2f%%", 100.*progress / (float)numberOfPhotons);
			for (int i = 0; i < m_lightObjects.size(); i++)
			{
				if (zeroToOneDist(dgen) >= lightSamplingPDF[i])
					continue;

				PBRSurface* lightSurface = m_lightObjects[i];

				float emissionProb;
				Ray ray = GeneratePhoton(lightSurface, emissionProb);

				TracePhoton(ray, lightSurface->m_material.m_emissivePower / lightSamplingPDF[i], 0);
				break;
			}
			m_photonsShot++;
			progress++;
		});
	}

	m_photonMap.BuildKDTree();
	m_volumetricPhotonMap.BuildKDTree();
}

Ray BLAengine::PBRPhotonMapping::GeneratePhoton(PBRSurface* lightSurface, float &outEmissionProb)
{
	vec3 samplePos, sampleNormal;
	float samplePDF;

	lightSurface->SampleSurfaceWithNormal(samplePos, sampleNormal, samplePDF);

	mat3 tangentSpace = GetTangentSpace(sampleNormal);

	// Generate uniformly distributed spherical coordinates
	double randtheta = 2 * M_PI * zeroToOneDist(dgen);
	double randphi = acos(2 * zeroToOneDist(dgen) - 1);

	// Get the World direction of our sample
	vec3 tangentDirection = vec3(cosf(randphi), cosf(randtheta)*sinf(randphi), sinf(randtheta)*sinf(randphi));
	vec3 direction = tangentSpace * tangentDirection;

	float directionPdf = 1.5707963f; // 1 / 2Pi

	outEmissionProb = samplePDF * directionPdf;

	return Ray(samplePos + 0.001f * sampleNormal, direction, INFINITY);
}

void BLAengine::PBRPhotonMapping::TracePhoton(Ray incidentRay, vec3 photonPower, int bounce)
{
	if (bounce > 10)
		return;

	auto p = IntersectWithScene(incidentRay, m_sceneObjects);

	PBRSurface* surface = p.first;
	Collider::RayCollision colHit = p.second;

	if (surface == nullptr || length(surface->m_material.m_emissivePower) > 0)
		return;


	if (bounce > -1 && m_volumeExtinctionCoeff > 0)
	{
		float nextEvent = -logf(zeroToOneDist(dgen)) / m_volumeExtinctionCoeff;

		if (nextEvent < colHit.m_t)
		{
			vec3 hitPosition = incidentRay.m_origin + nextEvent * incidentRay.m_direction;
			Photon p(hitPosition, incidentRay.m_direction, photonPower);
			m_volumetricPhotonMap.AddPhoton(p);

			if (zeroToOneDist(dgen) < (m_volumeScatterCoeff / m_volumeExtinctionCoeff))
			{
				// Generate uniformly distributed spherical coordinates
				float randtheta = 2 * M_PI * zeroToOneDist(dgen);
				float randphi = acos(2 * zeroToOneDist(dgen) - 1);

				// Convert them into a World directions
				vec3 direction(cosf(randphi), cosf(randtheta)*sinf(randphi), sinf(randtheta)*sinf(randphi));
				Ray outScatter(hitPosition, direction, INFINITY);
				TracePhoton(outScatter, photonPower, bounce + 1);
			}

			return;
		}
	}

	PBRMaterial* material = &(surface->m_material);
	float absorptionProb = 1 - (material->m_color[0] + material->m_color[1] + material->m_color[2]) / 3.0f;

	//Arriving a the surface... Russian roulette:
	bool shouldAbsorb = zeroToOneDist(dgen) < absorptionProb;

	if (!material->m_brdf->IsSpecular())
	{
		Photon p(colHit.m_colPositionW, incidentRay.m_direction, material->m_color * photonPower);
		m_photonMap.AddPhoton(p);
	}

	if (!shouldAbsorb)
	{
		mat3 tangentSpace = GetTangentSpace(colHit.m_colNormalW);
		vec3 outDir;
		float outSamplePDF = material->m_brdf->SampleBRDF(outDir, tangentSpace, incidentRay.m_direction);

		TracePhoton(Ray(colHit.m_colPositionW + 0.001f * colHit.m_colNormalW, outDir, INFINITY), material->m_color * photonPower, bounce + 1);
	}
}

vec3 BLAengine::PBRPhotonMapping::GatherSurfaceDensity(Ray incidentRay)
{
	auto p = IntersectWithScene(incidentRay, m_sceneObjects);

	PBRMaterial* m = &(p.first->m_material);

	if (p.first == nullptr || length(m->m_emissivePower) > 0.0f)
	{
		return vec3(0);
	}

	vec3 hitPosition = p.second.m_colPositionW;

	float gatherAreaRadius;
	vector<Photon*> photons = m_photonMap.GetPhotons(hitPosition, 100, gatherAreaRadius);

	vec3 powerDensity(0);
	for (Photon* photon : photons)
	{
		powerDensity += m->m_brdf->EvaluateBRDF(photon->m_power, p.second.m_colNormalW, photon->m_dir, -incidentRay.m_direction);
	}

	vec3 toPoint = incidentRay.m_origin - hitPosition;
	float sqrDistance = dot(toPoint, toPoint);

	if (sqrDistance < 16 * gatherAreaRadius * gatherAreaRadius)
	{
		return vec3(0);
	}
	powerDensity *= ComputeTransmittance(p.second.m_t);
	powerDensity /= (M_PI * gatherAreaRadius * gatherAreaRadius * -1.0f * dot(incidentRay.m_direction, p.second.m_colNormalW) * sqrDistance * (float)m_photonsShot);

	return length(powerDensity) > sqrt(1) ? normalize(powerDensity) : powerDensity;
}

vec3 BLAengine::PBRPhotonMapping::GatherVolumetricDensity(vec3 pos, vec3 incomingDir)
{
	float gatherAreaRadius;
	vector<Photon*> photons = m_volumetricPhotonMap.GetPhotons(pos, 100, gatherAreaRadius);

	vec3 powerDensity(0);
	for (Photon* photon : photons)
	{
		if (photon != nullptr)
			powerDensity += photon->m_power / (4 * M_PI); // isotropic phase function
	}

	return powerDensity / ((float)m_photonsShot * (4.0f / 3.0f) * M_PI * gatherAreaRadius * gatherAreaRadius * gatherAreaRadius);
}

vec3 BLAengine::PBRPhotonMapping::MarchIndirectVolumetric(Ray ray, float endOfRay, int numberOfSamples)
{
	vec3 indirectRadiance(0);
	float dt = endOfRay / (float)numberOfSamples;
	float marchedDistance = dt;
	while (marchedDistance < endOfRay && m_volumeExtinctionCoeff > 0)
	{
		indirectRadiance += dt * ComputeTransmittance(dt) * m_volumeScatterCoeff * GatherVolumetricDensity(ray.m_origin + marchedDistance * ray.m_direction, ray.m_direction);
		marchedDistance += dt;
	}

	return indirectRadiance;
}

vec3 BLAengine::PBRPhotonMapping::MarchDirectEquiAngular(Ray ray, float endOfRay, int numberOfSamples)
{
	float dt = endOfRay / (float) numberOfSamples;
	float marchedDistance = dt;
	vec3 indirectRadiance(0);
	while (marchedDistance < endOfRay && m_volumeExtinctionCoeff > 0)
	{
		vec3 lightContribution(0);
		for (PBRSurface* lightSurface : m_lightObjects)
		{
			vec3 montecarloRadiance = vec3(0);
			float MCSamples = 10;
			for (int mc = 0; mc < MCSamples; mc++)
			{

				vec3 samplePoint;
				float samplePDF;

				lightSurface->SampleSurface(samplePoint, samplePDF);

				vec3 directionToLightSample = samplePoint - ray.m_origin + marchedDistance * ray.m_direction;
				float sqrDistToSamplePoint = dot(directionToLightSample, directionToLightSample);
				directionToLightSample /= sqrt(sqrDistToSamplePoint);

				Ray toLightRay(ray.m_origin + marchedDistance * ray.m_direction, directionToLightSample, INFINITY);

				auto toLightRayCollision = IntersectWithScene(toLightRay, m_sceneObjects);

				vec3 lightIrradiance(0);
				if (toLightRayCollision.first == lightSurface)
				{
					lightIrradiance = lightSurface->m_material.m_emissivePower / lightSurface->GetSurfaceArea();
				}
				else
				{
					continue;
				}

				float cont = ComputeTransmittance(toLightRayCollision.second.m_t) / (4 * M_PI);

				// The geometric term for the change of coordinates (from directions to point on surfaces...)
				float geometricTerm = abs(dot(toLightRayCollision.second.m_colNormalW, directionToLightSample)) / sqrDistToSamplePoint;
				// You now the drill
				vec3 sampleContribution = lightIrradiance * cont;// geometricTerm * cont;

				montecarloRadiance += sampleContribution * (1.0f / samplePDF);
			}
			if (MCSamples != 0)
				lightContribution += montecarloRadiance * (1.0f / MCSamples);
		}
		indirectRadiance += dt * ComputeTransmittance(dt) * lightContribution  * m_volumeScatterCoeff;
		marchedDistance += dt;
	}
	return indirectRadiance;
}

vec3 BLAengine::PBRPhotonMapping::Shade(Ray incidentRay)
{
	auto p = IntersectWithScene(incidentRay, m_sceneObjects);

	if (p.first == nullptr)
		return vec3(0);

	Collider::RayCollision shadeRayCollision = p.second;
	//return p.second.m_colNormalW;
	vec3 indirectVolumetricRadiance = MarchIndirectVolumetric(incidentRay, p.second.m_t, 256);

	vec3 directVolumetricRadiance(0);// = MarchDirectEquiAngular(incidentRay, p.second.m_t, 100);

	vec3 volumetricRadiance = indirectVolumetricRadiance + directVolumetricRadiance;

	PBRMaterial* m = &(p.first->m_material);

	if (length(m->m_emissivePower) > 0.0f)
	{
		return ComputeTransmittance(shadeRayCollision.m_t) * m->m_emissivePower / p.first->GetSurfaceArea();
	}

	mat3 tangentSpace = GetTangentSpace(shadeRayCollision.m_colNormalW);

	vec3 directRadiance(0);
	for (PBRSurface* lightSurface : m_lightObjects)
	{
		vec3 montecarloRadiance = vec3(0);
		float MCSamples = 50;
		for (int mc = 0; mc < MCSamples; mc++)
		{

			vec3 samplePoint;
			float samplePDF;

			lightSurface->SampleSurface(samplePoint, samplePDF);

			vec3 directionToLightSample = samplePoint - shadeRayCollision.m_colPositionW;
			float sqrDistToSamplePoint = dot(directionToLightSample, directionToLightSample);
			directionToLightSample /= sqrt(sqrDistToSamplePoint);

			Ray toLightRay(shadeRayCollision.m_colPositionW + 0.001f * shadeRayCollision.m_colNormalW, directionToLightSample, INFINITY);

			auto toLightRayCollision = IntersectWithScene(toLightRay, m_sceneObjects);

			vec3 lightIrradiance(0);
			if (toLightRayCollision.first == lightSurface)
			{
				lightIrradiance = lightSurface->m_material.m_emissivePower / lightSurface->GetSurfaceArea();
			}
			else
			{
				continue;
			}
			// The diffuse Brdf...
			vec3 evaluatedBRDF = m->m_brdf->EvaluateBRDF(lightIrradiance, shadeRayCollision.m_colNormalW, directionToLightSample, -incidentRay.m_direction);

			// The geometric term for the change of coordinates (from directions to point on surfaces...)
			float geometricTerm = abs(dot(toLightRayCollision.second.m_colNormalW, directionToLightSample)) / sqrDistToSamplePoint;
			// You now the drill
			vec3 sampleContribution = evaluatedBRDF * geometricTerm * dot(shadeRayCollision.m_colNormalW, directionToLightSample);

			montecarloRadiance += sampleContribution * (1.0f / samplePDF);
		}
		if (MCSamples != 0)
			directRadiance += montecarloRadiance * (1.0f / MCSamples);
	}

	vec3 indirectRadiance(0);
	float indirectMCsamples = 20;
	for (int i = 0; i < indirectMCsamples; i++)
	{
		vec3 outDir;
		float samplePdf = m->m_brdf->SampleBRDF(outDir, tangentSpace, incidentRay.m_direction);

		Ray outRay(shadeRayCollision.m_colPositionW + 0.001f*shadeRayCollision.m_colNormalW, outDir, INFINITY);

		vec3 incomingRadiance = GatherSurfaceDensity(outRay);

		vec3 evaluatedBRDF = m->m_brdf->EvaluateBRDF(incomingRadiance, shadeRayCollision.m_colNormalW, outDir, -incidentRay.m_direction);

		indirectRadiance += evaluatedBRDF;
	}
	if (indirectMCsamples != 0)
		indirectRadiance /= indirectMCsamples;
	//printVector(volumetricRadiance);
	return ComputeTransmittance(shadeRayCollision.m_t) * (m->m_color * (directRadiance + indirectRadiance)) + volumetricRadiance;
}

vector<vec3> BLAengine::PBRPhotonMapping::Render(Transform cameraTransform, vec2 resolution, bool inParallel)
{
	for (PBRSurface* surface : m_sceneObjects)
	{
		if (length(surface->m_material.m_emissivePower) > 0.0f)
		{
			m_lightObjects.push_back(surface);
		}
	}

	int w = resolution.x, h = resolution.y;

	float horizontalFov = 100;

	float fovFactor = tan((125 / 180.0f) * M_PI);

	vec3 cameraForward = cameraTransform.LocalDirectionToWorld(vec3(0, 0, -1));

	vec3 cx = vec3(w * fovFactor / h, 0.f, 0.f); // hint : tan( 30 / 180.0 * M_PI ) == 0.57735
	vec3 cy = cross(cx, cameraForward) * fovFactor * (h / (float)w);


	vec3* concurrent_img_vector = (vec3*)malloc(sizeof(vec3)*h*w);

	vector<vec3> renderedImage;
	renderedImage.reserve(w*h);

	BuildPhotonMap(inParallel, 8000000);

	std::cout << "\nRendering:\n";
	if (inParallel)
	{
		size_t ht = h;
		int progress = 0;
		parallel_for(size_t(0), ht, [&](size_t yp)
		{
			int y = (int)yp;
			fprintf(stderr, "\r%5.2f%%", 100.*progress / (h - 1));

			for (int x = 0; x < w; x++)
			{
				int idx = (h - y - 1) * w + x;
				vec3 pixelValue(0);
				vec3 cameraRayDir = cx * (x / (float)w - 0.5f) + cy * (y / (float)h - 0.5f) + cameraForward;
				Ray ray = Ray(cameraTransform.m_position, cameraRayDir, INFINITY);
				pixelValue = Shade(ray);
				concurrent_img_vector[idx] = vec3(ClampOne(pixelValue.x), ClampOne(pixelValue.y), ClampOne(pixelValue.z));
			}
			progress++;
		});
	}
	else
	{
		for (int y = 0; y < h; y++)
		{
			fprintf(stderr, "\r%5.2f%%", 100.*y / (h - 1));
			for (int x = 0; x < w; x++) {
				int idx = (h - y - 1) * w + x;
				vec3 pixelValue(0);
				vec3 cameraRayDir = cx * (x / (float)w - 0.5f) + cy * (y / (float)h - 0.5f) + cameraForward;
				Ray ray = Ray(cameraTransform.m_position, cameraRayDir, INFINITY);
				pixelValue = Shade(ray);
				concurrent_img_vector[idx] = vec3(ClampOne(pixelValue.x), ClampOne(pixelValue.y), ClampOne(pixelValue.z));
			}
		}
	}

	fprintf(stderr, "\n");

	for (int i = 0; i <h*w; i++)
	{
		renderedImage[i] = concurrent_img_vector[i];
	}
	free(concurrent_img_vector);


	return renderedImage;
}