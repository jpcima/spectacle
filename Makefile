#!/usr/bin/make -f
# Makefile for DISTRHO Plugins #
# ---------------------------- #
# Created by falkTX, Christopher Arndt, and Patrick Desaulniers
#

-include Makefile.ini

include dpf/Makefile.base.mk

PLUGINS = spectacle

all: libs plugins gen

# --------------------------------------------------------------

submodules:
	git submodule update --init --recursive

libs:
	$(MAKE) -C dpf/dgl ../build/libdgl-opengl.a

res:
	$(foreach p,$(PLUGINS),$(MAKE) res -C plugins/${p};)

plugins: libs res
	$(foreach p,$(PLUGINS),$(MAKE) all -C plugins/${p};)

ifneq ($(CROSS_COMPILING),true)
gen: plugins dpf/utils/lv2_ttl_generator
	@$(CURDIR)/dpf/utils/generate-ttl.sh
ifeq ($(MACOS),true)
	@$(CURDIR)/dpf/utils/generate-vst-bundles.sh
endif

dpf/utils/lv2_ttl_generator:
	$(MAKE) -C dpf/utils/lv2-ttl-generator
else
gen: plugins dpf/utils/lv2_ttl_generator.exe
	@$(CURDIR)/dpf/utils/generate-ttl.sh

dpf/utils/lv2_ttl_generator.exe:
	$(MAKE) -C dpf/utils/lv2-ttl-generator WINDOWS=true
endif

# --------------------------------------------------------------

clean:
	$(MAKE) clean -C dpf/dgl
	$(MAKE) clean -C dpf/utils/lv2-ttl-generator
	$(foreach p,$(PLUGINS),$(MAKE) clean -C plugins/${p};)
	rm -rf bin build

install: all
	$(foreach p,$(PLUGINS),$(MAKE) install -C plugins/${p};)

install-user: all
	$(foreach p,$(PLUGINS),$(MAKE) install-user -C plugins/${p};)

# --------------------------------------------------------------

.PHONY: all clean install install-user submodule libs res plugins gen
