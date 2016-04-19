# type 'make help' for a list/explaination of recipes.

BUILDDIR = builddir

MAKEARGS += $(if $(VERBOSE),,--no-print-directory)

.PHONY: default
default: build

$(BUILDDIR):
	@echo "call 'mkdir $(BUILDDIR) && cd $(BUILDDIR) && cmake ..' to initialize the build directory."
	@echo ""
	@false

.PHONY: build
build: $(BUILDDIR)
	@$(MAKE) $(MAKEARGS) -C $(BUILDDIR)

.PHONY: docs
docs: $(BUILDDIR)
	@$(MAKE) $(MAKEARGS) -C $(BUILDDIR) docs

.PHONY: cleandocs
cleandocs: $(BUILDDIR)
	@# removes generated documentation
	rm -rf doc

.PHONY: clean
clean: $(BUILDDIR)
	@# removes object files, binaries, generated code
	@$(MAKE) $(MAKEARGS) -C $(BUILDDIR) clean

.PHONY: cleaninsourcebuild
cleaninsourcebuild:
	@echo "cleaning remains of in-source builds"
	rm -rf DartConfiguration.tcl codegen_depend_cache codegen_target_cache Doxyfile Testing
	@find . ! -path "./$(BUILDDIR)/*" -type f -name CTestTestfile.cmake -print -exec rm '{}' +
	@find . ! -path "./$(BUILDDIR)/*" -type f -name cmake_install.cmake -print -exec rm '{}' +
	@find . ! -path "./$(BUILDDIR)/*" -type f -name CMakeCache.txt -print -exec rm '{}' +
	@find . ! -path "./$(BUILDDIR)/*" -type f -name Makefile ! -path "./Makefile" -print -exec rm '{}' +
	@find . ! -path "./$(BUILDDIR)/*" -type d -name CMakeFiles -print -exec rm -r {} +

.PHONY: cleanbuilddirs
cleanbuilddirs: cleaninsourcebuild cleandocs
	@if test -d $(BUILDDIR); then $(MAKE) $(MAKEARGS) -C $(BUILDDIR) clean || true; fi
	@echo cleaning build directories
	rm -rf $(BUILDDIR)

.PHONY: mrproperer
mrproperer:
	@if ! test -d .git; then echo "mrproperer is only available for gitrepos."; false; fi
	@echo removing ANYTHING that is not checked into the dit repo
	@echo ENTER to confirm
	@read val
	git clean -x -d -f

.PHONY: help
help: $(BUILDDIR)/Makefile
	@echo "Lotos++ Makefile"
	@echo ""
	@echo "wrapper that mostly forwards recipes to the cmake-generated Makefile in build/"
	@echo ""
	@echo "targets:"
	@echo ""
	@echo "build              -> build entire project"
	@echo "docs               -> create documentation files"
	@echo ""
	@echo "cleandocs          -> undo 'make docs'"
	@echo "clean              -> remove C++ ELF files"
	@echo "cleanbuilddirs     -> undo 'make'"
	@echo "cleaninsourcebuild -> undo in-source build accidents"
	@echo "mrproperer         -> leaves nothing but ashes"
	@echo ""
	@echo "CMake help:"
	@test -d $(BUILDDIR) && $(MAKE) -C $(BUILDDIR) help || echo "no builddir is configured"
