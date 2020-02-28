file(GLOB_RECURSE ENGINE_SOURCE_FILES 
${CMAKE_CURRENT_SOURCE_DIR}/*.h 
${CMAKE_CURRENT_SOURCE_DIR}/*.c
${CMAKE_CURRENT_SOURCE_DIR}/*.hpp 
${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

GroupSources(./)

if(MSVC)
    add_definitions(/FI"${CMAKE_CURRENT_SOURCE_DIR}/GeneratedProjectDefinition.h")
else()
    # GCC or Clang
    add_definitions(-include ${CMAKE_CURRENT_SOURCE_DIR}/GeneratedProjectDefinition.h)
endif()

include_directories(
${COMMON_INCLUDE_DIR}
${CMAKE_CURRENT_SOURCE_DIR} 
${GLFW_INCLUDE_DIR} 
${GLEW_INCLUDE_DIR} 
${glm_INCLUDE_DIR} 
${cereal_INCLUDE_DIR}
${imgui_INCLUDE_DIR}
${VulkanHeaders_INCLUDE_DIR})

find_package(OpenGL REQUIRED)

# Gotta do the following here because glew with glew-cmake will build the target in build/lib which is otherwise unreachable
	link_directories(${CMAKE_BINARY_DIR}/lib)
#

add_library(BLAEngine SHARED ${ENGINE_SOURCE_FILES})

target_link_libraries(BLAEngine PRIVATE glfw PRIVATE glew PRIVATE ${OPENGL_LIBRARIES} PRIVATE vulkan-1.lib)


if(WIN32)
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS")
	target_link_libraries(BLAEngine PRIVATE setupapi.lib)
endif()