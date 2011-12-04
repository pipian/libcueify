INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/../include)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/../src)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR}/../include)
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/../include/cueify/types.h.in
               ${CMAKE_CURRENT_BINARY_DIR}/../include/cueify/types.h)
LINK_LIBRARIES(cueify)

ENABLE_TESTING()

ADD_CUSTOM_TARGET(check COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure)

FIND_PACKAGE(LibCheck)
IF(LIBCHECK_FOUND)
    INCLUDE_DIRECTORIES(${LIBCHECK_INCLUDE_DIRS})
    LINK_LIBRARIES(${LIBCHECK_LIBRARIES})
    
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

    ADD_CUSTOM_TARGET(check-pregaps
		      COMMAND ${CMAKE_CURRENT_BINARY_DIR}/check_pregaps)
    ADD_EXECUTABLE(check_pregaps check_pregaps.c)
    ADD_DEPENDENCIES(check check_pregaps)
    ADD_DEPENDENCIES(check-pregaps check_pregaps)

    ADD_CUSTOM_TARGET(check-track-control
		      COMMAND ${CMAKE_CURRENT_BINARY_DIR}/check_track_control)
    ADD_EXECUTABLE(check_track_control check_track_control.c)
    ADD_DEPENDENCIES(check check_track_control)
    ADD_DEPENDENCIES(check-track-control check_track_control)
ENDIF(LIBCHECK_FOUND)

FIND_PACKAGE(SWIG)
IF(SWIG_FOUND)
FIND_PACKAGE(PythonLibs)
IF(PYTHONLIBS_FOUND)
FIND_PACKAGE(PythonInterp)
IF(PYTHONINTERP_FOUND)
    ADD_TEST(check_toc.py ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/swig/check_toc.py)
    ADD_TEST(check_sessions.py ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/swig/check_sessions.py)
    ADD_TEST(check_full_toc.py ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/swig/check_full_toc.py)
    ADD_TEST(check_cdtext.py ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/swig/check_cdtext.py)
    ADD_DEPENDENCIES(check _python-cueify)

    ADD_CUSTOM_TARGET(check_unportable.py
		      COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/swig/check_unportable.py)
    ADD_DEPENDENCIES(check_unportable.py _python-cueify)
    ADD_DEPENDENCIES(check-unportable check_unportable.py)

    ADD_CUSTOM_TARGET(check_indices.py
		      COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/swig/check_indices.py)
    ADD_DEPENDENCIES(check_indices.py _python-cueify)
    ADD_DEPENDENCIES(check-indices check_indices.py)

    ADD_CUSTOM_TARGET(check_pregaps.py
		      COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/swig/check_pregaps.py)
    ADD_DEPENDENCIES(check_pregaps.py _python-cueify)
    ADD_DEPENDENCIES(check-pregaps check_pregaps.py)

    ADD_CUSTOM_TARGET(check_track_control.py
		      COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/swig/check_track_control.py)
    ADD_DEPENDENCIES(check_track_control.py _python-cueify)
    ADD_DEPENDENCIES(check-track-control check_track_control.py)
ENDIF(PYTHONINTERP_FOUND)
ENDIF(PYTHONLIBS_FOUND)
ENDIF(SWIG_FOUND)

IF(CMAKE_COMPILER_IS_GNUCC)
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror -Wextra")
ENDIF(CMAKE_COMPILER_IS_GNUCC)

# Windows requires the DLLs to be in the same directory.
IF(CYGWIN)
    GET_PROPERTY(cueify_LOCATION TARGET cueify PROPERTY LOCATION)
    GET_FILENAME_COMPONENT(cueify_BASENAME ${cueify_LOCATION} NAME)
    ADD_CUSTOM_TARGET(copy-test-library
                      COMMAND ${CMAKE_COMMAND} -E copy ${cueify_LOCATION} ${CMAKE_CURRENT_BINARY_DIR}/${cueify_BASENAME}
                      DEPENDS cueify)
    ADD_DEPENDENCIES(check copy-test-library)
    ADD_DEPENDENCIES(check-unportable copy-test-library)
    ADD_DEPENDENCIES(check-indices copy-test-library)
    ADD_DEPENDENCIES(check-pregaps copy-test-library)
    ADD_DEPENDENCIES(check-track-control copy-test-library)
ENDIF(CYGWIN)

