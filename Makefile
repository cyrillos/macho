__nmak_dir=../scripts/
export __nmak_dir

include $(__nmak_dir)include.mk

.PHONY: all clean

all:
	$(call msg-gen, macho)
	$(Q) $(MAKE) -C macho

clean:
	$(Q) $(MAKE) -C macho clean

.DEFAULT_GOAL ?= all
