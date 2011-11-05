INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/../include)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/../src)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR}/../include)
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/../include/libcueify/types.h.in
               ${CMAKE_CURRENT_BINARY_DIR}/../include/libcueify/types.h)
LINK_LIBRARIES(cueify)

ENABLE_TESTING()

FIND_PACKAGE(LibCheck)
IF(LIBCHECK_FOUND)
    INCLUDE_DIRECTORIES(${LIBCHECK_INCLUDE_DIRS})
    LINK_LIBRARIES(${LIBCHECK_LIBRARIES})
    
    ADD_CUSTOM_TARGET(check COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure)
    
    ADD_EXECUTABLE(check_toc check_toc.c)
    ADD_TEST(check_toc check_toc)
    ADD_DEPENDENCIES(check check_toc)
    
    ADD_EXECUTABLE(check_sessions check_sessions.c)
    ADD_TEST(check_sessions check_sessions)
    ADD_DEPENDENCIES(check check_sessions)
    
    ADD_EXECUTABLE(check_full_toc check_full_toc.c)
    ADD_TEST(check_full_toc check_full_toc)
    ADD_DEPENDENCIES(check check_full_toc)
    
    ADD_EXECUTABLE(check_cdtext check_cdtext.c)
    ADD_TEST(check_cdtext check_cdtext)
    ADD_DEPENDENCIES(check check_cdtext)
    
    ADD_CUSTOM_TARGET(check-unportable
		      COMMAND ${CMAKE_CURRENT_BINARY_DIR}/check_unportable)
    ADD_EXECUTABLE(check_unportable check_unportable.c)
    ADD_DEPENDENCIES(check check_unportable)
    ADD_DEPENDENCIES(check-unportable check_unportable)

    ADD_CUSTOM_TARGET(check-indices
		      COMMAND ${CMAKE_CURRENT_BINARY_DIR}/check_indices)
    ADD_EXECUTABLE(check_indices check_indices.c)
    ADD_DEPENDENCIES(check check_indices)
    ADD_DEPENDENCIES(check-indices check_indices)
ENDIF(LIBCHECK_FOUND)

IF(CMAKE_COMPILER_IS_GNUCC)
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror -Wextra")
ENDIF(CMAKE_COMPILER_IS_GNUCC)
