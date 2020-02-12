file(GLOB_RECURSE ENGINE_SOURCE_FILES 
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

GroupSources(.)

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

# add_custom_command(TARGET BLAEngine POST_BUILD      # Adds a post-build event to MyTest
    # COMMAND ${CMAKE_COMMAND} -E copy_if_different  	# which executes "cmake - E copy_if_different..."
        # "${LIBRARY_OUTPUT_PATH}/BLAengine.dll" 		# <--this is in-file
        # ${EXECUTABLE_OUTPUT_PATH})                 	# <--this is out-file path

if(WIN32)
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS")
	target_link_libraries(BLAEngine PRIVATE setupapi.lib)
endif()