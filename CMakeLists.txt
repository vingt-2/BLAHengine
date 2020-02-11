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

find_path(VulkanHeaders_PATH vulkan.h ${PROJECT_EXTERNAL_DIR}/Vulkan-Headers/include/vulkan)

if(NOT VulkanHeaders_PATH OR (NOT EXISTS ${VulkanHeaders_PATH}))
    # we couldn't find the header files for GLM or they don't exist
    message("Unable to find Vulkan-Headers, fetching the git submodule...")

    GetHeaderDependency(Vulkan-Headers)
	
	find_path(VulkanHeaders_PATH vulkan.h ${PROJECT_EXTERNAL_DIR}/Vulkan-Headers/include/vulkan)
	
	if(NOT VulkanHeaders_PATH)
		message("Vulkan-Headers submodule failed to clone")
	endif()
endif()

set(VulkanHeaders_INCLUDE_DIR ${PROJECT_EXTERNAL_DIR}/Vulkan-Headers/include CACHE PATH "Vulkan Headers directory")
set(VulkanRegistry_DIR ${PROJECT_EXTERNAL_DIR}/Vulkan-Headers/registry CACHE PATH "Vulkan Registry directory")

GetCMakeDependency(Vulkan-Loader)
set(VULKAN_LOADER_LIBRARY ${PROJECT_EXTERNAL_DIR}/Vulkan-Loader CACHE PATH "Vulkan-Loader directory")
add_subdirectory(${VULKAN_LOADER_LIBRARY}) 

link_directories(${LIBRARY_OUTPUT_PATH})

add_subdirectory(./Engine)
add_subdirectory(./Demos)
add_subdirectory(./Editor)