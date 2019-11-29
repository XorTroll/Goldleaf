.SUFFIXES:

ifeq ($(strip $(DEVKITPRO)),)
$(error "Please set DEVKITPRO in your environment. export DEVKITPRO=<path to>/devkitpro")
endif

TOPDIR ?= $(CURDIR)
include $(DEVKITPRO)/libnx/switch_rules

VER_MAJOR	:= 0
VER_MINOR	:= 7
VER_MICRO	:= 3

APP_TITLE   := Goldleaf
APP_AUTHOR  := XorTroll

# "-dev" prefix is appended when a new version has started development, and when it's close to release, the actual version number is increased.
APP_VERSION := $(VER_MAJOR).$(VER_MINOR).$(VER_MICRO)-dev
APP_TITLEID	:= 050032A5CF12E000

TARGET      := Goldleaf
ICON		:= Icon.jpg

BUILD       := Build
SOURCES     := Source Source/fatfs Source/acc Source/dump Source/err Source/es Source/fs Source/hos Source/ncm Source/net Source/ns Source/nsp Source/set Source/nfp Source/ui Source/usb
INCLUDES    := Include Include/fatfs Include/acc Include/dump Include/err Include/es Include/fs Include/hos Include/ncm Include/net Include/nfp Include/ns Include/nsp Include/set Include/ui Include/usb
ROMFS       := RomFs

ARCH	:=	-march=armv8-a -mtune=cortex-a57 -mtp=soft -fPIE

CFLAGS	:=	-g -Wall -O2 -ffunction-sections \
			$(ARCH) $(DEFINES)

CFLAGS	+=	$(INCLUDE) -D__SWITCH__ -DGOLDLEAF_MAJOR=$(VER_MAJOR) -DGOLDLEAF_MINOR=$(VER_MINOR) -DGOLDLEAF_MICRO=$(VER_MICRO) -DGOLDLEAF_VERSION=\"$(APP_VERSION)\" -DGOLDLEAF_APPID=0x$(APP_TITLEID)

CXXFLAGS	:= $(CFLAGS) -fno-rtti -fexceptions -std=gnu++17

ASFLAGS	:=	-g $(ARCH)
LDFLAGS	=	-specs=$(DEVKITPRO)/libnx/switch.specs -g $(ARCH) -Wl,-Map,$(notdir $*.map)

LIBS	:=  -lcurl -lz -lmbedtls -lmbedcrypto -lmbedx509 -lnx -lpu -lfreetype -lSDL2_mixer -lopusfile -lopus -lmodplug -lmpg123 -lvorbisidec -lSDL2 -lc -logg -lSDL2_ttf -lSDL2_gfx -lSDL2_image -lwebp -lpng -ljpeg `sdl2-config --libs` `freetype-config --libs`
LIBDIRS	:= $(PORTLIBS) $(LIBNX) $(CURDIR)/../Plutonium/Plutonium/Output

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

#---------------------------------------------------------------------------------
# use CXX for linking C++ projects, CC for standard C
#---------------------------------------------------------------------------------
ifeq ($(strip $(CPPFILES)),)
#---------------------------------------------------------------------------------
	export LD	:=	$(CC)
#---------------------------------------------------------------------------------
else
#---------------------------------------------------------------------------------
	export LD	:=	$(CXX)
#---------------------------------------------------------------------------------
endif
#---------------------------------------------------------------------------------

export OFILES_BIN	:=	$(addsuffix .o,$(BINFILES))
export OFILES_SRC	:=	$(CPPFILES:.cpp=.o) $(CFILES:.c=.o) $(SFILES:.s=.o)
export OFILES 	:=	$(OFILES_BIN) $(OFILES_SRC)
export HFILES_BIN	:=	$(addsuffix .h,$(subst .,_,$(BINFILES)))

export INCLUDE	:=	$(foreach dir,$(INCLUDES),-I$(CURDIR)/$(dir)) \
			$(foreach dir,$(LIBDIRS),-I$(dir)/include) \
			-I$(CURDIR)/$(BUILD)

export LIBPATHS	:=	$(foreach dir,$(LIBDIRS),-L$(dir)/lib)

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
	export NROFLAGS += --nacp=$(CURDIR)/Output/$(TARGET).nacp
endif

ifneq ($(APP_TITLEID),)
	export NACPFLAGS += --titleid=$(APP_TITLEID)
endif

ifneq ($(ROMFS),)
	export NROFLAGS += --romfsdir=$(CURDIR)/$(ROMFS)
endif

.PHONY: $(BUILD) clean all

#---------------------------------------------------------------------------------
all: $(BUILD)

$(BUILD):
	@[ -d $@ ] || mkdir -p $@
	@[ -d Output ] || mkdir -p Output
	@echo Goldleaf: Building Plutonium...
	@$(MAKE) --no-print-directory -C ../Plutonium/Plutonium/
	@echo Goldleaf: Plutonium compiled! Starting Goldleaf build...
	@$(MAKE) --no-print-directory -C $(BUILD) -f $(CURDIR)/Makefile
	@echo Goldleaf: Done!

#---------------------------------------------------------------------------------
clean:
	@echo Goldleaf: Cleaning...
	@$(MAKE) clean -C ../Plutonium/Plutonium/
	@rm -fr $(BUILD)
	@rm -fr $(CURDIR)/Output
	@echo Goldleaf: Done!

#---------------------------------------------------------------------------------
else
.PHONY:	all

DEPENDS	:=	$(OFILES:.o=.d)

all: $(OUTPUT).nro

ifeq ($(strip $(NO_NACP)),)
$(OUTPUT).nro	:	$(OUTPUT).elf $(OUTPUT).nacp
else
$(OUTPUT).nro	:	$(OUTPUT).elf
endif

$(OUTPUT).elf	:	$(OFILES)

$(OFILES_SRC)	: $(HFILES_BIN)

#---------------------------------------------------------------------------------
# you need a rule like this for each extension you use as binary data
#---------------------------------------------------------------------------------
%.bin.o	%_bin.h :	%.bin
#---------------------------------------------------------------------------------
	@echo $(notdir $<)
	@$(bin2o)

-include $(DEPENDS)

#---------------------------------------------------------------------------------------
endif
#---------------------------------------------------------------------------------------