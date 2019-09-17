
.PHONY: all clean

all:
	$(MAKE) -C Goldleaf/
	$(MAKE) -C Forwarder/

clean:
	$(MAKE) clean -C Goldleaf/
	$(MAKE) clean -C Forwarder/