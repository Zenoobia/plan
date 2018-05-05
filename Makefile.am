AUTOMAKE_OPTIONS = foreign
ACLOCAL_AMFLAGS = -I m4 -I third_part/repos/imgui
AM_CXXFLAGS = -std=c++17 -fpic -Wall -Wextra

if DEBUG
AM_CXXFLAGS +=-g3 -O0
else
AM_CXXFLAGS += -O2
endif


SUBDIRS = src doc examples man scripts
#third_party/repos/imgui

