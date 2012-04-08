.PHONY: all po src install userinstall uninstall useruninstall clean dist

all: po src

po:
	$(MAKE) -C po
src:
	$(MAKE) -C src

install:
	$(MAKE) -C po install
	$(MAKE) -C src install

userinstall:
	$(MAKE) -C po userinstall
	$(MAKE) -C src userinstall

uninstall:
	$(MAKE) -C po uninstall
	$(MAKE) -C src uninstall

useruninstall:
	$(MAKE) -C po useruninstall
	$(MAKE) -C src useruninstall

clean:
	$(MAKE) -C po clean
	$(MAKE) -C src clean

dist:
	false
