# Note: for the first time, run 'make setup' first (to install libusbhsfs packages), after that simply run 'make' or 'make dev' to build the project

.PHONY: all dev allclean clean libclean setup arc

all:
	@$(MAKE) -C libusbhsfs/ BUILD_TYPE=GPL install
	@$(MAKE) -C Plutonium/
	@$(MAKE) -C Goldleaf/

dev:
	@$(MAKE) -C libusbhsfs/ BUILD_TYPE=GPL install
	@$(MAKE) -C Plutonium/
	@$(MAKE) -C Goldleaf/ DEV_PREFIX=-dev

arc:
	@python arc/arc.py gen_db default+$(CURDIR)/Goldleaf/include/base_Results.rc.hpp
	@python arc/arc.py gen_cpp rc GLEAF $(CURDIR)/Goldleaf/include/base_Results.gen.hpp

setup: arc
	@$(MAKE) -C libusbhsfs/ fs-libs

allclean: libclean clean

libclean:
	@$(MAKE) -C Plutonium/ clean
	@$(MAKE) -C libusbhsfs/ clean

clean:
	@$(MAKE) -C Goldleaf/ clean