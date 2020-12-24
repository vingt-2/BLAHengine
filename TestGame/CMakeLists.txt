file(GLOB_RECURSE EDITOR_SOURCE_FILES 
${CMAKE_CURRENT_SOURCE_DIR}/*.h 
${CMAKE_CURRENT_SOURCE_DIR}/*.c
${CMAKE_CURRENT_SOURCE_DIR}/*.hpp 
${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

GroupSources(./)

GenerateProjectDefinitionFile(BLATestGame)

include_directories(
${COMMON_INCLUDE_DIR}
${GLEW_INCLUDE_DIR} # TODO: Remove dependencies on glew here ...
${CMAKE_CURRENT_SOURCE_DIR} 
${GLFW_INCLUDE_DIR} 
${glm_INCLUDE_DIR}
${VulkanHeaders_INCLUDE_DIR}
${PROJECT_SOURCE_DIR}/Engine
)

add_executable(BLATestGame ${EDITOR_SOURCE_FILES})

target_link_libraries(BLATestGame BLAEngine)

add_custom_command ( TARGET BLATestGame POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    $<TARGET_FILE:BLAEngine> $<TARGET_FILE_DIR:BLATestGame>
)

#TODO: It'd be great to take that down to 11 ... (to of my mind we need to get rid of std::apply)
set_property(TARGET BLATestGame PROPERTY CXX_STANDARD 17)

if(WIN32)
	set_property(TARGET BLATestGame PROPERTY VS_DEBUGGER_WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS")
endif()