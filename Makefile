no_target:
	@echo "Please select a build target: linux macosx"
	@false
linux:
	make -C src -f makefile.linux
macosx:
	make -C src -f makefile.macosx
