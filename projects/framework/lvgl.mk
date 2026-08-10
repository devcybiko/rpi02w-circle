# LVGL integration for the Circle framework.
#
# This file lives in projects/framework and builds the LVGL submodule
# without modifying the submodule itself.

LVGLHOME ?= ../../lvgl
LVGLLIB  := $(LVGLHOME)/liblvgl-circle.a

LVGL_SRCS := $(shell find $(LVGLHOME)/src -type f -name '*.c')
LVGL_OBJS := $(patsubst %.c,%.circle.o,$(LVGL_SRCS))

# LVGL needs to find:
#
#   ../../lvgl/lvgl.h
#   ./lv_conf.h
#
LVGL_CFLAGS := $(CFLAGS) \
	-I$(LVGLHOME) \
	-I. \
	-DLV_CONF_INCLUDE_SIMPLE

$(LVGLHOME)/%.circle.o: $(LVGLHOME)/%.c
	@echo "  LVCC  $@"
	$(CC) $(LVGL_CFLAGS) -c $< -o $@

$(LVGLLIB): $(LVGL_OBJS)
	@echo "  AR    $@"
	$(AR) rcs $@ $(LVGL_OBJS)

.PHONY: lvgl
lvgl: $(LVGLLIB)

.PHONY: clean-lvgl
clean-lvgl:
	@find $(LVGLHOME)/src -type f -name '*.circle.o' -delete
	@rm -f $(LVGLLIB)