# Standard devkitPro / libnds template compatible build
ifeq ($(strip $(DEVKITARM)),)
$(error DEVKITARM is not set. Install devkitPro nds-dev and source devkitpro environment)
endif
include $(DEVKITARM)/ds_rules
TARGET := elemental_legends
BUILD := build
SOURCES := source
INCLUDES :=
ARCH := -mthumb -mthumb-interwork
CFLAGS := -g -Wall -Wextra -O2 -ffunction-sections -fdata-sections $(ARCH) $(INCLUDE) -DARM9
CXXFLAGS := $(CFLAGS) -fno-rtti -fno-exceptions
ASFLAGS := -g $(ARCH)
LDFLAGS := -specs=ds_arm9.specs -g $(ARCH) -Wl,-Map,$(notdir $*.map)
LIBS := -lfat -lnds9
LIBFAT ?= $(DEVKITPRO)/libfat
LIBDIRS := $(LIBNDS) $(LIBFAT)
ifneq ($(BUILD),$(notdir $(CURDIR)))
export OUTPUT := $(CURDIR)/$(TARGET)
export VPATH := $(foreach dir,$(SOURCES),$(CURDIR)/$(dir))
export DEPSDIR := $(CURDIR)/$(BUILD)
export INCLUDE := $(foreach dir,$(INCLUDES),-I$(CURDIR)/$(dir)) -I$(CURDIR)/$(BUILD)
export LIBPATHS := $(foreach dir,$(LIBDIRS),-L$(dir)/lib)
export OFILES := $(addsuffix .o,$(basename $(notdir $(wildcard $(SOURCES)/*.c))))
.PHONY: $(BUILD) clean
$(BUILD):
	@mkdir -p $@
	@$(MAKE) --no-print-directory -C $(BUILD) -f $(CURDIR)/Makefile
clean:
	rm -rf $(BUILD) $(TARGET).nds $(TARGET).elf
else
DEPENDS := $(OFILES:.o=.d)
$(OUTPUT).nds: $(OUTPUT).elf
$(OUTPUT).elf: $(OFILES)
-include $(DEPENDS)
endif
