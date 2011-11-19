# If we are in MinGW, we need to specify an absolute CMake path.
# This kludge may not play nicely when gcc isn't available...
CMAKE_COMMAND = cmake
ifeq ($(shell gcc -dumpmachine),mingw32)
CMAKE_COMMAND = "/c/Program Files/CMake 2.8/bin/cmake.exe"
endif

all:
	if [ ! -d build ]; then mkdir build; fi; cd build; $(CMAKE_COMMAND) ..; make all

check:
	if [ ! -d build ]; then mkdir build; fi; cd build; $(CMAKE_COMMAND) ..; make check

check-unportable:
	if [ ! -d build ]; then mkdir build; fi; cd build; $(CMAKE_COMMAND) ..; make check-unportable

check-indices:
	if [ ! -d build ]; then mkdir build; fi; cd build; $(CMAKE_COMMAND) ..; make check-indices

check-pregaps:
	if [ ! -d build ]; then mkdir build; fi; cd build; $(CMAKE_COMMAND) ..; make check-pregaps

docs:
	if [ -d docs ]; then rm -rf docs; fi
	if [ ! -d build ]; then mkdir build; fi; cd build; $(CMAKE_COMMAND) ..; make docs

install:
	if [ ! -d build ]; then mkdir build; fi; cd build; $(CMAKE_COMMAND) ..; make install

clean:
	if [ -d build ]; then cd build; $(CMAKE_COMMAND) ..; make clean; fi

distclean:
	if [ -d build ]; then rm -rf build; fi