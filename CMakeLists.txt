CMAKE_MINIMUM_REQUIRED(VERSION 3.1.0)

PROJECT(QREncodingLibrary VERSION 4.0.0 LANGUAGES C)


SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")

OPTION(GPROF "Generate extra code to write profile information" OFF)
IF(GPROF)
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pg")
ENDIF()

OPTION(COVERAGE "Generate extra code to write coverage information" OFF)
IF(COVERAGE)
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --coverage")
ENDIF()

OPTION(ASAN "Use AddressSanitizer" OFF)
IF(ASAN)
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=address -fno-omit-frame-pointer -fno-optimize-sibling-calls")
ENDIF()

ADD_DEFINITIONS(-DMAJOR_VERSION=${PROJECT_VERSION_MAJOR})
ADD_DEFINITIONS(-DMINOR_VERSION=${PROJECT_VERSION_MINOR})
ADD_DEFINITIONS(-DMICRO_VERSION=${PROJECT_VERSION_PATCH})
ADD_DEFINITIONS(-DVERSION="${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}")
ADD_DEFINITIONS(-D__STATIC=)


SET(LIB_FILES qrencode.c
              qrinput.c
              bitstream.c
              qrspec.c
              rsecc.c
              split.c
              mask.c
              mqrspec.c
              mmask.c)
ADD_LIBRARY(qrencode ${LIB_FILES})
OPTION(BUILD_SHARED_LIBS "Enable build of shared libraries")


SET(EXE_FILES qrenc.c)
ADD_EXECUTABLE(qrenc ${EXE_FILES} ${PNG_LIBRARIES})
SET_TARGET_PROPERTIES(qrenc PROPERTIES OUTPUT_NAME "qrencode")
TARGET_LINK_LIBRARIES(qrenc qrencode)


FIND_PACKAGE(PNG)
IF(PNG_FOUND)
    ADD_DEFINITIONS(-DHAVE_PNG=1)
    IF(TARGET PNG::PNG)
        TARGET_LINK_LIBRARIES(qrenc PNG::PNG)
    ELSE()
        TARGET_INCLUDE_DIRECTORIES(qrenc SYSTEM PUBLIC ${PNG_INCLUDE_DIRS})
        TARGET_LINK_LIBRARIES(qrenc ${PNG_LIBRARIES})
        ADD_DEFINITIONS(${PNG_DEFINITIONS})
    ENDIF()
ENDIF()

INSTALL(TARGETS qrenc DESTINATION bin)
CONFIGURE_FILE(qrencode.1.in qrencode.1 @ONLY)
INSTALL(FILES qrencode.1 DESTINATION share/man/man1)
IF(BUILD_SHARED_LIBS)
    CONFIGURE_FILE(libqrencode.pc.in libqrencode.pc @ONLY)
    INSTALL(FILES libqrencode.pc DESTINATION lib/pkgconfig)
    INSTALL(TARGETS qrencode DESTINATION lib)
    INSTALL(FILES qrencode.h DESTINATION include)
ENDIF()

