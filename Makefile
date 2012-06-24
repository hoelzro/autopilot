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
	make -C src -f makefile.linux
macosx:
	make -C src -f makefile.macosx
