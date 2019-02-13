.SUFFIXES:

ifeq ($(strip $(DEVKITPRO)),)
$(error "Please set DEVKITPRO in your environment. export DEVKITPRO=<path to>/devkitpro")
endif

TOPDIR ?= $(CURDIR)
include $(DEVKITPRO)/libnx/switch_rules

# Goldleaf's new Makefile system :)

APP_TITLE   := Goldleaf
APP_AUTHOR  := XorTroll
APP_VERSION := 0.4
APP_TITLEID	:= 050032A5CF12E000
TARGET      := Goldleaf
ICON		:= Control/RomFs/icon_AmericanEnglish.dat

BUILD       := Build
SOURCES     := Program/ExeFs/main/Source Program/ExeFs/main/Source/fatfs Program/ExeFs/main/Source/gleaf Program/ExeFs/main/Source/gleaf/acc Program/ExeFs/main/Source/gleaf/dump Program/ExeFs/main/Source/gleaf/es Program/ExeFs/main/Source/gleaf/fs Program/ExeFs/main/Source/gleaf/hacpack Program/ExeFs/main/Source/gleaf/hactool Program/ExeFs/main/Source/gleaf/horizon Program/ExeFs/main/Source/gleaf/ini Program/ExeFs/main/Source/gleaf/ncm Program/ExeFs/main/Source/gleaf/ns Program/ExeFs/main/Source/gleaf/nsp Program/ExeFs/main/Source/gleaf/set Program/ExeFs/main/Source/gleaf/ui Program/ExeFs/main/Source/gleaf/usb
INCLUDES    := Program/ExeFs/main/Include Program/ExeFs/main/Include/fatfs Program/ExeFs/main/Include/gleaf Program/ExeFs/main/Include/gleaf/acc Program/ExeFs/main/Include/gleaf/dump Program/ExeFs/main/Include/gleaf/es Program/ExeFs/main/Include/gleaf/fs Program/ExeFs/main/Include/gleaf/hacpack Program/ExeFs/main/Include/gleaf/hactool Program/ExeFs/main/Include/gleaf/horizon Program/ExeFs/main/Include/gleaf/ini Program/ExeFs/main/Include/gleaf/ncm Program/ExeFs/main/Include/gleaf/ns Program/ExeFs/main/Include/gleaf/nsp Program/ExeFs/main/Include/gleaf/set Program/ExeFs/main/Include/gleaf/ui Program/ExeFs/main/Include/gleaf/usb
ROMFS       := Program/RomFs

ARCH	:=	-march=armv8-a -mtune=cortex-a57 -mtp=soft -fPIE

CFLAGS	:=	-g -Wall -O2 -ffunction-sections -fpermissive \
			$(ARCH) $(DEFINES) $(CFLAGS)

CFLAGS	+=	$(INCLUDE) -D__SWITCH__ $(QFLAG)

CXXFLAGS	:= $(CFLAGS) -std=gnu++17 $(CXXFLAGS)

ASFLAGS	:=	-g $(ARCH)
LDFLAGS	=	-specs=$(DEVKITPRO)/libnx/switch.specs -g $(ARCH) -Wl,-Map,$(notdir $*.map)

LIBS	:=  -lnx -lpu -lhactool -lmbedcrypto -lfreetype -lSDL2_ttf -lSDL2_gfx -lSDL2_image -lSDL2 -lEGL -lGLESv2 -lglapi -ldrm_nouveau -lpng -ljpeg `sdl2-config --libs` `freetype-config --libs`
LIBDIRS	:= $(PORTLIBS) $(CURDIR)/../libnx-Goldleaf/nx $(CURDIR)/Program/ExeFs/main/External/pu $(CURDIR)/Program/ExeFs/main/External/json $(CURDIR)/Program/ExeFs/main/External/hactool $(CURDIR)/Program/ExeFs/main/External/mbedtls

ifneq ($(BUILD),$(notdir $(CURDIR)))

export OUTPUT	:=	$(CURDIR)/Output/$(TARGET)
export TOPDIR	:=	$(CURDIR)

export VPATH	:=	$(foreach dir,$(SOURCES),$(CURDIR)/$(dir)) \
			$(foreach dir,$(DATA),$(CURDIR)/$(dir))

export DEPSDIR	:=	$(CURDIR)/$(BUILD)

CFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.c)))
CPPFILES	:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.cpp)))
SFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.s)))
BINFILES	:=	$(foreach dir,$(DATA),$(notdir $(wildcard $(dir)/*.*)))

ifeq ($(strip $(CPPFILES)),)
	export LD	:=	$(CC)
else
	export LD	:=	$(CXX)
endif

export OFILES_BIN	:=	$(addsuffix .o,$(BINFILES))
export OFILES_SRC	:=	$(CPPFILES:.cpp=.o) $(CFILES:.c=.o) $(SFILES:.s=.o)
export OFILES 	:=	$(OFILES_BIN) $(OFILES_SRC)
export HFILES_BIN	:=	$(addsuffix .h,$(subst .,_,$(BINFILES)))

export INCLUDE	:=	$(foreach dir,$(INCLUDES),-I$(CURDIR)/$(dir)) \
			$(foreach dir,$(LIBDIRS),-I$(dir)/include) \
			-I$(CURDIR)/$(BUILD)

export LIBPATHS	:=	$(foreach dir,$(LIBDIRS),-L$(dir)/lib)

export BUILD_EXEFS_SRC := $(TOPDIR)/$(EXEFS_SRC)

ifeq ($(strip $(ICON)),)
	icons := $(wildcard *.jpg)
	ifneq (,$(findstring $(TARGET).jpg,$(icons)))
		export APP_ICON := $(TOPDIR)/$(TARGET).jpg
	else
		ifneq (,$(findstring icon.jpg,$(icons)))
			export APP_ICON := $(TOPDIR)/icon.jpg
		endif
	endif
else
	export APP_ICON := $(TOPDIR)/$(ICON)
endif

ifeq ($(strip $(NO_ICON)),)
	export NROFLAGS += --icon=$(APP_ICON)
endif

ifeq ($(strip $(NO_NACP)),)
	export NROFLAGS += --nacp=$(CURDIR)/Control/RomFs/control.nacp
endif

ifneq ($(APP_TITLEID),)
	export NACPFLAGS += --titleid=$(APP_TITLEID)
endif

ifneq ($(ROMFS),)
	export NROFLAGS += --romfsdir=$(CURDIR)/$(ROMFS)
endif

.PHONY: $(BUILD) nro nsp qlaunch clean

nro: $(BUILD)

$(BUILD):
	@mkdir -p $@ $(CURDIR)/Output
	@echo Goldleaf[nro]: Compiling and generating NRO...
	@$(MAKE) --no-print-directory -C $(BUILD) -f $(CURDIR)/Makefile
	@rm -f $(CURDIR)/Output/Goldleaf.nso
	@echo Goldleaf[nro]: Build finished!

qlaunch:
	@mkdir -p $(BUILD) $(CURDIR)/Output
	@echo Goldleaf[qlaunch]: Compiling and generating as qlaunch replacement...
	@$(MAKE) --no-print-directory QFLAG=-DGOLDLEAF_QLAUNCH -C $(BUILD) -f $(CURDIR)/Makefile
	@rm -f $(CURDIR)/Output/Goldleaf.nro
	@rm -rf $(CURDIR)/Output/titles
	@mkdir -p $(CURDIR)/Output/titles/0100000000001000/exefs $(CURDIR)/Output/titles/0100000000001000/romfs
	@touch $(CURDIR)/Output/titles/0100000000001000/fsmitm.flag
	@touch $(CURDIR)/Output/titles/0100000000001000/exefs/rtld.stub
	@cp $(CURDIR)/Program/ExeFs/main.npdm/qlaunch.npdm $(CURDIR)/Output/titles/0100000000001000/exefs/main.npdm
	@cp -a $(CURDIR)/Program/RomFs/. $(CURDIR)/Output/titles/0100000000001000/romfs/
	@mv $(CURDIR)/Output/Goldleaf.nso $(CURDIR)/Output/titles/0100000000001000/exefs/main
	@echo Goldleaf[qlaunch]: Build finished!

nsp:
	@echo Goldleaf[nsp]: Starting build process...
	@mkdir -p $(CURDIR)/Output
	@echo Goldleaf[nsp]: Compiling source code...
	@$(MAKE) --no-print-directory -C $(BUILD) -f $(CURDIR)/Makefile
	@echo Goldleaf[nsp]: Ensuring content directories...
	@rm -rf $(CURDIR)/Control/_content
	@mkdir -p $(CURDIR)/Control/_content
	@rm -rf $(CURDIR)/HtmlDocument/_content
	@mkdir -p $(CURDIR)/HtmlDocument/_content
	@rm -rf $(CURDIR)/LegalInfo/_content
	@mkdir -p $(CURDIR)/LegalInfo/_content
	@rm -rf $(CURDIR)/Program/_content
	@mkdir -p $(CURDIR)/Program/_content
	@rm -rf $(CURDIR)/Program/_temp
	@mkdir -p $(CURDIR)/Program/_temp
	@rm -rf $(CURDIR)/Output/_meta
	@mkdir -p $(CURDIR)/Output/_meta
	@mv $(CURDIR)/Output/Goldleaf.nso $(CURDIR)/Program/_temp/main
	@cp $(CURDIR)/Program/ExeFs/main.npdm/default.npdm $(CURDIR)/Program/_temp/main.npdm
	@echo Goldleaf[nsp]: Generating program NCA...
	@$(CURDIR)/BuildTools/hacpack -k $(CURDIR)/BuildTools/prod.keys --type nca --ncatype program --titleid $(APP_TITLEID) --exefsdir=$(CURDIR)/Program/_temp --romfsdir=$(CURDIR)/Program/RomFs --logodir=$(CURDIR)/Program/Logo --outdir=$(CURDIR)/Program/_content
	$(eval CPROGRAM = $(shell ls -t $(CURDIR)/Program/_content | head -n 1))
	@echo Goldleaf[nsp]: Generating control NCA...
	@$(CURDIR)/BuildTools/hacpack -k $(CURDIR)/BuildTools/prod.keys --type nca --ncatype control --titleid $(APP_TITLEID) --romfsdir=$(CURDIR)/Control/RomFs --outdir=$(CURDIR)/Control/_content
	$(eval CCONTROL = $(shell ls -t $(CURDIR)/Control/_content | head -n 1))
	@echo Goldleaf[nsp]: Generating HTML document NCA...
	@$(CURDIR)/BuildTools/hacpack -k $(CURDIR)/BuildTools/prod.keys --type nca --ncatype manual --titleid $(APP_TITLEID) --romfsdir=$(CURDIR)/HtmlDocument/RomFs --outdir=$(CURDIR)/HtmlDocument/_content
	$(eval CHTMLDOC = $(shell ls -t $(CURDIR)/HtmlDocument/_content | head -n 1))
	@echo Goldleaf[nsp]: Generating legal information NCA...
	@$(CURDIR)/BuildTools/hacpack -k $(CURDIR)/BuildTools/prod.keys --type nca --ncatype manual --titleid $(APP_TITLEID) --romfsdir=$(CURDIR)/LegalInfo/RomFs --outdir=$(CURDIR)/LegalInfo/_content
	$(eval CLEGALINFO = $(shell ls -t $(CURDIR)/LegalInfo/_content | head -n 1))
	@echo Goldleaf[nsp]: Generating meta CNMT NCA...
	@$(CURDIR)/BuildTools/hacpack -k $(CURDIR)/BuildTools/prod.keys --type nca --ncatype meta --titleid $(APP_TITLEID) --titletype application --programnca $(CURDIR)/Program/_content/$(CPROGRAM) --controlnca $(CURDIR)/Control/_content/$(CCONTROL) --htmldocnca $(CURDIR)/HtmlDocument/_content/$(CHTMLDOC) --legalnca $(CURDIR)/LegalInfo/_content/$(CLEGALINFO) --outdir=$(CURDIR)/Output/_meta
	$(eval CMETA = $(shell ls -t $(CURDIR)/Output/_meta | head -n 1))
	@cp $(CURDIR)/Program/_content/$(CPROGRAM) $(CURDIR)/Output/_meta/$(CPROGRAM)
	@cp $(CURDIR)/Control/_content/$(CCONTROL) $(CURDIR)/Output/_meta/$(CCONTROL)
	@cp $(CURDIR)/HtmlDocument/_content/$(CHTMLDOC) $(CURDIR)/Output/_meta/$(CHTMLDOC)
	@cp $(CURDIR)/LegalInfo/_content/$(CLEGALINFO) $(CURDIR)/Output/_meta/$(CLEGALINFO)
	@echo Goldleaf[nsp]: Contents copied. Generating final NSP...
	@$(CURDIR)/BuildTools/hacpack -k $(CURDIR)/BuildTools/prod.keys --type nsp --titleid $(APP_TITLEID) --ncadir $(CURDIR)/Output/_meta --outdir=$(CURDIR)/Output
	@mv $(CURDIR)/Output/050032a5cf12e000.nsp $(CURDIR)/Output/Goldleaf.nsp
	@rm -rf $(CURDIR)/hacpack_backup
	@echo Goldleaf[nsp]: Build finished!

clean:
	@echo Goldleaf[clean]: Cleaning output and build data...
	@rm -fr $(BUILD)
	@rm -fr $(CURDIR)/Output
	@echo Goldleaf[clean]: Clean finished!

else

.PHONY:	all

DEPENDS	:=	$(OFILES:.o=.d)

all: $(OUTPUT).nro $(OUTPUT).nso

$(OUTPUT).nso	:	$(OUTPUT).elf
$(OUTPUT).nro	:	$(OUTPUT).elf
$(OUTPUT).elf	:	$(OFILES)
$(OFILES_SRC)	: $(HFILES_BIN)

-include $(DEPENDS)

endif