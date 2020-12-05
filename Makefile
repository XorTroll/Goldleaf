
.PHONY: all allclean clean libclean

all:
	@$(MAKE) -C Plutonium/
	@$(MAKE) -C libusbhsfs/
	@$(MAKE) -C Goldleaf/

allclean: libclean clean

libclean:
	@$(MAKE) -C Plutonium/ clean
	@$(MAKE) -C libusbhsfs/ clean

clean:
	@$(MAKE) -C Goldleaf/ clean