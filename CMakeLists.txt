cmake_minimum_required(VERSION 2.6)

PROJECT(libcueify)
SET(PROJECT_VERSION 0.1.0)

# Follow libmusicbrainz-style so-versioning:
# 
# 1. If the library source code has changed at all since the last update, then increment revision.
# 2. If any interfaces have been added, removed, or changed since the last update, increment current, and set revision to 0.
# 3. If any interfaces have been added since the last public release, then increment age.
# 4. If any interfaces have been removed since the last public release, then set age to 0.
SET(cueify_SOVERSION_CURRENT  0)
SET(cueify_SOVERSION_REVISION 0)
SET(cueify_SOVERSION_AGE      0)

MATH(EXPR cueify_SOVERSION_MAJOR "${cueify_SOVERSION_CURRENT} - ${cueify_SOVERSION_AGE}")
MATH(EXPR cueify_SOVERSION_MINOR "${cueify_SOVERSION_AGE}")
MATH(EXPR cueify_SOVERSION_PATCH "${cueify_SOVERSION_REVISION}")

SET(cueify_VERSION ${cueify_SOVERSION_MAJOR}.${cueify_SOVERSION_MINOR}.${cueify_SOVERSION_PATCH})
SET(cueify_SOVERSION ${cueify_SOVERSION_MAJOR})

ADD_SUBDIRECTORY(src)
ADD_SUBDIRECTORY(examples)
