CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

# Suppress CYGWIN warning (until 2.8.4 is required)
SET(CMAKE_LEGACY_CYGWIN_WIN32 0)

PROJECT(libcueify)
SET(PROJECT_VERSION 0.5.0)

SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules")

# Follow libmusicbrainz-style so-versioning:
# 
# 1. If the library source code has changed at all since the last
#    update, then increment revision.
# 2. If any interfaces have been added, removed, or changed since the
#    last update, increment current, and set revision to 0.
# 3. If any interfaces have been added since the last public release,
#    then increment age.
# 4. If any interfaces have been removed since the last public
#    release, then set age to 0.
SET(libcueify_SOVERSION_CURRENT  1)
SET(libcueify_SOVERSION_REVISION 0)
SET(libcueify_SOVERSION_AGE      1)

MATH(EXPR libcueify_SOVERSION_MAJOR
     "${libcueify_SOVERSION_CURRENT} - ${libcueify_SOVERSION_AGE}")
MATH(EXPR libcueify_SOVERSION_MINOR "${libcueify_SOVERSION_AGE}")
MATH(EXPR libcueify_SOVERSION_PATCH "${libcueify_SOVERSION_REVISION}")

SET(libcueify_VERSION ${libcueify_SOVERSION_MAJOR}.${libcueify_SOVERSION_MINOR}.${libcueify_SOVERSION_PATCH})
SET(libcueify_SOVERSION ${libcueify_SOVERSION_MAJOR})

SET(LIB_SUFFIX "" CACHE STRING "Define suffix of directory name (32/64)")
SET(EXEC_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX} CACHE PATH
    "Installation prefix for executables and object code libraries" FORCE)
SET(LIB_INSTALL_DIR ${EXEC_INSTALL_PREFIX}/lib${LIB_SUFFIX} CACHE PATH
    "Installation prefix for object code libraries" FORCE)
SET(INCLUDE_INSTALL_DIR ${EXEC_INSTALL_PREFIX}/include CACHE PATH
    "Installation prefix for C header files" FORCE)

ADD_SUBDIRECTORY(src)
ADD_SUBDIRECTORY(examples)
ADD_SUBDIRECTORY(swig EXCLUDE_FROM_ALL)
ADD_SUBDIRECTORY(tests EXCLUDE_FROM_ALL)

CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in
	       ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile)

FILE(GLOB cheaders ${CMAKE_CURRENT_SOURCE_DIR}/include/cueify/*.h)
FILE(GLOB cppheaders ${CMAKE_CURRENT_SOURCE_DIR}/include/cueify/*.hxx)
INSTALL(FILES ${cheaders} ${cppheaders}
        ${CMAKE_CURRENT_BINARY_DIR}/include/cueify/types.h
        DESTINATION ${INCLUDE_INSTALL_DIR}/cueify)

ADD_CUSTOM_TARGET(docs doxygen WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
