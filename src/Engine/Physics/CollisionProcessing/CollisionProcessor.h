#pragma once

#include <Common/StdInclude.h>
#include <Common/Maths/Maths.h>

namespace BLAengine
{
    class RigidBodyComponent;
    class Timer;

    class BLACORE_API Contact
    {
    public:
        Contact(RigidBodyComponent* body1, RigidBodyComponent* body2, blaVec3 colPoint, blaVec3 normalW, blaVec3 tangentW, int face);
        ~Contact();

        int m_faceFrom;
        RigidBodyComponent* m_body1;
        RigidBodyComponent* m_body2;

        blaVec3 m_contactNormalW;
        blaVec3 m_contactTangent1W;
        blaVec3 m_contactTangent2W;

        blaVec3 m_contactPositionW;

        blaVector<blaVec3> m_normalJacobian;
        blaVector<blaVec3> m_tangentJacobian1;
        blaVector<blaVec3> m_tangentJacobian2;

        void ComputeJacobian();
    private:
    };

    class BLACORE_API CollisionProcessor
    {
    public:
        CollisionProcessor(Timer* time, float* timestepPtr);
        ~CollisionProcessor();

        int m_maxIterations;
        bool m_flagInterpolateNormals;
        float m_friction;
        float m_bounce;
        float m_epsilon;

        float* m_timeStepPointer;

        bool debug_stop;

        int m_iterationCount;
        int m_solveCount;

        blaVector<RigidBodyComponent*> m_bodiesList;
        blaVector<Contact> m_currentContacts;


        //Profiling
        float m_detectionTime;
        float m_processingTime;

        void ProcessCollisions();

        void setTimeObject(Timer* time) { m_time = time; }
    private:
        Timer* m_time;

        void ComputeT(blaVector<blaVector<blaVec3>>& T);
        void GetDiagonalElements(blaVector<blaVector<blaVec3>> T, blaVector<float>& D);
        void BroadPhaseDetection();
        void NarrowPhaseDetection(RigidBodyComponent* body1, RigidBodyComponent* body2);
        void SolveContacts();
    };

    class BLACORE_API SpatialCoherence
    {
    public:
        SpatialCoherence(float granularity);
        ~SpatialCoherence();

        void CreateSpatialTable();

    private:

        float m_resolution;
        blaVector<Contact*> m_spatialTable;
    };
}
