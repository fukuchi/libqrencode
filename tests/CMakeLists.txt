if(ICONV_FOUND)
    include_directories(${ICONV_INCLUDES})
else(ICONV_FOUND)
    message(FATAL_ERROR "Iconv is required for tests")
endif(ICONV_FOUND)

if(PNG_FOUND)
    include_directories(${PNG_INCLUDE_DIR})
else(PNG_FOUND)
    message(FATAL_ERROR "PNG is required for tests")
endif(PNG_FOUND)

set(TEST_FILES test_bitstream.c
               test_estimatebit.c
               test_mask.c
               test_mmask.c
               test_monkey.c
               test_mqrspec.c
               test_qrencode.c
               test_qrinput.c
               test_qrspec.c
               test_rs.c
               test_split.c
               test_split_urls.c)

set(TEST_COMMON_FILES common.c
                      decoder.c
                      datachunk.c
                      rsecc_decoder.c
                      rscode.c)


add_executable(create_frame_pattern create_frame_pattern.c ${TEST_COMMON_FILES})
add_executable(create_mqr_frame_pattern create_mqr_frame_pattern.c ${TEST_COMMON_FILES})
target_link_libraries(create_frame_pattern qrencode)
target_link_libraries(create_mqr_frame_pattern qrencode)

if(TARGET PNG::PNG)
    target_link_libraries(create_frame_pattern PNG::PNG)
    target_link_libraries(create_mqr_frame_pattern PNG::PNG)
else()
    target_include_directories(create_frame_pattern SYSTEM PUBLIC ${PNG_INCLUDE_DIRS})
    target_link_libraries(create_frame_pattern ${PNG_LIBRARIES})

    target_include_directories(create_mqr_frame_pattern SYSTEM PUBLIC ${PNG_INCLUDE_DIRS})
    target_link_libraries(create_mqr_frame_pattern ${PNG_LIBRARIES})
endif()



foreach(testfile ${TEST_FILES})
    set(binary ${testfile}.bin)
    add_executable(${binary} ${testfile} ${TEST_COMMON_FILES})
    target_link_libraries(${binary} qrencode)
    add_test(${testfile} ${binary})
endforeach()

