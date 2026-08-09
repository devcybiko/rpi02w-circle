# Base application for Raspberry Pi Zero W / Raspberry Pi 2-class 32-bit builds.

CIRCLEHOME = ../../circle
RASPPI = 3
AARCH = 32

OBJS = main.o kernel.o core.o core0.o core1.o core2.o core3.o \
	   eventqueue.o eventrouter.o executor.o hidservice.o loggerservice.o \
	   screenservice.o serialservice.o service.o storageservice.o webserver.o \
	   webserverservice.o

DEFINE += -DARM_ALLOW_MULTI_CORE

LIBS = $(CIRCLEHOME)/addon/wlan/hostap/wpa_supplicant/libwpa_supplicant.a \
	  $(CIRCLEHOME)/addon/wlan/libwlan.a \
	  $(CIRCLEHOME)/addon/fatfs/libfatfs.a \
	  $(CIRCLEHOME)/addon/SDCard/libsdcard.a \
	  $(CIRCLEHOME)/lib/net/libnet.a \
	  $(CIRCLEHOME)/lib/sched/libsched.a \
	  $(CIRCLEHOME)/lib/usb/libusb.a \
	  $(CIRCLEHOME)/lib/input/libinput.a \
	  $(CIRCLEHOME)/lib/fs/libfs.a \
	  $(CIRCLEHOME)/lib/libcircle.a

include $(CIRCLEHOME)/Rules.mk

.PHONY: clean-all

clean-all: clean
	cd $(CIRCLEHOME) && ./makeall --nosample clean
	$(MAKE) -C $(CIRCLEHOME)/addon/SDCard clean
	$(MAKE) -C $(CIRCLEHOME)/addon/fatfs clean
	cd $(CIRCLEHOME)/addon/wlan && ./makeall --nosample clean

-include $(DEPS)
