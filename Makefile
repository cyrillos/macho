#
# Mandatory part to include nmak itself
__nmak_dir=nmak/scripts/
export __nmak_dir

include $(__nmak_dir)include.mk
include $(__nmak_dir)macro.mk

PROGRAM := macho

$(eval $(call gen-program,$(PROGRAM),src))

clean:
	$(call msg-clean, $(PROGRAM))
	$(Q) $(RM) $(PROGRAM)
	$(Q) $(MAKE) $(build)=src $@

all: $(PROGRAM)
	@true
.PHONY: all

.DEFAULT_GOAL := all
