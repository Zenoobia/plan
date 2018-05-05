
AUTOMAKE_OPTIONS = subdir-objects
ACLOCAL_AMFLAGS = $(ACLOCAL_FLAGS)

bin_PROGRAMS = plan
plan_SOURCES = main.cpp
plan_LDADD = libPlan.la

plan_CFLAGS = $(DEPS_CFLAGS)
plan_LDADD += $(DEPS_LIBS)

lib_LTLIBRARIES = libPlan.la
libPlan_la_LDFLAGS = -version-info 0:0:0
libPlan_la_SOURCES = plan.cpp plan.hpp
