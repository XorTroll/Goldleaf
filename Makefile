# Note: run 'make initial' for the first time (to install libusbhsfs packages), after that simply run 'make'

.PHONY: all allclean clean libclean initial

all:
	@$(MAKE) -C libnx/
	@$(MAKE) -C Plutonium/ LIBNX=$(CURDIR)/libnx/nx
	@$(MAKE) -C Goldleaf/ LIBNX=$(CURDIR)/libnx/nx

initial:
	@$(MAKE) -C libnx/
	@$(MAKE) -C Plutonium/ LIBNX=$(CURDIR)/libnx/nx
	@$(MAKE) -C libusbhsfs/ LIBNX=$(CURDIR)/libnx/nx BUILD_TYPE=GPL install
	@$(MAKE) -C Goldleaf/ LIBNX=$(CURDIR)/libnx/nx

allclean: libclean clean

libclean:
	@$(MAKE) -C libnx/ clean
	@$(MAKE) -C Plutonium/ clean
	@$(MAKE) -C libusbhsfs/ clean

clean:
	@$(MAKE) -C Goldleaf/ clean