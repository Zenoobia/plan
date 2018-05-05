
AUTOMAKE_OPTIONS = subdir-objects
ACLOCAL_AMFLAGS = $(ACLOCAL_FLAGS)

 bin_PROGRAMS = plan
plan_SOURCES = main.cpp
plan_LDADD = libPlan.la libImgui.la
plan_DEPENDENCIES = libImgui.a

plan_CPPFLAGS = $(DEPS_CFLAGS) \
 -I../third_party/repos/imgui \
 -I../third_party/repos/imgui/examples/opengl3_example \
 -I../third_party/repos/imgui/examples/opengl2_example \
 -I../include


plan_LDADD += $(DEPS_LIBS)# libImgui.la

lib_LTLIBRARIES = libPlan.la libImgui.la

libPlan_la_LDFLAGS = -version-info 0:0:0
libPlan_la_SOURCES = plan.cpp plan.hpp
libPlan_la_CPPFLAGS = -I../third_party/repos/imgui \
 -I../third_party/repos/imgui/examples/opengl3_example \
 -I../third_party/repos/imgui/examples/opengl2_example \
 -I../include

# IMGUI
include $(top_srcdir)/Makefile.common

#noinst_LTLIBRARIES = libImgui.la
libImgui_la_LDFLAGS = -version-info 0:0:0
libImgui_la_SOURCES = $(top_srcdir)/third_party/repos/imgui/imgui.cpp $(top_srcdir)/third_party/repos/imgui/imgui_draw.cpp $(top_srcdir)/third_party/repos/imgui/examples/opengl3_example/imgui_impl_glfw_gl3.cpp $(top_srcdir)/third_party/repos/imgui/examples/opengl2_example/imgui_impl_gl2.cpp

libImgui_la_CPPFLAGS = $(GL_CFLAGS)

libImgui_la_CPPFLAGS += -I$(top_srcdir)/third_party/repos/imgui -I$(top_srcdir)/third_party/repos/imgui/examples/libs/glfw/ -I$(top_srcdir)/third_party/repos/ -I$(top_srcdir)/src/ -std=c++11

noinst_HEADERS = $(top_srcdir)/third_party/repos/imgui.h $(top_srcdir)/third_party/repos/examples/sdl_opengl3_example/imgui_impl_sdl_gl3.h $(top_srcdir)/third_party/repos/examples/sdl_opengl2_example/imgui_impl_sdl.h


