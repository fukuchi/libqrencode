include(CheckFunctionExists)

set(_ICONV_SEARCHES)

# Search ICONV_DIR first if it is set.
if(NOT ICONV_DIR AND ENV{ICONV_DIR})
  set(ICONV_DIR $ENV{ICONV_DIR})
endif()

if(ICONV_DIR)
  set(_ICONV_DIR_SEARCH PATHS ${ICONV_DIR} NO_DEFAULT_PATH)
  list(APPEND _ICONV_SEARCHES _ICONV_DIR_SEARCH)
endif()

# Normal search.
set(_ICONV_SEARCH_NORMAL
  PATHS "[HKEY_LOCAL_MACHINE\\SOFTWARE\\GnuWin32\\Iconv;InstallPath]"
        "$ENV{PROGRAMFILES}/iconv"
  )
list(APPEND _ICONV_SEARCHES _ICONV_SEARCH_NORMAL)

set(ICONV_NAMES iconv iconv2 libiconv iconv64)
set(ICONV_NAMES_DEBUG iconvd iconv64d)

# Try each search configuration.
foreach(search ${_ICONV_SEARCHES})
  find_path(ICONV_INCLUDE_DIR NAMES iconv.h ${${search}} PATH_SUFFIXES include)
endforeach()

# Allow ICONV_LIBRARY to be set manually, as the location of the iconv library
if(NOT ICONV_LIBRARY)
  foreach(search ${_ICONV_SEARCHES})
    find_library(ICONV_LIBRARY_RELEASE NAMES ${ICONV_NAMES} ${${search}} PATH_SUFFIXES lib)
    find_library(ICONV_LIBRARY_DEBUG   NAMES ${ICONV_NAMES_DEBUG} ${${search}} PATH_SUFFIXES lib)
  endforeach()

  include(SelectLibraryConfigurations)
  select_library_configurations(ICONV)
endif()

unset(ICONV_NAMES)
unset(ICONV_NAMES_DEBUG)

mark_as_advanced(ICONV_LIBRARY ICONV_INCLUDE_DIR)

if(ICONV_INCLUDE_DIR AND EXISTS "${ICONV_INCLUDE_DIR}/iconv.h")
    file(STRINGS "${ICONV_INCLUDE_DIR}/iconv.h" ICONV_H REGEX "^#define _LIBICONV_VERSION 0x([0-9]+)")

    string(REGEX MATCH "#define _LIBICONV_VERSION 0x([0-9][0-9])([0-9][0-9])?([0-9][0-9])?.*" temp_match "${ICONV_H}")
    unset(temp_match)
    set(ICONV_VERSION_MAJOR "${CMAKE_MATCH_1}")
    set(ICONV_VERSION_MINOR "${CMAKE_MATCH_2}")
    set(ICONV_VERSION_PATCH "${CMAKE_MATCH_3}")
    string(REGEX REPLACE "0*([1-9][0-9]*).*" "\\1" ICONV_VERSION_MAJOR "${ICONV_VERSION_MAJOR}")
    string(REGEX REPLACE "0*([1-9][0-9]*).*" "\\1" ICONV_VERSION_MINOR "${ICONV_VERSION_MINOR}")
    string(REGEX REPLACE "0*([1-9][0-9]*).*" "\\1" ICONV_VERSION_PATCH "${ICONV_VERSION_PATCH}")
    
    set(ICONV_VERSION_STRING "${ICONV_VERSION_MAJOR}.${ICONV_VERSION_MINOR}")
    if(ICONV_VERSION_PATCH)
        set(ICONV_VERSION_STRING "${ICONV_VERSION_STRING}.${ICONV_VERSION_PATCH}")
    endif()
endif()

check_function_exists(iconv_open ICONV_IN_GLIBC)

if(ICONV_IN_GLIBC)
    set(ICONV_TEST "In glibc")
else()
    set(ICONV_TEST ${ICONV_LIBRARY})
endif()

# handle the QUIETLY and REQUIRED arguments and set ICONV_FOUND to TRUE if
# all listed variables are TRUE
include(FindPackageHandleStandardArgs)
if(ICONV_LIBRARY)
    find_package_handle_standard_args(ICONV
        REQUIRED_VARS ICONV_LIBRARY ICONV_INCLUDE_DIR
        VERSION_VAR ICONV_VERSION_STRING)
elseif(ICONV_IN_GLIBC)
    find_package_handle_standard_args(ICONV
        REQUIRED_VARS ICONV_INCLUDE_DIR
        VERSION_VAR ICONV_VERSION_STRING)
endif()

if(NOT ICONV_FOUND)
    return()
endif()

set(ICONV_INCLUDE_DIRS ${ICONV_INCLUDE_DIR})

if(NOT ICONV_LIBRARIES)
    set(ICONV_LIBRARIES ${ICONV_LIBRARY})
endif()

if(NOT TARGET ICONV::ICONV)
    add_library(ICONV::ICONV UNKNOWN IMPORTED)
    set_target_properties(ICONV::ICONV PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${ICONV_INCLUDE_DIRS}")

    if(ICONV_LIBRARY_RELEASE)
        set_property(TARGET ICONV::ICONV APPEND PROPERTY
            IMPORTED_CONFIGURATIONS RELEASE)
        set_target_properties(ICONV::ICONV PROPERTIES
            IMPORTED_LOCATION_RELEASE "${ICONV_LIBRARY_RELEASE}")
    endif()

    if(ICONV_LIBRARY_DEBUG)
        set_property(TARGET ICONV::ICONV APPEND PROPERTY
            IMPORTED_CONFIGURATIONS DEBUG)
        set_target_properties(ICONV::ICONV PROPERTIES
            IMPORTED_LOCATION_DEBUG "${ICONV_LIBRARY_DEBUG}")
    endif()

    if(NOT ICONV_LIBRARY_RELEASE AND NOT ICONV_LIBRARY_DEBUG)
        set_property(TARGET ICONV::ICONV APPEND PROPERTY
            IMPORTED_LOCATION "${ICONV_LIBRARY}")
    endif()
endif()
