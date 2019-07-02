

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

#define BLA_NO_DLL
//#define EXPORT_DLL

#ifndef BLA_NO_DLL
#ifdef EXPORT_DLL
#define    BLACORE_API __declspec ( dllexport )
#else
#define BLACORE_API __declspec ( dllimport )
#endif
#else
#define BLACORE_API
#endif

#ifndef WIN32
#define WIN32
#endif

typedef std::string blaString;

#define blaVector   std::vector
#define blaHashMap  std::unordered_map
#define blaMap      std::map
#define blaPair     std::pair