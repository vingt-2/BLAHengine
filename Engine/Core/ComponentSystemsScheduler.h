#pragma once

#include "StdInclude.h"
#include "BLAStringID.h"
#include <mutex>

namespace BLA
{
    class ComponentSystemsRegistry;
    class ComponentSystem;
    struct ComponentSystemsScheduler
    {
        void RebuildScheduler(const ComponentSystemsRegistry* registry);
        void RefreshSchedulerState();
        bool GetNextJob(blaStringId& job);
        void NotifyCompletion(blaStringId job);

    private:

        bool GetNextJob(blaStringId root, blaStringId& job);

        struct SchedulerNode
        {
            blaVector<blaStringId> m_children;

            blaIndex m_parentToComplete;

            const ComponentSystem* m_componentSystemRegisteryEntry;

            enum SystemState : blaU8
            {
                TO_RUN,
                RUNNING,
                COMPLETED
            } m_state;
        };

        std::mutex m_lock;
        blaMap<blaStringId, SchedulerNode> m_graph;
    };
}