#----------------------------------------------------------------
# Generated CMake target import file.
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "log4cxx" for configuration ""
set_property(TARGET log4cxx APPEND PROPERTY IMPORTED_CONFIGURATIONS NOCONFIG)
set_target_properties(log4cxx PROPERTIES
  IMPORTED_LOCATION_NOCONFIG "${_IMPORT_PREFIX}/lib/liblog4cxx.so.11.0.0"
  IMPORTED_SONAME_NOCONFIG "liblog4cxx.so.11"
  )

list(APPEND _IMPORT_CHECK_TARGETS log4cxx )
list(APPEND _IMPORT_CHECK_FILES_FOR_log4cxx "${_IMPORT_PREFIX}/lib/liblog4cxx.so.11.0.0" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
