cmake_minimum_required(VERSION 3.1)

project(BLAEngine CXX)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake/Modules/")

#set the default path for built executables to the "bin" directory

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

#set the default path for built libraries to the "lib" directory

set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

#Specify my Include Directory

set(PROJECT_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/src)

set(PROJECT_EXTERNAL_DIR ${PROJECT_SOURCE_DIR}/external)

macro(GetHeaderDependency dep)
	# we have a submodule setup for dep, assume it is under external/dep
    # now we need to clone this submodule
	execute_process(COMMAND git submodule update --init ${PROJECT_EXTERNAL_DIR}/${dep}
                    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
	
    execute_process(COMMAND git submodule update --recursive --remote ${PROJECT_EXTERNAL_DIR}/${dep}
                    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

    # # also install it
    # install(DIRECTORY ${FOO_INCLUDE_DIR}/foo DESTINATION ${some_dest})

    # for convenience setup a target
    add_library(${dep} INTERFACE)
    target_include_directories(${dep} INTERFACE
                               $<INSTALL_INTERFACE:inlude/${dep}>
                               $<BUILD_INTERFACE:${${dep}_INCLUDE_DIR}>)

    # need to export target as well
    #install(TARGETS foo EXPORT my_export_set DESTINATION ${some_dest})
endmacro()

macro(GetCMakeDependency dep)
	# we have a submodule setup for dep, assume it is under external/dep
    # now we need to clone this submodule
	execute_process(COMMAND git submodule update --init ${PROJECT_EXTERNAL_DIR}/${dep}
                    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
					
    execute_process(COMMAND git submodule update --recursive --remote ${PROJECT_EXTERNAL_DIR}/${dep}
                    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

endmacro()

find_path(glm_INCLUDE_DIR glm ${PROJECT_EXTERNAL_DIR}/glm)

if(NOT glm_INCLUDE_DIR OR (NOT EXISTS ${glm_INCLUDE_DIR}))
    # we couldn't find the header files for GLM or they don't exist
    message("Unable to find GLM, fetching the git submodule...")

    GetHeaderDependency(glm)
	
	find_path(glm_INCLUDE_DIR glm ${PROJECT_EXTERNAL_DIR}/glm)
	
	if(NOT glm_INCLUDE_DIR)
		message("GLM submodule failed to clone")
	endif()
endif()

find_path(cereal_INCLUDE_DIR cereal ${PROJECT_EXTERNAL_DIR}/cereal/include)

if(NOT cereal_INCLUDE_DIR OR (NOT EXISTS ${cereal_INCLUDE_DIR}))
    # we couldn't find the header files for GLM or they don't exist
    message("Unable to find CEREAL, fetching the git submodule...")

    GetHeaderDependency(cereal)
	
	find_path(cereal_INCLUDE_DIR cereal ${PROJECT_EXTERNAL_DIR}/cereal/include)
	
	if(NOT cereal_INCLUDE_DIR)
		message("CEREAL submodule failed to clone")
	endif()
endif()

find_path(imgui_INCLUDE_DIR imgui.h ${PROJECT_EXTERNAL_DIR}/imgui)

if(NOT imgui_INCLUDE_DIR OR (NOT EXISTS ${imgui_INCLUDE_DIR}))
    # we couldn't find the header files for GLM or they don't exist
    message("Unable to find Imgui, fetching the git submodule...")

    GetHeaderDependency(imgui)
	
	find_path(imgui_INCLUDE_DIR imgui.h ${PROJECT_EXTERNAL_DIR}/imgui)
	
	if(NOT imgui_INCLUDE_DIR)
		message("IMGUI submodule failed to clone")
	endif()
endif()

GetCMakeDependency(glfw)
set(GLFW_LIBRARY ${PROJECT_EXTERNAL_DIR}/glfw CACHE PATH "GLFW directory")
add_subdirectory(${GLFW_LIBRARY})
set(GLFW_INCLUDE_DIR ${GLFW_LIBRARY}/include)

GetCMakeDependency(glew-cmake)
set(GLEW_LIBRARY ${PROJECT_EXTERNAL_DIR}/glew-cmake CACHE PATH "GLEW directory")
add_subdirectory(${GLEW_LIBRARY})
set(GLEW_INCLUDE_DIR ${GLEW_LIBRARY}/include)

# also new ones, without having to touch the CMakeLists.txt

file(GLOB_RECURSE PROJECT_SOURCE_FILES 
${PROJECT_SOURCE_DIR}/src/*.h 
${PROJECT_SOURCE_DIR}/src/*.c
${PROJECT_SOURCE_DIR}/src/*.hpp 
${PROJECT_SOURCE_DIR}/src/*.cpp)

# I then wrote this Macro, which organize the files in visual studio folders,

# according to the organization in the file systems

macro(GroupSources curdir)

   file(GLOB children RELATIVE ${PROJECT_SOURCE_DIR}/${curdir} ${PROJECT_SOURCE_DIR}/${curdir}/*)

   foreach(child ${children})

          if(IS_DIRECTORY ${PROJECT_SOURCE_DIR}/${curdir}/${child})

				GroupSources(${curdir}/${child})

          else()

				string(REPLACE "/" "\\" groupname ${curdir})

				# I would like to call the src root folder in a different name, only in visual studio (not mandatory requirement)

				string(REPLACE "src" "Sources" groupname ${groupname})

            source_group(${groupname} FILES ${PROJECT_SOURCE_DIR}/${curdir}/${child})

          endif()

   endforeach()

endmacro()

# Make groups for all source subfolders
GroupSources(src)

include_directories( ${PROJECT_INCLUDE_DIR} 
${GLFW_INCLUDE_DIR} 
${GLEW_INCLUDE_DIR} 
${glm_INCLUDE_DIR} 
${cereal_INCLUDE_DIR}
${imgui_INCLUDE_DIR})

find_package(OpenGL REQUIRED)

# Gotta do the following here because glew with glew-cmake will build the target in build/lib which is otherwise unreachable
	link_directories(${CMAKE_BINARY_DIR}/lib)
#

add_executable(${PROJECT_NAME} ${PROJECT_SOURCE_FILES})

target_link_libraries(${PROJECT_NAME} glfw glew ${OPENGL_LIBRARIES})

if(WIN32)
	set_property(TARGET ${PROJECT_NAME}  PROPERTY VS_DEBUGGER_WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS")
	target_link_libraries(${PROJECT_NAME} setupapi.lib)
endif()