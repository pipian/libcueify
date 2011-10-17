DIRS = src examples

all:
	-for d in $(DIRS); do (cd $$d; $(MAKE) all); done
