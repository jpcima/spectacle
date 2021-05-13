#!/usr/bin/make -f
# Makefile for DISTRHO Plugins #
# ---------------------------- #
# Created by falkTX, Christopher Arndt, and Patrick Desaulniers
#

-include ../../Makefile.ini

# --------------------------------------------------------------
# Installation directories

PREFIX ?= /usr/local
BINDIR ?= $(PREFIX)/bin
LIBDIR ?= $(PREFIX)/lib
DSSI_DIR ?= $(LIBDIR)/dssi
LADSPA_DIR ?= $(LIBDIR)/ladspa
ifneq ($(MACOS_OR_WINDOWS),true)
LV2_DIR ?= $(LIBDIR)/lv2
VST_DIR ?= $(LIBDIR)/vst
endif
ifeq ($(MACOS),true)
LV2_DIR ?= /Library/Audio/Plug-Ins/LV2
VST_DIR ?= /Library/Audio/Plug-Ins/VST
endif
ifeq ($(WINDOWS),true)
LV2_DIR ?= $(COMMONPROGRAMFILES)/LV2
VST_DIR ?= $(COMMONPROGRAMFILES)/VST2
endif

USER_DSSI_DIR ?= $(HOME)/.dssi
USER_LADSPA_DIR ?= $(HOME)/.ladspa
ifneq ($(MACOS_OR_WINDOWS),true)
USER_LV2_DIR ?= $(HOME)/.lv2
USER_VST_DIR ?= $(HOME)/.vst
endif
ifeq ($(MACOS),true)
USER_LV2_DIR ?= $(HOME)/Library/Audio/Plug-Ins/LV2
USER_VST_DIR ?= $(HOME)/Library/Audio/Plug-Ins/VST
endif
ifeq ($(WINDOWS),true)
USER_LV2_DIR ?= $(APPDATA)/LV2
USER_VST_DIR ?= $(APPDATA)/VST
endif

# --------------------------------------------------------------
# Project name, used for binaries

NAME = spectacle-analyzer

# --------------------------------------------------------------
# Plugin types to build

BUILD_LV2 ?= true
BUILD_VST2 ?= true
BUILD_JACK ?= true
BUILD_DSSI ?= false
BUILD_LADSPA ?= false

# --------------------------------------------------------------
# Files to build

FILES_DSP = \
	sources/plugin/PluginSpectralAnalyzer.cpp \
	sources/plugin/Parameters.cpp \
	sources/dsp/FFTPlanner.cpp \
	sources/dsp/SpectralAnalyzer.cpp \
	sources/dsp/STFT.cpp \
	sources/dsp/MultirateSTFT.cpp \
	thirdparty/spin_mutex/src/SpinMutex.cpp \
	thirdparty/rt_semaphore/src/RTSemaphore.cpp

FILES_UI = \
	sources/plugin/UISpectralAnalyzer.cpp \
	sources/plugin/Parameters.cpp \
	sources/plugin/Config.cpp \
	sources/plugin/ColorPalette.cpp \
	sources/plugin/FontDefs.cpp \
	sources/ui/components/SpectrumView.cpp \
	sources/ui/components/MainToolBar.cpp \
	sources/ui/components/FloatingWindow.cpp \
	sources/ui/components/SpinBoxChooser.cpp \
	sources/ui/components/Slider.cpp \
	sources/ui/components/TextLabel.cpp \
	sources/ui/components/SelectionRectangle.cpp \
	sources/ui/components/ResizeHandle.cpp \
	sources/ui/FontEngine.cpp \
	sources/util/format_string.cpp \
	thirdparty/spline/spline/spline.cpp \
	thirdparty/simpleini/ConvertUTF.cpp

# --------------------------------------------------------------
# Do some magic

#UI_TYPE = cairo
UI_TYPE = opengl
include ../../dpf/Makefile.plugins.mk

BUILD_CXX_FLAGS += -Isources -Imeta -I$(BUILD_DIR)/res
BUILD_CXX_FLAGS += -Ithirdparty/blink -Ithirdparty/spline -Ithirdparty/stb -Ithirdparty/simpleini -Ithirdparty/hiir-1.33 -Ithirdparty/spin_mutex/src -Ithirdparty/rt_semaphore/src

FFTW_CFLAGS = $(shell $(PKG_CONFIG) --cflags fftw3f)
FFTW_LIBS   = $(shell $(PKG_CONFIG) --libs fftw3f)

BUILD_C_FLAGS += $(FFTW_CFLAGS)
BUILD_CXX_FLAGS += $(FFTW_CFLAGS)
LINK_FLAGS += $(FFTW_LIBS)

ifeq ($(USE_IMPATIENT_FFT_PLANNING),1)
BUILD_CXX_FLAGS += -DUSE_IMPATIENT_FFT_PLANNING=1
endif
ifeq ($(SKIP_FFT_PRECACHING),1)
BUILD_CXX_FLAGS += -DSKIP_FFT_PRECACHING=1
endif

ifeq ($(LINUX),true)
BUILD_CXX_FLAGS += -pthread
endif

# want the asprintf functions
BUILD_C_FLAGS += -D_GNU_SOURCE=1
BUILD_CXX_FLAGS += -D_GNU_SOURCE=1

# --------------------------------------------------------------
# Resources

FILES_RES = \
	fonts/liberation/LiberationSans-Regular.ttf.h \
	fonts/fontawesome/Font-Awesome-5-Free-Solid-900.otf.h

