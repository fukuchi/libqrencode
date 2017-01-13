cmake_minimum_required(VERSION 3.1.0)

project(QREncodingLibrary VERSION 4.0.0 LANGUAGES C)


set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")

option(GPROF "Generate extra code to write profile information" OFF)
if(GPROF)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pg")
endif()

option(COVERAGE "Generate extra code to write coverage information" OFF)
if(COVERAGE)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --coverage")
endif()

option(ASAN "Use AddressSanitizer" OFF)
if(ASAN)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=address -fno-omit-frame-pointer -fno-optimize-sibling-calls")
endif()

add_definitions(-DMAJOR_VERSION=${PROJECT_VERSION_MAJOR})
add_definitions(-DMINOR_VERSION=${PROJECT_VERSION_MINOR})
add_definitions(-DMICRO_VERSION=${PROJECT_VERSION_PATCH})
add_definitions(-DVERSION="${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}")
add_definitions(-DSTATIC_IN_RELEASE=)


set(LIB_FILES qrencode.c
              qrinput.c
              bitstream.c
              qrspec.c
              rsecc.c
              split.c
              mask.c
              mqrspec.c
              mmask.c)
add_library(qrencode ${LIB_FILES})
option(BUILD_SHARED_LIBS "Enable build of shared libraries")


set(EXE_FILES qrenc.c)
add_executable(qrenc ${EXE_FILES} ${PNG_LIBRARIES})
set_target_properties(qrenc PROPERTIES OUTPUT_NAME "qrencode")
target_link_libraries(qrenc qrencode)


add_definitions(-DHAVE_SDL=0)

find_package(PNG)
if(PNG_FOUND)
    add_definitions(-DHAVE_PNG=1)
    if(TARGET PNG::PNG)
        target_link_libraries(qrenc PNG::PNG)
    else()
        TARGET_INCLUDE_DIRECTORIES(qrenc SYSTEM PUBLIC ${PNG_INCLUDE_DIRS})
        target_link_libraries(qrenc ${PNG_LIBRARIES})
        add_definitions(${PNG_DEFINITIONS})
    endif()
endif(PNG_FOUND)

install(TARGETS qrenc DESTINATION bin)
configure_file(qrencode.1.in qrencode.1 @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/qrencode.1 DESTINATION share/man/man1)
if(BUILD_SHARED_LIBS)
    configure_file(libqrencode.pc.in libqrencode.pc @ONLY)
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/libqrencode.pc DESTINATION lib/pkgconfig)
    install(TARGETS qrencode DESTINATION lib)
    install(FILES qrencode.h DESTINATION include)
endif(BUILD_SHARED_LIBS)


if(BUILD_TESTING)
    enable_testing()
    add_definitions(-DWITH_TESTS=)
    add_subdirectory(tests)
endif(BUILD_TESTING)
