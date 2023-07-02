# Note: run 'make setup' for the first time (to install libusbhsfs packages), after that simply run 'make' to avoid unnecessary recompiles

.PHONY: all allclean clean libclean setup arc

all:
	@$(MAKE) -C libusbhsfs/ BUILD_TYPE=GPL install
	@$(MAKE) -C Plutonium/
	@$(MAKE) -C Goldleaf/

arc:
	@python arc/arc.py gen_db default+$(CURDIR)/Goldleaf/include/base_Results.rc.hpp
	@python arc/arc.py gen_cpp rc GLEAF $(CURDIR)/Goldleaf/include/base_Results.gen.hpp

setup: arc
	@$(MAKE) -C libusbhsfs/ fs-libs
	@$(MAKE) -C libusbhsfs/ BUILD_TYPE=GPL install
	@$(MAKE) -C Plutonium/
	@$(MAKE) -C Goldleaf/

allclean: libclean clean

libclean:
	@$(MAKE) -C Plutonium/ clean
	@$(MAKE) -C libusbhsfs/ clean

clean:
	@$(MAKE) -C Goldleaf/ clean