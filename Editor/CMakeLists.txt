file(GLOB_RECURSE EDITOR_SOURCE_FILES 
${CMAKE_CURRENT_SOURCE_DIR}/*.h 
${CMAKE_CURRENT_SOURCE_DIR}/*.c
${CMAKE_CURRENT_SOURCE_DIR}/*.hpp 
${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

# Organize the files in folders according to the organization in the file systems
macro(GroupSources curdir)

   file(GLOB children RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}/${curdir} ${CMAKE_CURRENT_SOURCE_DIR}/${curdir}/*)

   foreach(child ${children})

          if(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${curdir}/${child})

				GroupSources(${curdir}/${child})

          else()

				string(REPLACE "/" "\\" groupname ${curdir})

				string(REPLACE "." "Sources" groupname ${groupname})

            source_group(${groupname} FILES ${CMAKE_CURRENT_SOURCE_DIR}/${curdir}/${child})

          endif()

   endforeach()

endmacro()

GroupSources(./)

if(MSVC)
    add_definitions(/FI"${CMAKE_CURRENT_SOURCE_DIR}/GeneratedProjectDefinition.h")
else()
    # GCC or Clang
    add_definitions(-include ${CMAKE_CURRENT_SOURCE_DIR}/GeneratedProjectDefinition.h)
endif()

include_directories(
${COMMON_INCLUDE_DIR}
${GLEW_INCLUDE_DIR} # TODO: Remove dependencies on glew here ...
${CMAKE_CURRENT_SOURCE_DIR} 
${GLFW_INCLUDE_DIR} 
${glm_INCLUDE_DIR}
${VulkanHeaders_INCLUDE_DIR}
${PROJECT_SOURCE_DIR}/Engine
)

add_executable(BLAEditor ${EDITOR_SOURCE_FILES})

target_link_libraries(BLAEditor BLAEngine)

if(WIN32)
	set_property(TARGET BLAEditor PROPERTY VS_DEBUGGER_WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS")
endif()