#pragma once
#include <cereal\cereal.hpp>
#include <cereal\archives\binary.hpp>
#include <cereal\archives\xml.hpp>

#include "../Common/Maths/Maths.h"

class vec3serializer
{
public:

    vec3serializer() = default;

    void FillData(blaVec3 &vec)
    {
        x = vec.x;
        y = vec.y;
        z = vec.z;
    }

    void LoadData(blaVec3 &vec)
    {
        vec.x = x;
        vec.y = y;
        vec.z = z;
    }

    float x, y, z;

private:

    friend class cereal::access;

    template <class Archive>
    void serialize(Archive & archive)
    {
        archive
        (
            CEREAL_NVP(x), CEREAL_NVP(y), CEREAL_NVP(z)
        );
    }
};

class vec2serializer
{
public:
    vec2serializer() = default;

    void FillData(glm::vec2 &vec)
    {
        x = vec.x;
        y = vec.y;
    }

    void LoadData(glm::vec2 &vec)
    {
        vec.x = x;
        vec.y = y;
    }

    float x, y;

private:

    friend class cereal::access;

    template <class Archive>
    void serialize(Archive & archive)
    {
        archive
        (
            CEREAL_NVP(x), CEREAL_NVP(y)
        );
    }
};

class quatserializer
{
public:

    quatserializer() = default;

    void FillData(blaQuat &q)
    {
        x = q.x;
        y = q.y;
        z = q.z;
        w = q.w;
    }

    void LoadData(blaQuat &q)
    {
        q.x = x;
        q.y = y;
        q.z = z;
        q.w = w;
    }

    float x, y, z, w;

private:

    friend class cereal::access;

    template <class Archive>
    void serialize(Archive & archive)
    {
        archive
        (
            CEREAL_NVP(x), CEREAL_NVP(y), CEREAL_NVP(z), CEREAL_NVP(w)
        );
    }
};

class mat3serializer
{
public:

    mat3serializer() = default;

    void FillData(blaMat3 &mat)
    {
        a = mat[0][0];
        b = mat[0][1];
        c = mat[0][2];
        d = mat[1][0];
        e = mat[1][1];
        f = mat[1][2];
        g = mat[2][0];
        h = mat[2][1];
        i = mat[2][2];
    }

    void LoadData(blaMat3 &mat)
    {
        mat[0][0] = a;
        mat[0][1] = b;
        mat[0][2] = c;
        mat[1][0] = d;
        mat[1][1] = e;
        mat[1][2] = f;
        mat[2][0] = g;
        mat[2][1] = h;
        mat[2][2] = i;

    }

    float a, b, c, d, e, f, g, h, i;

private:

    friend class cereal::access;

    template <class Archive>
    void serialize(Archive & archive)
    {
        archive
        (
            CEREAL_NVP(a), CEREAL_NVP(b), CEREAL_NVP(c),
            CEREAL_NVP(d), CEREAL_NVP(e), CEREAL_NVP(f),
            CEREAL_NVP(g), CEREAL_NVP(h), CEREAL_NVP(i)
        );
    }
};