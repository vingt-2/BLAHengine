#pragma once
#define GLEW_STATIC
#include <GL/glew.h>

#define GLFW_INTERFACE
#ifdef GLFW_INTERFACE
    #define GLFW_INCLUDE_GLU
    #include <GLFW/glfw3.h>
#else
#include <GL/gl.h>
#include <GL/glext.h>
#endif

#include <glm\glm.hpp>

#ifdef BLA_NO_GLM
typedef char blaU8;
typedef glm::i8 blaS8;
typedef unsigned short blaU16;
typedef short blaS16;
typedef unsigned long blaU32;
typedef long blaS32;
typedef float blaF32;
typedef float blaF64;
#else
typedef glm::u8 blaU8;
typedef glm::i8 blaS8;
typedef glm::u16 blaU16;
typedef glm::i16 blaS16;
typedef glm::u32 blaU32;
typedef glm::i32 blaS32;
typedef glm::f32 blaF32;
typedef glm::f64 blaF64;
#endif

typedef bool blaBool;

namespace BLAColors
{
    static const glm::vec3 RED = glm::vec3(1.f, 0.f, 0.f);
    static const glm::vec3 GREEN = glm::vec3(0.f, 0.5f, 0.f);
    static const glm::vec3 LIME = glm::vec3(0.f, 0.5f, 0.f);
    static const glm::vec3 BLUE = glm::vec3(0.f, 0.f, 1.f);
    static const glm::vec3 YELLOW = glm::vec3(1.f, 1.f, 0.f);
    static const glm::vec3 MAGENTA = glm::vec3(1.f, 0.f, 1.f);
    static const glm::vec3 PURPLE = glm::vec3(0.5f, 0.f, 0.5f);
    static const glm::vec3 ORANGE = glm::vec3(1.f, 0.3f, 0.f);

};