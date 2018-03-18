/**
    Code modified from examples provided by nanoflann dev team
*/

/***********************************************************************
 * Software License Agreement (BSD License)
 *
 * Copyright 2011-2016 Jose Luis Blanco (joseluisblancoc@gmail.com).
 *   All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *************************************************************************/
#pragma once
#include "nanoflann.h"
#include "../../../Common/StdInclude.h"
#include "../../../Common/Maths.h"
#include <concurrent_vector.h>

using namespace std;
using namespace nanoflann;

// This is an example of a custom data set class
struct Photon
{
    Photon(vec3 pos, vec3 dir, vec3 power)
    {
        m_pos = pos;
        m_dir = dir;
        m_power = power;
    }
    vec3 m_pos;
    vec3 m_dir;
    vec3 m_power;
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

    PhotonMap(uint sizeHint);
    ~PhotonMap();

    void AddPhoton(Photon photon);
    
    void BuildKDTree();

    vector<Photon*> PhotonMap::GetPhotons(vec3 position, int knnSize, float &biggestRadius);

private:
    PhotonVector m_photonVector;

    typedef KDTreeSingleIndexAdaptor<L2_Simple_Adaptor<float, PhotonVector>, PhotonVector, 3> PhotonTree;
    PhotonTree* m_photonTree;
};

inline PhotonMap::PhotonMap(uint sizeHint)
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

inline vector<Photon*> PhotonMap::GetPhotons(vec3 position, int knnSize, float &outRadius)
{
    std::vector<Photon*> results;
    
    std::vector<size_t>   ret_index(knnSize);
    std::vector<float> out_dist_sqr(knnSize);
    
    nanoflann::SearchParams params;
    //params.sorted = false;

    const size_t nMatches = m_photonTree->knnSearch(&position[0], knnSize, &(ret_index[0]), &(out_dist_sqr[0]));

    results.reserve(nMatches);
    
    outRadius = 0;
    for (int i = 0; i < ret_index.size(); i++)
    {
        size_t matchIndx = ret_index[i];
        float dist = out_dist_sqr[i];
        results.push_back(&(m_photonVector.m_photons[matchIndx]));
        if (dist > outRadius)
            outRadius = dist;
    }

    return results;
}
