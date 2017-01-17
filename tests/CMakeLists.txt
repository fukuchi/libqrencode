set(TEST_COMMON_FILES common.c common.h)

macro(MAKE_TEST test_name)
    set(ADDITIONAL_LIBS "${ARGN}")
    add_executable(${test_name} ${test_name}.c ${TEST_COMMON_FILES})
    target_link_libraries(${test_name} qrencode ${ADDITIONAL_LIBS})
    add_test(${test_name} ${test_name})
endmacro(MAKE_TEST)

if(TARGET PNG::PNG)
    add_executable(create_frame_pattern create_frame_pattern.c ${TEST_COMMON_FILES})
    target_link_libraries(create_frame_pattern qrencode PNG::PNG)

    add_executable(create_mqr_frame_pattern create_mqr_frame_pattern.c ${TEST_COMMON_FILES})
    target_link_libraries(create_mqr_frame_pattern qrencode PNG::PNG)
endif()

if(HAVE_SDL)
    add_executable(view_qrcode view_qrcode.c ${TEST_COMMON_FILES})
    target_link_libraries(view_qrcode qrencode)
endif(HAVE_SDL)

if(TARGET Threads::Threads)
    add_executable(prof_qrencode prof_qrencode.c ${TEST_COMMON_FILES})
    target_link_libraries(prof_qrencode qrencode Threads::Threads)

    add_executable(pthread_qrencode pthread_qrencode.c ${TEST_COMMON_FILES})
    target_link_libraries(pthread_qrencode qrencode Threads::Threads)
endif()

MAKE_TEST(test_bitstream)
MAKE_TEST(test_estimatebit)
MAKE_TEST(test_split)

if(TARGET ICONV::ICONV)
    add_library(decoder 
        decoder.c decoder.h 
        datachunk.c datachunk.h 
        rsecc_decoder.c rsecc_decoder.h)
    target_link_libraries(decoder ICONV::ICONV)

    MAKE_TEST(test_qrinput decoder)
    MAKE_TEST(test_qrspec decoder)
    MAKE_TEST(test_mqrspec decoder)
    MAKE_TEST(test_qrencode decoder)
    MAKE_TEST(test_split_urls decoder)
    MAKE_TEST(test_monkey decoder)

    include(CheckCSourceCompiles)
    check_c_source_compiles(
        "int main(){
            const int w = 1;
            char buf[w];
            return 0;
        }"
        FIXED_SIZE_BUFFER_INITIALIZATION)
    
    if(FIXED_SIZE_BUFFER_INITIALIZATION)
        MAKE_TEST(test_mask decoder)
        MAKE_TEST(test_mmask decoder)
        MAKE_TEST(test_rs decoder)
    endif()
endif()

