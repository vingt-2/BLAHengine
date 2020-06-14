#include "ComponentSystemsScheduler.h"

#include "Core/ComponentSystems.h"

using namespace BLA;

void ComponentSystemsScheduler::RebuildScheduler(const ComponentSystemsRegistry* registry)
{
    m_lock.lock();

    m_graph.clear();
    m_graph.insert(std::make_pair(BlaStringId("RootSystem"), SchedulerNode{ blaVector<blaStringId>(), 0, nullptr, SchedulerNode::SchedulerNode::TO_RUN }));

    for (auto registryEntry : registry->GetAllAvailableSystems())
    {
        const blaVector<blaStringId>& dependencies = registryEntry.second->GetSystemDependencies();

        auto graphEntry = m_graph.find(registryEntry.first);
        if (graphEntry == m_graph.end())
        {
            SchedulerNode node = { blaVector<blaStringId>(), dependencies.size(), registryEntry.second, SchedulerNode::TO_RUN };
            m_graph.insert(std::make_pair(registryEntry.first, node));
        }
        else
        {
            graphEntry->second.m_parentToComplete = dependencies.size();
            graphEntry->second.m_componentSystemRegisteryEntry = registryEntry.second;
        }

        for (auto parent : dependencies)
        {
            auto parentGraphEntry = m_graph.find(parent);
            if (parentGraphEntry == m_graph.end())
            {
                SchedulerNode node = { blaVector<blaStringId>(), 0, nullptr, SchedulerNode::TO_RUN }; // m_parentToComplete and m_componentSystemRegisteryEntry To Set upon visiting
                parentGraphEntry = m_graph.insert(std::make_pair(parent, node)).first;
            }
            parentGraphEntry->second.m_children.push_back(registryEntry.first);
        }
    }
    m_lock.unlock();
}

void ComponentSystemsScheduler::RefreshSchedulerState()
{
    m_lock.lock();
    for(auto se : m_graph) 
    {
        m_graph[se.first].m_parentToComplete = se.second.m_componentSystemRegisteryEntry->GetSystemDependencies().size();
        m_graph[se.first].m_state = SchedulerNode::TO_RUN;
    }
    m_lock.unlock();
}

bool ComponentSystemsScheduler::GetNextJob(blaStringId& job)
{
    m_lock.lock();
    bool result = GetNextJob(BlaStringId("RootSystem"), job);
    m_lock.unlock();
    return result;
}

void ComponentSystemsScheduler::NotifyCompletion(blaStringId job)
{
    m_lock.lock();
    SchedulerNode& node = m_graph.at(job);
    node.m_state = SchedulerNode::COMPLETED;
    for (blaStringId child : node.m_children)
    {
        m_graph[child].m_parentToComplete--;
    }
    m_lock.unlock();
}

bool ComponentSystemsScheduler::GetNextJob(blaStringId id, blaStringId& job)
{
    SchedulerNode& node = m_graph.at(id);

    if (node.m_parentToComplete < 1 && node.m_state == SchedulerNode::TO_RUN)
    {
        job = id;
        return true;
    }

    for (blaStringId child : node.m_children)
    {
        if (GetNextJob(child, job))
            return true;
    }

    return false;
}
