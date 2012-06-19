.PHONY: docs

no_target:
	@echo "Please select a build target: linux macosx docs"
	@false

docs:
	mkdir -p docs/html
	mkdir -p docs/man
	echo "PROJECT_NUMBER = $(git rev-parse --short HEAD)" >> Doxyfile
	doxygen Doxyfile
	sed -i -e '/^PROJECT_NUMBER/d' Doxyfile
linux:
	make -C src -f makefile.linux
macosx:
	make -C src -f makefile.macosx
