INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/../include)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR}/../include)
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/../include/cueify/types.h.in
               ${CMAKE_CURRENT_BINARY_DIR}/../include/cueify/types.h)

FIND_PACKAGE(SWIG)
IF(SWIG_FOUND)
    INCLUDE(${SWIG_USE_FILE})
    SET_SOURCE_FILES_PROPERTIES(cueify.i PROPERTIES CPLUSPLUS ON)

    SET(CMAKE_SWIG_FLAGS "")
    SET(SWIG_SOURCE_FILE_FULLNAME "${CMAKE_CURRENT_SOURCE_DIR}/cueify.i")

    GET_DIRECTORY_PROPERTY(cmake_include_directories INCLUDE_DIRECTORIES)
    SET(swig_include_dirs)
    FOREACH(it ${cmake_include_directories})
    	SET(swig_include_dirs ${swig_include_dirs} "-I${it}")
    ENDFOREACH(it)

    FIND_PACKAGE(PythonLibs)
    IF(PYTHONLIBS_FOUND)
        # setup.py does this automagically.
#        ADD_CUSTOM_TARGET(python-cueify
#			  "${SWIG_EXECUTABLE}"
#    			  -python -c++
#			  ${CMAKE_SWIG_FLAGS}
#			  -outdir ${CMAKE_CURRENT_BINARY_DIR}
#    			  ${swig_include_dirs}
#			  -o cueifyPYTHON_wrap.cxx
#    			  "${SWIG_SOURCE_FILE_FULLNAME}")
#        ADD_DEPENDENCIES(bindings python-cueify)

	# Also, build the Python SWIG module internally for tests.
        INCLUDE_DIRECTORIES(${PYTHON_INCLUDE_PATH})
        SWIG_ADD_MODULE(python-cueify python cueify.i)
        SWIG_LINK_LIBRARIES(python-cueify cueify ${PYTHON_LIBRARIES})
        ADD_CUSTOM_COMMAND(TARGET _python-cueify POST_BUILD
		   	   COMMAND "${CMAKE_COMMAND}"
			   ARGS -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/python)
        ADD_CUSTOM_COMMAND(TARGET _python-cueify POST_BUILD
		   	   COMMAND "${CMAKE_COMMAND}"
			   ARGS -E rename ${CMAKE_CURRENT_BINARY_DIR}/cueify.py ${CMAKE_CURRENT_BINARY_DIR}/python/cueify.py)
        ADD_CUSTOM_COMMAND(TARGET _python-cueify POST_BUILD
		   	   COMMAND "${CMAKE_COMMAND}"
			   ARGS -E rename ${CMAKE_CURRENT_BINARY_DIR}/_python-cueify${CMAKE_SHARED_LIBRARY_SUFFIX} ${CMAKE_CURRENT_BINARY_DIR}/python/_cueify${CMAKE_SHARED_LIBRARY_SUFFIX})
    ENDIF(PYTHONLIBS_FOUND)

    FIND_PACKAGE(Ruby)
    IF(RUBY_FOUND)
        IF(${SWIG_VERSION} VERSION_LESS "2.0.1")
            MESSAGE(STATUS "WARNING: SWIG >= 2.0.1 is required for Ruby >= 1.9.0")
        ENDIF(${SWIG_VERSION} VERSION_LESS "2.0.1")

        # mkmf requires that all files be used in the compilation, so
	# we need to write out to our own directory.
        ADD_CUSTOM_TARGET(ruby-cueify
			  "${SWIG_EXECUTABLE}"
    			  -ruby -c++ -autorename
			  ${CMAKE_SWIG_FLAGS}
			  -outdir ${CMAKE_CURRENT_BINARY_DIR}
    			  ${swig_include_dirs}
			  -o cueifyRUBY_wrap.cxx
    			  "${SWIG_SOURCE_FILE_FULLNAME}")
        ADD_CUSTOM_COMMAND(TARGET ruby-cueify PRE_BUILD
		   	   COMMAND "${CMAKE_COMMAND}"
			   ARGS -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/ruby)
        ADD_CUSTOM_COMMAND(TARGET ruby-cueify POST_BUILD
		   	   COMMAND "${CMAKE_COMMAND}"
			   ARGS -E rename ${CMAKE_CURRENT_BINARY_DIR}/cueifyRUBY_wrap.cxx ${CMAKE_CURRENT_BINARY_DIR}/ruby/cueifyRUBY_wrap.cxx)
        ADD_CUSTOM_COMMAND(TARGET ruby-cueify POST_BUILD
		   	   COMMAND "${CMAKE_COMMAND}"
			   ARGS -E copy ${CMAKE_CURRENT_SOURCE_DIR}/extconf.rb ${CMAKE_CURRENT_BINARY_DIR}/ruby/extconf.rb)
    ENDIF(RUBY_FOUND)

    FIND_PACKAGE(PerlLibs)
    IF(PERLLIBS_FOUND)
         ADD_CUSTOM_TARGET(perl-cueify
			  "${SWIG_EXECUTABLE}"
    			  -perl -c++ -shadow
			  ${CMAKE_SWIG_FLAGS}
			  -outdir ${CMAKE_CURRENT_BINARY_DIR}
    			  ${swig_include_dirs}
			  -o cueifyPERL_wrap.cxx
    			  "${SWIG_SOURCE_FILE_FULLNAME}")
        ADD_CUSTOM_COMMAND(TARGET perl-cueify PRE_BUILD
		   	   COMMAND "${CMAKE_COMMAND}"
			   ARGS -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/perl)
        ADD_CUSTOM_COMMAND(TARGET perl-cueify POST_BUILD
		   	   COMMAND "${CMAKE_COMMAND}"
			   ARGS -E rename ${CMAKE_CURRENT_BINARY_DIR}/cueifyPERL_wrap.cxx ${CMAKE_CURRENT_BINARY_DIR}/perl/cueifyPERL_wrap.cxx)
        ADD_CUSTOM_COMMAND(TARGET perl-cueify POST_BUILD
		   	   COMMAND "${CMAKE_COMMAND}"
			   ARGS -E rename ${CMAKE_CURRENT_BINARY_DIR}/cueify.pm ${CMAKE_CURRENT_BINARY_DIR}/perl/cueify.pm)
        ADD_CUSTOM_COMMAND(TARGET ruby-cueify POST_BUILD
		   	   COMMAND "${CMAKE_COMMAND}"
			   ARGS -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Makefile.PL ${CMAKE_CURRENT_BINARY_DIR}/perl/Makefile.PL)
    ENDIF(PERLLIBS_FOUND)
ENDIF(SWIG_FOUND)

IF(CMAKE_COMPILER_IS_GNUCC)
    SET(CMAKE_CC_FLAGS "${CMAKE_CC_FLAGS} -Wall -Werror -Wextra -pedantic")
ENDIF(CMAKE_COMPILER_IS_GNUCC)
