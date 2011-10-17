all:
	if [ ! -d build ]; then mkdir build; fi; cd build; cmake ..; make all

install:
	if [ ! -d build ]; then mkdir build; fi; cd build; cmake ..; make install

clean:
	if [ -d build ]; then cd build; cmake ..; make clean; fi

distclean:
	if [ -d build ]; then rm -rf build; fi