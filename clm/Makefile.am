#
# FAP2 command line monitor
#

bin_PROGRAMS = fapm

AM_CPPFLAGS = -D_PLAT_LINUX_ -g -O0

fapm_SOURCES = \
	       monitor.h monitor.cpp main.cpp

# Flags for static linking, used for gprof profiling, ref ds_prfopt_gperf
#fapm_LDFLAGS=-L../src/.libs/ -l:/usr/lib/gcc/x86_64-linux-gnu/5/libgcc.a -l:libgrayb.a -ldl -pg -lrt
fapm_LDFLAGS=-L../src/.libs/ -lgrayb -lrt
fapm_CXXFLAGS=-I@srcdir@/../src -pg -pthread

