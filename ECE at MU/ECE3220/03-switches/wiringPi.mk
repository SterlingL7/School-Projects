# ~/git/courses/ece3220/share/wiringPi.mk
# Copyright 2019-2020 James D. Fischer.  All rights reserved.
#
#  wiringPi.mk
#
# When building with a cross toolchain, the toolchain requires access to
# the wiringPi header files and library files which are installed on the
# RPI's filesystem. The easiest way to manage this is to mount the root
# directory '/' of the RPI's filesystem onto any convenient folder within
# the local computer's filesystem--e.g., ${HOME}/rpifs/--prior to invoking
# `make'. Define a makefile variable `RPIFS' whose value is the path to the
# folder on the local filesystem where you've mounted the RPS's filesystem
# (the "mount point").
#
# Depending on how the wiringPi software was installed, the wiringPi header
# files and library files are (typically) located in one of two locations:
#
# 	/usr/include
# 	/usr/lib
#
# 	OR
#
# 	/usr/local/include
# 	/usr/local/lib
#
# If the wiringPi header files and libraries are found in folders
# /usr/local/include and /usr/local/lib, respectively, this makefile uses
# those headers/libraries (PREFIX:=/usr/local); otherwise, this makefile
# uses the header files and libraries found in folders /usr/include and
# /usr/lib (PREFIX:=/usr).
#
# Within this Makefile, prepend the makefile variable expansion $(RPIFS) to
# any path string that defines the path to a folder within the RPI's
# filesystem that contains the wiringPi header files or library files:
#
#	wiringPi header files  > $(RPIFS)$(PREFIX)/include/
#	wiringPi library files > $(RPIFS)$(PREFIX)/lib/
#
# where PREFIX is either '/usr/local' or '/usr', as described above. Note
# that when building with the RPI's native toolchain, the variable RPIFS
# must be undefined so that it expands to the empty string:
#
# BUILDING WITH A CROSS C/C++           BUILDING WITH THE NATIVE C/C++
# TOOLCHAIN ON A DESKTOP COMPUTER       TOOLCHAIN ON THE RASPBERRY PI
# $(RPIFS)=="${HOME}/ripfs"             $(RPIFS)==""
# ===============================       ===================
# ${HOME}/rpifs$(PREFIX)/include        $(PREFIX)/include
# ${HOME}/rpifs$(PREFIX)/lib            $(PREFIX)/lib
#

ifndef WIRINGPI.MK
WIRINGPI.MK = 1

WIRINGPI__BUILD_CPU_ARCH := $(shell lscpu | grep 'Architecture:' | sed 's/Architecture:[[:blank:]]*//')
ifneq ($(WIRINGPI__BUILD_CPU_ARCH),armv7l)
	# The build CPU's architecture is not 'armv7l'; therefore, assume we're
	# cross compiling on a desktop computer, AND the root folder '/' of the
	# Raspberry Pi's filesystem is mounted onto this folder: $HOME/rpifs/
	RPIFS = "${HOME}/rpifs"
endif

# Default PREFIX := /usr/local
WIRINGPI__INCLUDEDIR := $(RPIFS)/usr/local/include
WIRINGPI__LIBDIR := $(RPIFS)/usr/local/lib
ifeq (,$(wildcard $(WIRINGPI__INCLUDEDIR)/wiringPi.h))
# PREFIX := /usr
WIRINGPI__INCLUDEDIR := $(RPIFS)/usr/include
WIRINGPI__LIBDIR := $(RPIFS)/usr/lib
endif

# Preprocessor options.  Specify the path to the folder where the wiringPi
# library's header files are stored.
CPPFLAGS += -I$(WIRINGPI__INCLUDEDIR)

# GCC linker/loader (ld) options
# -L Specify the path to the folder that contains the library file
#  `libwiringPi.so.*'.
# -pthread Link the program with Linux's pthread library.
# -l Specify the name of the library we want to link our program to.  The
#  wiringPi library's file name is `libwiringPi.so'; therefore, the
#  library's name is `wiringPi' (strip off the `lib' prefix and the `.so.*'
#  suffix).
LDFLAGS += -L$(WIRINGPI__LIBDIR)
LDLIBS += -lwiringPi

# The wiringPi library requires Linux pthreads support
ifneq (,$(wildcard pthreads.mk))
include pthreads.mk
endif

endif # WIRINGPI.MK

