IF(NOT PNG_FOUND)
    MESSAGE(FATAL_ERROR "PNG is required for tests")
ENDIF()


SET(TEST_FILES test_bitstream.c
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

SET(TEST_COMMON_FILES common.c
                      decoder.c
                      datachunk.c
                      rsecc_decoder.c
                      rscode.c)


ADD_EXECUTABLE(create_frame_pattern create_frame_pattern.c ${TEST_COMMON_FILES})
ADD_EXECUTABLE(create_mqr_frame_pattern create_mqr_frame_pattern.c ${TEST_COMMON_FILES})
TARGET_LINK_LIBRARIES(create_frame_pattern qrencode)
TARGET_LINK_LIBRARIES(create_mqr_frame_pattern qrencode)

IF(TARGET PNG::PNG)
    TARGET_LINK_LIBRARIES(create_frame_pattern PNG::PNG)
    TARGET_LINK_LIBRARIES(create_mqr_frame_pattern PNG::PNG)
ELSE()
    TARGET_INCLUDE_DIRECTORIES(create_frame_pattern SYSTEM PUBLIC ${PNG_INCLUDE_DIRS})
    TARGET_LINK_LIBRARIES(create_frame_pattern ${PNG_LIBRARIES})

    TARGET_INCLUDE_DIRECTORIES(create_mqr_frame_pattern SYSTEM PUBLIC ${PNG_INCLUDE_DIRS})
    TARGET_LINK_LIBRARIES(create_mqr_frame_pattern ${PNG_LIBRARIES})
ENDIF()



FOREACH(testfile ${TEST_FILES})
    SET(binary ${testfile}.bin)
    ADD_EXECUTABLE(${binary} ${testfile} ${TEST_COMMON_FILES})
    TARGET_LINK_LIBRARIES(${binary} qrencode)
    ADD_TEST(${testfile} ${binary})
ENDFOREACH()

