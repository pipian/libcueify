SET(CMAKE_INCLUDE_CURRENT_DIR ON)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/../include)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR}/../include)
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/../include/libcueify/types.h.in
               ${CMAKE_CURRENT_BINARY_DIR}/../include/libcueify/types.h)

SET(_sources device.c toc.c sessions.c full_toc.c cdtext.c latin1.c msjis.c
             ascii.c mcn_isrc.c indices.c data_mode.c cdtext_crc.c)

INCLUDE(CheckTypeSize)
CHECK_TYPE_SIZE(uint8_t UINT8_T)
CHECK_TYPE_SIZE(uint16_t UINT16_T)
CHECK_TYPE_SIZE(uint32_t UINT32_T)
IF(!HAVE_UINT32_T)
    CHECK_TYPE_SIZE(int INT)
ENDIF(!HAVE_UINT32_T)
CHECK_TYPE_SIZE(size_t SIZE_T)

INCLUDE(CheckSymbolExists)
CHECK_SYMBOL_EXISTS(_WIN32 windows.h OS_IS_WINDOWS)
CHECK_SYMBOL_EXISTS(__FreeBSD__ sys/param.h OS_IS_FREEBSD)
CHECK_SYMBOL_EXISTS(NULL stddef.h HAVE_NULL)

IF(OS_IS_WINDOWS)
    SET(_os_sources win32.c)
ENDIF(OS_IS_WINDOWS)
IF(OS_IS_FREEBSD)
    SET(_os_sources freebsd.c)
ENDIF(OS_IS_FREEBSD)

ADD_LIBRARY(cueify SHARED ${_sources} ${_os_sources})

IF(OS_IS_FREEBSD)
    TARGET_LINK_LIBRARIES(cueify cam)
ENDIF(OS_IS_FREEBSD)

SET_TARGET_PROPERTIES(cueify PROPERTIES
    VERSION ${libcueify_VERSION}
    SOVERSION ${libcueify_SOVERSION}
    DEFINE_SYMBOL LIBCUEIFY_API_EXPORTS
)

IF(CMAKE_COMPILER_IS_GNUCC)
    # Unfortunately, libcam and w32api fail -pedantic-errors
    # (and -isystem fails to help)
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror -Wextra -std=gnu99 -pedantic")
ENDIF(CMAKE_COMPILER_IS_GNUCC)

INSTALL(TARGETS cueify DESTINATION ${LIB_INSTALL_DIR})
