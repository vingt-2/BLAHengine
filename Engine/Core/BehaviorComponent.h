// BLAEngine Copyright (C) 2016-2020 Vincent Petrella. All rights reserved.

#pragma once
#include "StdInclude.h"
#include "ComponentSystems.h"

#define BeginBehaviorDeclaration(ProjectName, BehaviorName)                                           \
    class BehaviorName;                                                                               \
    DeclareComponentSystem(ProjectName, BehaviorSystem ## BehaviorName,                               \
        InputComponents(), OutputComponents(BehaviorName))                                            \
    BeginComponentDeclaration(ProjectName, BehaviorName)                                              \
    friend class BLA::Core::TypedComponentSystem<BLA::Core::IOTS<>, BLA::Core::IOTS<BehaviorName>>;   \

#define EndBehaviorDeclaration()                                        \
    private:                                                            \
    void Update();                                                      \
    EndComponentDeclaration()                                           \

// TODO: Remove existence check in update call b[o] should never return nullptr
#define BeginBehaviorDescription(BehaviorName, ...)                                     \
    RegisterComponentSystem(BehaviorSystem ## BehaviorName, __VA_ARGS__)                \
    void BehaviorSystem##BehaviorName::Execute(BLA::Core::SystemObjectsIterator& os,    \
        BLA::Core::OutputComponents<BehaviorName> b)                                    \
            { for(auto o : os) { if(b.find(o) != b.end()) {b[o]->Update();}}}           \
    BeginComponentDescription(BehaviorName)                                             \
                                                                                        
#define EndBehaviorDescription()    \
    EndComponentDescription()                                         