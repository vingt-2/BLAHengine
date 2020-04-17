#pragma once
#include "StdInclude.h"
#include "ComponentSystems.h"

#define BeginBehaviorDeclaration(ProjectName, BehaviorName)             \
    class BehaviorName;                                                 \
    DeclareComponentSystem(ProjectName, BehaviorSystem ## BehaviorName, \
        InputComponents(), OutputComponents(BehaviorName))              \
    BeginComponentDeclaration(ProjectName, BehaviorName)                \

#define EndBehaviorDeclaration()                                        \
    void Update();                                                      \
    EndComponentDeclaration()                                           \

#define BeginBehaviorDescription(BehaviorName, ...)                          \
    RegisterComponentSystem(BehaviorSystem ## BehaviorName, __VA_ARGS__)     \
    void BehaviorSystem ## BehaviorName::Execute(SystemObjectsIterator& os,  \
        OutputComponents<BehaviorName> b)                                    \
            { for(auto o : os) b[o]->Update(); }                             \
    BeginComponentDescription(BehaviorName)                                  \
                                                                             
#define EndBehaviorDescription()                                             \
    EndComponentDescription()                                         