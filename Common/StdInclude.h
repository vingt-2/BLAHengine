// Copyright (C) 2018 Vincent Petrella. All rights reserved

#pragma once

#include <sstream> 
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <iostream>
#include <fstream>
#include <string>
#include <math.h>
#include <vector>
#include <algorithm>
#include <map>
#include <utility>
#include <unordered_map>
#include <tuple>
#include <functional>

#ifdef BLAEngine_EXPORTS
#define    BLACORE_API __declspec ( dllexport )
#else
#define BLACORE_API __declspec ( dllimport )
#endif

#ifndef WIN32
#define WIN32
#endif

using blaString = std::string;

template<typename  T>
using blaVector = std::vector<T>;

template<typename T, typename V, typename H>
using blaHashMap = std::unordered_map<T,V,H>;

template<typename T, typename V>
using blaMap = std::map<T, V>;

template<typename T, typename V>
using blaPair = std::pair<T, V>;

template<typename L>
using blaLambda = std::function<L>;

template<typename T>
using blaOwnedPointer = std::unique_ptr<T>;

#define ArraySize(arr) (int)(sizeof(arr) / sizeof(*arr))

#define BLA_ENGINE_VERSION 1

// TODO: Obviously windows specific for now ...
#define DLL_PROJECT_DECLARE                                                             \
    BOOL WINAPI DllMain(                                                                \
    _In_ HINSTANCE hinstDLL,                                                            \
    _In_ DWORD     fdwReason,                                                           \
    _In_ LPVOID    lpvReserved                                                          \
    )                                                                                   \
    {                                                                                   \
        int engineVersion =                                                             \
            BLA::EngineInstance::GetSingletonInstanceRead()->GetEngineVersion();        \
        if(engineVersion != BLA_ENGINE_VERSION)                                         \
        {                                                                               \
            return false;                                                               \
        }                                                                               \
        return true;                                                                    \
    }

#define VULKAN 1