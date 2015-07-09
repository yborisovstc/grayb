bin_PROGRAMS = ut-grayb-lib

AM_CPPFLAGS = -D_PLAT_LINUX_ -g -O0

ut_grayb_lib_SOURCES = \
		       ut_grayb_main.cpp \
		       ut_mut.cpp \
		       ut_module.cpp \
		       ut_cre.cpp \
		       ut_conn.cpp \
		       ut_uri.cpp \
		       ut_fun.cpp \
		       ut_icache.cpp \
		       ut_des.cpp

ut_grayb_lib_LDFLAGS=-L../src/.libs/ -lgrayb  -lcppunit -lrt
#ut_grayb_lib_LDFLAGS=-L../src/.libs/ -lgrayb  -lcppunit
ut_grayb_lib_CXXFLAGS=-I@srcdir@/../src

check: test_grayb_lib

test_grayb_lib:
	 ./ut-grayb-lib


