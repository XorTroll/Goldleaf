
.PHONY: all clean

all:
	@$(MAKE) -C Plutonium/
	@$(MAKE) -C Goldleaf/

clean:
	@$(MAKE) -C Plutonium/ clean
	@$(MAKE) -C Goldleaf/ clean