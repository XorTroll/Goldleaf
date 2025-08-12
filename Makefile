# Note: for the first time, run 'make setup' first (to install libusbhsfs packages), after that simply run 'make' to build the project

.PHONY: all allclean build clean libclean setup arc

all: arc build

build:
	@$(MAKE) -C Plutonium/
	@$(MAKE) -C libnx-ext/libnx-ipcext/
	@$(MAKE) -C libnxtc/
	@$(MAKE) -C Goldleaf/
arc:
	@python arc/arc.py gen_db default+./Goldleaf/include/res/res_Account.rc.hpp+./Goldleaf/include/res/res_ETicket.rc.hpp+./Goldleaf/include/res/res_NS.rc.hpp+./Goldleaf/include/res/res_Goldleaf.rc.hpp
	@python arc/arc.py gen_cpp rc GLEAF ./Goldleaf/include/res/res_Generated.gen.hpp

setup:
	@$(MAKE) -C libusbhsfs/ BUILD_TYPE=GPL install

allclean: libclean clean

libclean:
	@$(MAKE) -C Plutonium/ clean
	@$(MAKE) -C libnx-ext/libnx-ipcext/ clean
	@$(MAKE) -C libnxtc/ clean
	@$(MAKE) -C libusbhsfs/ clean

clean:
	@$(MAKE) -C Goldleaf/ clean
