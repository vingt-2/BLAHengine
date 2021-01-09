cmake_minimum_required(VERSION 3.1)

project(BLAEngine CXX)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake/Modules/")

#set the default path for built executables to the "bin" directory

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

#set the default path for built libraries to the "lib" directory

set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)


set(PROJECT_EXTERNAL_DIR ${PROJECT_SOURCE_DIR}/external)


set(COMMON_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/Common)

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

# Set the msvc runtime library as Non DLL !
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

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

find_path(VulkanHeaders_PATH vulkan.h ${PROJECT_EXTERNAL_DIR}/Vulkan-Headers/include/vulkan)

if(NOT VulkanHeaders_PATH OR (NOT EXISTS ${VulkanHeaders_PATH}))
    # we couldn't find the header files for VulkanHeaders or they don't exist
    message("Unable to find Vulkan-Headers, fetching the git submodule...")

    GetHeaderDependency(Vulkan-Headers)
	
	find_path(VulkanHeaders_PATH vulkan.h ${PROJECT_EXTERNAL_DIR}/Vulkan-Headers/include/vulkan)
	
	if(NOT VulkanHeaders_PATH)
		message("Vulkan-Headers submodule failed to clone")
	endif()
endif()

set(VulkanHeaders_INCLUDE_DIR ${PROJECT_EXTERNAL_DIR}/Vulkan-Headers/include CACHE PATH "Vulkan Headers include directory")
set(VulkanRegistry_DIR ${PROJECT_EXTERNAL_DIR}/Vulkan-Headers/registry CACHE PATH "Vulkan Registry directory")

GetCMakeDependency(Vulkan-Loader)
set(VULKAN_LOADER_LIBRARY ${PROJECT_EXTERNAL_DIR}/Vulkan-Loader CACHE PATH "Vulkan-Loader directory")
add_subdirectory(${VULKAN_LOADER_LIBRARY}) 

GetHeaderDependency(SPIRV-Headers)
set(SPIRV-Headers_SOURCE_DIR ${PROJECT_EXTERNAL_DIR}/SPIRV-Headers CACHE PATH "SPIRV-Headers directory")

GetCMakeDependency(SPIRV-Tools)
set(SPIRVTOOLS_LIBRARY ${PROJECT_EXTERNAL_DIR}/SPIRV-Tools CACHE PATH "SPIRV-Tools directory")
set(SPIRV_SKIP_TESTS ON)
set(SKIP_SPIRV_TOOLS_INSTALL ON)
set(SPIRV_SKIP_EXECUTABLES ON)
add_subdirectory(${SPIRVTOOLS_LIBRARY})

GetCMakeDependency(glslang)
set(GLSLANG_LIBRARY ${PROJECT_EXTERNAL_DIR}/glslang CACHE PATH "glslang directory")
set(SKIP_GLSLANG_INSTALL ON)
set(ENABLE_SPVREMAPPER OFF)
set(ENABLE_CTEST OFF)
add_subdirectory(${GLSLANG_LIBRARY})

GetCMakeDependency(shaderc)
set(SHADERC_LIBRARY ${PROJECT_EXTERNAL_DIR}/shaderc CACHE PATH "shaderc directory")
set(SHADERC_SKIP_INSTALL ON)
set(SHADERC_ENABLE_SPVC OFF)
set(SHADERC_SKIP_TESTS ON)
set(SHADERC_ENABLE_SHARED_CRT OFF)
add_subdirectory(${SHADERC_LIBRARY})

link_directories(${LIBRARY_OUTPUT_PATH})

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

macro(GenerateProjectDefinitionFile projectName)
file(WRITE GeneratedProjectDefinition.h
     "// This file is generated by the build system. It should not be modified as it reflects the name of the current project.// Every BLAEngine project needs one of these, and you shouldn't make it yourself\n\n#define BLA_PROJECT_NAME ${projectName} \n#define __SILLY_MACRO__COMPARE_${projectName}(x) x")

if(MSVC)
    add_definitions(/FI"${CMAKE_CURRENT_SOURCE_DIR}/GeneratedProjectDefinition.h")
else()
    # GCC or Clang
    add_definitions(-include ${CMAKE_CURRENT_SOURCE_DIR}/GeneratedProjectDefinition.h)
endif()
endmacro()

if ( CMAKE_COMPILER_IS_GNUCC )
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wall -Wextra")
endif()
if ( MSVC )
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} /WX /wd\"4251\" /wd\"4003\"")
endif()

add_subdirectory(./Engine)
add_subdirectory(./Demos)
add_subdirectory(./Editor)
#add_subdirectory(./TestGame)