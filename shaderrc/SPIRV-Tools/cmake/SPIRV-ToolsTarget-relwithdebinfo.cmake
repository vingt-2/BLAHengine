#----------------------------------------------------------------
# Generated CMake target import file for configuration "RelWithDebInfo".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "SPIRV-Tools" for configuration "RelWithDebInfo"
set_property(TARGET SPIRV-Tools APPEND PROPERTY IMPORTED_CONFIGURATIONS RELWITHDEBINFO)
set_target_properties(SPIRV-Tools PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELWITHDEBINFO "CXX"
  IMPORTED_LOCATION_RELWITHDEBINFO "${_IMPORT_PREFIX}/lib/SPIRV-Tools.lib"
  )

list(APPEND _IMPORT_CHECK_TARGETS SPIRV-Tools )
list(APPEND _IMPORT_CHECK_FILES_FOR_SPIRV-Tools "${_IMPORT_PREFIX}/lib/SPIRV-Tools.lib" )

# Import target "SPIRV-Tools-shared" for configuration "RelWithDebInfo"
set_property(TARGET SPIRV-Tools-shared APPEND PROPERTY IMPORTED_CONFIGURATIONS RELWITHDEBINFO)
set_target_properties(SPIRV-Tools-shared PROPERTIES
  IMPORTED_IMPLIB_RELWITHDEBINFO "${_IMPORT_PREFIX}/lib/SPIRV-Tools-shared.lib"
  IMPORTED_LOCATION_RELWITHDEBINFO "${_IMPORT_PREFIX}/bin/SPIRV-Tools-shared.dll"
  )

list(APPEND _IMPORT_CHECK_TARGETS SPIRV-Tools-shared )
list(APPEND _IMPORT_CHECK_FILES_FOR_SPIRV-Tools-shared "${_IMPORT_PREFIX}/lib/SPIRV-Tools-shared.lib" "${_IMPORT_PREFIX}/bin/SPIRV-Tools-shared.dll" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