res: $(foreach r,$(FILES_RES),$(BUILD_DIR)/res/$(r))

$(BUILD_DIR)/res/%.h: ../../resources/%.gz
	mkdir -p $(dir $@)
	zcat $^ | od -An -tx1 -v | awk '{for(i=1;i<=NF;i++){printf "0x%s,",$$i}}' > $@.tmp
	mv -f $@.tmp $@

$(BUILD_DIR)/res/%.h: ../../resources/%
	mkdir -p $(dir $@)
	cat $^ | od -An -tx1 -v | awk '{for(i=1;i<=NF;i++){printf "0x%s,",$$i}}' > $@.tmp
	mv -f $@.tmp $@

# --------------------------------------------------------------
# Enable all selected plugin types

ifeq ($(BUILD_LV2),true)
TARGETS += lv2
endif

ifeq ($(BUILD_VST2),true)
TARGETS += vst
endif

ifeq ($(BUILD_JACK),true)
ifeq ($(HAVE_JACK),true)
TARGETS += jack
endif
endif

ifeq ($(BUILD_DSSI),true)
ifneq ($(MACOS_OR_WINDOWS),true)
ifeq ($(HAVE_DGL),true)
ifeq ($(HAVE_LIBLO),true)
TARGETS += dssi
endif
endif
endif
endif

ifeq ($(BUILD_LADSPA),true)
TARGETS += ladspa
endif

all: $(TARGETS)

install: all
ifeq ($(BUILD_DSSI),true)
ifneq ($(MACOS_OR_WINDOWS),true)
ifeq ($(HAVE_DGL),true)
ifeq ($(HAVE_LIBLO),true)
	@mkdir -p -m 755 $(DESTDIR)$(DSSI_DIR)
	@install -m 755 $(TARGET_DIR)/$(NAME)-dssi$(LIB_EXT) $(DESTDIR)$(DSSI_DIR)/$(NAME)-dssi$(LIB_EXT)
endif
endif
endif
endif
ifeq ($(BUILD_LADSPA),true)
	@mkdir -p -m 755 $(DESTDIR)$(LADSPA_DIR)
	@install -m 755 $(TARGET_DIR)/$(NAME)-ladspa$(LIB_EXT) $(DESTDIR)$(LADSPA_DIR)/$(NAME)-ladspa$(LIB_EXT)
endif
ifeq ($(BUILD_VST2),true)
	@mkdir -p -m 755 $(DESTDIR)$(VST_DIR)
	@install -m 755 $(TARGET_DIR)/$(NAME)-vst$(LIB_EXT) $(DESTDIR)$(VST_DIR)/$(NAME)-vst$(LIB_EXT)
endif
ifeq ($(BUILD_LV2),true)
	@mkdir -p -m 755 $(DESTDIR)$(LV2_DIR)/$(NAME).lv2
	@install -m 644 $(TARGET_DIR)/$(NAME).lv2/*.ttl $(DESTDIR)$(LV2_DIR)/$(NAME).lv2
	@install -m 755 $(TARGET_DIR)/$(NAME).lv2/*$(LIB_EXT) $(DESTDIR)$(LV2_DIR)/$(NAME).lv2
endif
ifeq ($(BUILD_JACK),true)
ifeq ($(HAVE_JACK),true)
	@mkdir -p -m 755 $(DESTDIR)$(BINDIR)
	@install -m 755 $(TARGET_DIR)/$(NAME)$(APP_EXT) $(DESTDIR)$(BINDIR)/$(NAME)$(APP_EXT)
endif
endif

install-user: all
ifeq ($(BUILD_DSSI),true)
ifneq ($(MACOS_OR_WINDOWS),true)
ifeq ($(HAVE_DGL),true)
ifeq ($(HAVE_LIBLO),true)
	@mkdir -p -m 755 $(USER_DSSI_DIR)
	@install -m 755 $(TARGET_DIR)/$(NAME)-dssi$(LIB_EXT) $(USER_DSSI_DIR)/$(NAME)-dssi$(LIB_EXT)
endif
endif
endif
endif
ifeq ($(BUILD_LADSPA),true)
	@mkdir -p -m 755 $(USER_LADSPA_DIR)
	@install -m 755 $(TARGET_DIR)/$(NAME)-ladspa$(LIB_EXT) $(USER_LADSPA_DIR)/$(NAME)-ladspa$(LIB_EXT)
endif
ifeq ($(BUILD_VST2),true)
	@mkdir -p -m 755 $(USER_VST_DIR)
	@install -m 755 $(TARGET_DIR)/$(NAME)-vst$(LIB_EXT) $(USER_VST_DIR)/$(NAME)-vst$(LIB_EXT)
endif
ifeq ($(BUILD_LV2),true)
	@mkdir -p -m 755 $(USER_LV2_DIR)/$(NAME).lv2
	@install -m 644 $(TARGET_DIR)/$(NAME).lv2/*.ttl $(USER_LV2_DIR)/$(NAME).lv2
	@install -m 755 $(TARGET_DIR)/$(NAME).lv2/*$(LIB_EXT) $(USER_LV2_DIR)/$(NAME).lv2
endif
ifeq ($(BUILD_JACK),true)
ifeq ($(HAVE_JACK),true)
	@mkdir -p -m 755 $(HOME)/bin
	@install -m 755 $(TARGET_DIR)/$(NAME)$(APP_EXT) $(HOME)/bin/$(NAME)$(APP_EXT)
endif
endif

# --------------------------------------------------------------

.PHONY: all res install install-user
