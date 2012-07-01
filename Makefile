.PHONY: docs doxygen luadoc

ROOT=.
include config.mk

no_target:
	@echo "Please select a build target: linux macosx docs"
	@false

docs: doxygen luadoc

doxygen:
	mkdir -p docs/html
	mkdir -p docs/man
	echo "PROJECT_NUMBER = $(git rev-parse --short HEAD)" >> Doxyfile
	doxygen Doxyfile
	sed -i -e '/^PROJECT_NUMBER/d' Doxyfile

luadoc:
	mkdir -p docs/luadoc
	luadoc -d docs/luadoc docs.lua

linux:
	echo "$(PREFIX)" > .prefix
	make -C src -f makefile.linux
macosx:
	echo "$(PREFIX)" > .prefix
	make -C src -f makefile.macosx

install: src/autopilot
	prefix=$$(cat .prefix) ; \
	install -D -m 755 src/autopilot "$(DESTDIR)$$prefix/bin/autopilot" ; \
	mkdir -p "$(DESTDIR)$$prefix//share/autopilot/lib" ; \
	mkdir -p "$(DESTDIR)$$prefix//lib/autopilot/" ; \
	install -m 755 src/plugins/*.so "$(DESTDIR)$$prefix//lib/autopilot/" ; \
	install -m 644 src/*.lua "$(DESTDIR)$$prefix//share/autopilot/lib/"
