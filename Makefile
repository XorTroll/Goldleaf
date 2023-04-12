# Note: run 'make initial' for the first time (to install libusbhsfs packages), after that simply run 'make'

.PHONY: all allclean clean libclean initial

all:
	@$(MAKE) -C Plutonium/
	@$(MAKE) -C Goldleaf/

initial:
	@$(MAKE) -C Plutonium/
	@$(MAKE) -C libusbhsfs/ BUILD_TYPE=GPL install
	@$(MAKE) -C Goldleaf/

allclean: libclean clean

libclean:
	@$(MAKE) -C Plutonium/ clean
	@$(MAKE) -C libusbhsfs/ clean

clean:
	@$(MAKE) -C Goldleaf/ clean