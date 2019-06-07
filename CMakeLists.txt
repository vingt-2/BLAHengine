cmake_minimum_required(VERSION 2.8.11)

project(BLAHEngine CXX)

#set the default path for built executables to the "bin" directory

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

#set the default path for built libraries to the "lib" directory

set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

#Specify my Include Directory

set(PROJECT_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/src)

set(PROJECT_EXTERNAL_DIR ${PROJECT_SOURCE_DIR}/src/External)

macro(GetHeaderDependency dep pathToInclude)
	# we have a submodule setup for dep, assume it is under external/dep
    # now we need to clone this submodule
    execute_process(COMMAND git submodule update --init -- ${PROJECT_EXTERNAL_DIR}/${dep}
                    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

    # set dep_INCLUDE_DIR properly
    set(${dep}_INCLUDE_DIR ${PROJECT_EXTERNAL_DIR}/${dep}/${pathToInclude}
        CACHE PATH ${dep}"include directory")

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
    execute_process(COMMAND git submodule update --init -- ${PROJECT_EXTERNAL_DIR}/${dep}
                    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

endmacro()

include_directories( ${PROJECT_INCLUDE_DIR} )

find_path(GLM_INCLUDE_DIR glm.hpp ${PROJECT_EXTERNAL_DIR}/glm/glm)

if(NOT GLM_INCLUDE_DIR OR (NOT EXISTS ${GLM_INCLUDE_DIR}))
    # we couldn't find the header files for FOO or they don't exist
    message("Unable to find GLM")

    GetHeaderDependency(glm glm/)
	
	find_path(GLM_INCLUDE_DIR glm.hpp ${PROJECT_EXTERNAL_DIR}/glm/glm)
		if(NOT GLM_INCLUDE_DIR)
		message("GLM submodule failed to clone")
	else()
		include_directories(GLM_INCLUDE_DIR)
	endif()	
else()
	include_directories(GLM_INCLUDE_DIR)
endif()

find_library(GLFW_LIBRARY ${PROJECT_EXTERNAL_DIR}/glfw)

if(NOT GLFW_LIBRARY OR (NOT EXISTS ${GLFW_LIBRARY}))
    # we couldn't find the header files for FOO or they don't exist
    message("Unable to find GLFW")

    GetCMakeDependency(glfw)

	find_library(GLFW_LIBRARY ${PROJECT_EXTERNAL_DIR}/glfw)
	
	if(NOT GLFW_LIBRARY)
		message("GLFW submodule failed to clone")
	else()
		add_subdirectory(GLFW_LIBRARY)
		include_directories(${GLFW_LIBRARY}/include)
	endif()
else()
	add_subdirectory(GLFW_LIBRARY)
	include_directories(${GLFW_LIBRARY}/include)
endif()

# I used this for allowing CMake to include all the files,

# also new ones, without having to touch the CMakeLists.txt

file(GLOB_RECURSE PROJECT_SOURCE_FILES "*.h" "*.hpp" "*.cpp")

# I then wrote this Macro, which organize the files in visual studio folders,

# according to the organization in the file systems

macro(GroupSources curdir)

   file(GLOB children RELATIVE ${PROJECT_SOURCE_DIR}/${curdir}
${PROJECT_SOURCE_DIR}/${curdir}/*)

   foreach(child ${children})

          if(IS_DIRECTORY ${PROJECT_SOURCE_DIR}/${curdir}/${child})

                  GroupSources(${curdir}/${child})

          else()

                  string(REPLACE "/" "\\" groupname ${curdir})

				  # I would like to call the src root folder in a different name, only in visual studio (not mandatory requirement)

                   string(REPLACE "src" "Common" groupname ${groupname})

            source_group(${groupname} FILES
${PROJECT_SOURCE_DIR}/${curdir}/${child})

          endif()

   endforeach()

endmacro()



# Execute the macro

GroupSources(src)

add_library(${PROJECT_NAME} ${PROJECT_SOURCE_FILES})