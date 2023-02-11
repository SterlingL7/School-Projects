# ~/git/courses/ece3220/share/toolchain-jessie.mk
# Copyright 2020 James D. Fischer.  All rights reserved.
#
#  toolchain-jessie.mk
#

ifndef TOOLCHAIN.MK
TOOLCHAIN.MK := 1

# If the build system's CPU type is `armv7l', then assume Make was invoked on
# the Raspberry Pi (RPI) and we're building with the RPI's native toolchain.
# Otherwise, the build host is not the RPI--e.g., Make was invoked on a desktop
# computer--and therefore we're building with a cross toolchain.
#
TOOLCHAIN__BUILD_CPU_ARCH := $(shell lscpu | grep 'Architecture:' | sed 's/Architecture:[[:space:]]*//')
ifneq ($(TOOLCHAIN__BUILD_CPU_ARCH),armv7l)

	#
	# The BUILD CPU's architecture is NOT `armv7l'; therefore, use the cross
	# toolchain to perform the build.
	#

	# Cross toolchain installation path, bin directory, and program prefix
	TOOLCHAIN__CROSS_BASEDIR := /usr/local/rpicross/jessie/
	TOOLCHAIN__CROSS_BINDIR  := $(TOOLCHAIN__CROSS_BASEDIR)/bin
	TOOLCHAIN__CROSS_PREFIX  := arm-linux-gnueabihf-

	# Redefine Make's implicit variables so that Make uses the cross
	# toolchain's C/C++ compilers, not the native toolchain's C/C++ compilers.
	AR := $(TOOLCHAIN__CROSS_BINDIR)/$(TOOLCHAIN__CROSS_PREFIX)ar
	AS := $(TOOLCHAIN__CROSS_BINDIR)/$(TOOLCHAIN__CROSS_PREFIX)as
	CC := $(TOOLCHAIN__CROSS_BINDIR)/$(TOOLCHAIN__CROSS_PREFIX)gcc
	CXX := $(TOOLCHAIN__CROSS_BINDIR)/$(TOOLCHAIN__CROSS_PREFIX)g++
	LD := $(TOOLCHAIN__CROSS_BINDIR)/$(TOOLCHAIN__CROSS_PREFIX)ld

	# Other programs that are part of the cross toolchain.
	# NB: Make does not define implicit variables for these programs.
	ADDR2LINE := $(TOOLCHAIN__CROSS_BINDIR)/$(TOOLCHAIN__CROSS_PREFIX)addr2line
	CXXFILT := $(TOOLCHAIN__CROSS_BINDIR)/$(TOOLCHAIN__CROSS_PREFIX)c++filt
	GCOV := $(TOOLCHAIN__CROSS_BINDIR)/$(TOOLCHAIN__CROSS_PREFIX)gcov
	GPROF := $(TOOLCHAIN__CROSS_BINDIR)/$(TOOLCHAIN__CROSS_PREFIX)gprof
	NM := $(TOOLCHAIN__CROSS_BINDIR)/$(TOOLCHAIN__CROSS_PREFIX)nm
	OBJCOPY := $(TOOLCHAIN__CROSS_BINDIR)/$(TOOLCHAIN__CROSS_PREFIX)objcopy
	OBJDUMP := $(TOOLCHAIN__CROSS_BINDIR)/$(TOOLCHAIN__CROSS_PREFIX)objdump
	RANLIB := $(TOOLCHAIN__CROSS_BINDIR)/$(TOOLCHAIN__CROSS_PREFIX)ranlib
	READELF := $(TOOLCHAIN__CROSS_BINDIR)/$(TOOLCHAIN__CROSS_PREFIX)readelf
	SIZE := $(TOOLCHAIN__CROSS_BINDIR)/$(TOOLCHAIN__CROSS_PREFIX)size
	STRINGS := $(TOOLCHAIN__CROSS_BINDIR)/$(TOOLCHAIN__CROSS_PREFIX)strings
	STRIP := $(TOOLCHAIN__CROSS_BINDIR)/$(TOOLCHAIN__CROSS_PREFIX)strip

endif

endif # ifndef TOOLCHAIN.MK

