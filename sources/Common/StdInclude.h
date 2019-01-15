

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

using namespace std;

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