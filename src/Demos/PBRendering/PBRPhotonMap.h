#pragma once

#include <External/nanoflann/nanoflann.h>

#include <Common/StdInclude.h>
#include <Common/Maths/Maths.h>

#include <concurrent_vector.h>

using namespace std;
using namespace nanoflann;

// This is an example of a custom data set class
struct Photon
{
    Photon(blaVec3 pos, blaVec3 dir, blaVec3 power)
    {
        m_pos = pos;
        m_dir = dir;
        m_power = power;
    }
    blaVec3 m_pos;
    blaVec3 m_dir;
    blaVec3 m_power;
};

struct PhotonVector
{
    concurrency::concurrent_vector<Photon>  m_photons;

    // Must return the number of data points
    inline size_t kdtree_get_point_count() const { return m_photons.size(); }

    // Returns the distance between the vector "p1[0:size-1]" and the data point with index "idx_p2" stored in the class:
    inline float kdtree_distance(const float *p1, const size_t idx_p2, size_t) const
    {
        const float d0=p1[0]- m_photons[idx_p2].m_pos.x;
        const float d1=p1[1] - m_photons[idx_p2].m_pos.y;
        const float d2=p1[2] - m_photons[idx_p2].m_pos.z;
        return d0*d0+d1*d1+d2*d2;
    }

    // Returns the dim'th component of the idx'th point in the class:
    // Since this is inlined and the "dim" argument is typically an immediate value, the
    //  "if/else's" are actually solved at compile time.
    inline float kdtree_get_pt(const size_t idx, int dim) const
    {
        if (dim==0) return m_photons[idx].m_pos.x;
        else if (dim==1) return m_photons[idx].m_pos.y;
        else return m_photons[idx].m_pos.z;
    }

    // Optional bounding-box computation: return false to default to a standard bbox computation loop.
    //   Return true if the BBOX was already computed by the class and returned in "bb" so it can be avoided to redo it again.
    //   Look at bb.size() to find out the expected dimensionality (e.g. 2 or 3 for point clouds)
    template <class BBOX>
    bool kdtree_get_bbox(BBOX& /*bb*/) const { return false; }

};

class PhotonMap
{
public:

    PhotonMap(blaU32 sizeHint);
    ~PhotonMap();

    void AddPhoton(Photon photon);
    
    void BuildKDTree();

    blaVector<Photon*> PhotonMap::GetPhotons(blaVec3 position, int knnSize, float &biggestRadius);

private:
    PhotonVector m_photonVector;

    typedef KDTreeSingleIndexAdaptor<L2_Simple_Adaptor<float, PhotonVector>, PhotonVector, 3> PhotonTree;
    PhotonTree* m_photonTree;
};

inline PhotonMap::PhotonMap(blaU32 sizeHint)
{
    m_photonTree = nullptr;
}

inline PhotonMap::~PhotonMap()
{
}

inline void PhotonMap::AddPhoton(Photon photon)
{
    m_photonVector.m_photons.push_back(photon);
}

inline void PhotonMap::BuildKDTree()
{
    m_photonTree = new PhotonTree(3, m_photonVector, KDTreeSingleIndexAdaptorParams(10 /* max leaf */));

    m_photonTree->buildIndex();
}

inline blaVector<Photon*> PhotonMap::GetPhotons(blaVec3 position, int knnSize, float &outRadius)
{
    blaVector<Photon*> results;
    
    blaVector<size_t>   ret_index(knnSize);
    blaVector<float> out_dist_sqr(knnSize);
    
    nanoflann::SearchParams params;
    //params.sorted = false;

    const size_t nMatches = m_photonTree->knnSearch(&position[0], knnSize, &(ret_index[0]), &(out_dist_sqr[0]));

    results.reserve(nMatches);
    
    outRadius = 0;
    for (size_t i = 0; i < ret_index.size(); i++)
    {
        size_t matchIndx = ret_index[i];
        float dist = out_dist_sqr[i];
        results.push_back(&(m_photonVector.m_photons[matchIndx]));
        if (dist > outRadius)
            outRadius = dist;
    }

    return results;
}
