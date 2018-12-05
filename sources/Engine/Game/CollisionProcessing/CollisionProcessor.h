#pragma once
#include "..\GameComponents\RigidBodyComponent.h"
#include "..\..\..\Common\StdInclude.h"
#include "..\time.h"
namespace BLAengine
{
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

        vector<blaVec3> m_normalJacobian;
        vector<blaVec3> m_tangentJacobian1;
        vector<blaVec3> m_tangentJacobian2;

        void ComputeJacobian();
    private:
    };

    class BLACORE_API CollisionProcessor
    {
    public:
        CollisionProcessor(Time* time, float* timestepPtr);
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

        vector<RigidBodyComponent*> m_bodiesList;
        vector<Contact> m_currentContacts;


        //Profiling
        float m_detectionTime;
        float m_processingTime;

        void ProcessCollisions();

        void setTimeObject(Time* time) { m_time = time; }
    private:
        Time* m_time;

        void ComputeT(vector<vector<blaVec3>>& T);
        void GetDiagonalElements(vector<vector<blaVec3>> T, vector<float>& D);
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
        vector<Contact*> m_spatialTable;
    };
}
