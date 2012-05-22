bin_PROGRAMS = ut-grayb-lib

ut_grayb_lib_SOURCES = \
		       ut_grayb_main.cpp \
		       ut_cre.cpp \
		       ut_uri.cpp \
		       ut_mut.cpp \
		       ut_fun.cpp \
		       ut_des.cpp 

AM_CPPFLAGS = -D_PLAT_LINUX_ -g -O0
ut_grayb_lib_LDFLAGS=-L../src/ -lgrayb  -lcppunit
#ut_grayb_lib_LDFLAGS=-L@srcdir@/../src/.libs -lgrayb -lcppunit
ut_grayb_lib_CXXFLAGS=-I@srcdir@/../src

check: test_grayb_lib

test_grayb_lib:
	 ./ut-grayb-lib


