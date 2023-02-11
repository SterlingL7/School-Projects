#
#  pthreads.mk
#
#  2019-05-02 Jim Fischer <fischerjd@missouri.edu>
#

ifndef PTHREADS.MK
PTHREADS.MK := 1

# GNU C/C++'s `-pthread' option.  If compiling and linking are performed in
# separate steps, the `-pthread' option needs to be supplied via make's CFLAGS,
# CXXFLAGS, and LDFLAGS variables.
CFLAGS   += -pthread
CXXFLAGS += -pthread
LDFLAGS  += -pthread

endif # PTHREADS.MK

