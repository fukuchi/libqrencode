include(CheckFunctionExists)

set(_Iconv_SEARCHES)

# Search Iconv_DIR first if it is set.
if(NOT Iconv_DIR AND ENV{Iconv_DIR})
  set(Iconv_DIR $ENV{Iconv_DIR})
endif()

if(Iconv_DIR)
  set(_Iconv_DIR_SEARCH PATHS ${Iconv_DIR} NO_DEFAULT_PATH)
  list(APPEND _Iconv_SEARCHES _Iconv_DIR_SEARCH)
endif()

# Normal search.
set(_Iconv_SEARCH_NORMAL
    PATHS "[HKEY_LOCAL_MACHINE\\SOFTWARE\\GnuWin32\\Iconv;InstallPath]"
          "$ENV{PROGRAMFILES}/iconv"
          ENV CPATH
          ENV C_INCLUDE_PATH
          ENV CPLUS_INCLUDE_PATH
          ENV LIBRARY_PATH)
list(APPEND _Iconv_SEARCHES _Iconv_SEARCH_NORMAL)

set(Iconv_NAMES iconv iconv2 libiconv iconv64)
set(Iconv_NAMES_DEBUG iconvd iconv64d)

# Try each search configuration.
foreach(search ${_Iconv_SEARCHES})
    find_path(Iconv_INCLUDE_DIR NAMES iconv.h ${${search}} PATH_SUFFIXES include)
endforeach()

# Allow Iconv_LIBRARY to be set manually, as the location of the iconv library
if(NOT Iconv_LIBRARY)
    foreach(search ${_Iconv_SEARCHES})
        find_library(Iconv_LIBRARY_RELEASE NAMES ${Iconv_NAMES} ${${search}} PATH_SUFFIXES lib)
        find_library(Iconv_LIBRARY_DEBUG   NAMES ${Iconv_NAMES_DEBUG} ${${search}} PATH_SUFFIXES lib)
    endforeach()

    include(SelectLibraryConfigurations)
    select_library_configurations(Iconv)
endif()

unset(Iconv_NAMES)
unset(Iconv_NAMES_DEBUG)

if(Iconv_INCLUDE_DIR AND EXISTS "${Iconv_INCLUDE_DIR}/iconv.h")
    file(STRINGS "${Iconv_INCLUDE_DIR}/iconv.h" Iconv_H REGEX "^#define _LIBIconv_VERSION 0x([0-9]+)")
    string(REGEX MATCH "q#define _LIBIconv_VERSION 0x([0-9][0-9])([0-9][0-9])?([0-9][0-9])?.*" temp_match "${Iconv_H}")
    unset(temp_match)
    if(CMAKE_MATCH_0)
        set(Iconv_VERSION_MAJOR "${CMAKE_MATCH_1}")
        set(Iconv_VERSION_MINOR "${CMAKE_MATCH_2}")
        set(Iconv_VERSION_PATCH "${CMAKE_MATCH_3}")
        string(REGEX REPLACE "0*([1-9][0-9]*).*" "\\1" Iconv_VERSION_MAJOR "${Iconv_VERSION_MAJOR}")
        string(REGEX REPLACE "0*([1-9][0-9]*).*" "\\1" Iconv_VERSION_MINOR "${Iconv_VERSION_MINOR}")
        string(REGEX REPLACE "0*([1-9][0-9]*).*" "\\1" Iconv_VERSION_PATCH "${Iconv_VERSION_PATCH}")

        set(Iconv_VERSION_STRING "${Iconv_VERSION_MAJOR}.${Iconv_VERSION_MINOR}")
        if(Iconv_VERSION_PATCH)
            set(Iconv_VERSION_STRING "${Iconv_VERSION_STRING}.${Iconv_VERSION_PATCH}")
        endif()
    endif()
endif()

check_function_exists(iconv_open Iconv_IN_GLIBC)

set(Iconv_FOUND_ANY FALSE)
if(Iconv_IN_GLIBC OR Iconv_LIBRARY)
    set(Iconv_FOUND_ANY TRUE)
endif()

# handle the QUIETLY and REQUIRED arguments and set Iconv_FOUND to TRUE if
# all listed variables are TRUE
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Iconv
    REQUIRED_VARS Iconv_FOUND_ANY Iconv_INCLUDE_DIR
    VERSION_VAR Iconv_VERSION_STRING)

mark_as_advanced(Iconv_LIBRARY Iconv_INCLUDE_DIR)

if(NOT Iconv_FOUND)
    return()
endif()

set(Iconv_INCLUDE_DIRS ${Iconv_INCLUDE_DIR})

if(NOT Iconv_LIBRARIES)
    set(Iconv_LIBRARIES ${Iconv_LIBRARY})
endif()

if(Iconv_LIBRARY AND NOT TARGET Iconv::Iconv)
    add_library(Iconv::Iconv UNKNOWN IMPORTED)
    set_target_properties(Iconv::Iconv PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${Iconv_INCLUDE_DIRS}"
        IMPORTED_LOCATION "${Iconv_LIBRARY}")

    if(Iconv_LIBRARY_RELEASE)
        set_property(TARGET Iconv::Iconv APPEND PROPERTY
            IMPORTED_CONFIGURATIONS RELEASE)
        set_target_properties(Iconv::Iconv PROPERTIES
            IMPORTED_LOCATION_RELEASE "${Iconv_LIBRARY_RELEASE}")
    endif()

    if(Iconv_LIBRARY_DEBUG)
        set_property(TARGET Iconv::Iconv APPEND PROPERTY
            IMPORTED_CONFIGURATIONS DEBUG)
        set_target_properties(Iconv::Iconv PROPERTIES
            IMPORTED_LOCATION_DEBUG "${Iconv_LIBRARY_DEBUG}")
    endif()
elseif(NOT TARGET Iconv::Iconv)
    add_library(Iconv::Iconv INTERFACE IMPORTED)
    set_target_properties(Iconv::Iconv PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${Iconv_INCLUDE_DIRS}")
endif()
