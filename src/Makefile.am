
AUTOMAKE_OPTIONS = subdir-objects
ACLOCAL_AMFLAGS = $(ACLOCAL_FLAGS)# $(AM_CXXFLAGS)
bin_PROGRAMS = plan
plan_SOURCES = main_new.cpp PDFUtils.cpp Application.cpp emeocv/ImageProcessor.cpp \
 emeocv/Directory.cpp emeocv/ImageInput.cpp emeocv/Config.cpp \
 emeocv/KNearestOcr.cpp emeocv/Plausi.cpp emeocv/TextDetection.cpp \
 ROIUtils.cpp

plan_LDADD = $(DEPS_LIBS) libImgui.la
plan_DEPENDENCIES =  libImgui.la


plan_CPPFLAGS = $(DEPS_CFLAGS) \
 -I../third_party/repos/imgui \
 -I../third_party/repos/imgui/examples/opengl3_example \
 -I../third_party/repos/imgui/examples/opengl2_example \
 -I../include

#plan_LDADD += $(DEPS_LIBS) libImgui.la

#lib_LTLIBRARIES = libPlan.la

#libPlan_la_LDFLAGS = -version-info 0:0:0
#libPlan_la_SOURCES = plan.cpp plan.hpp

# ImGui Library
noinst_LTLIBRARIES = libImgui.la
noinst_HEADERS = ../third_party/repos/imgui/imgui.h ../third_party/repos/imgui/examples/opengl3_example/imgui_impl_glfw_gl3.h ../third_party/repos/imgui/examples/opengl2_example/imgui_impl_glfw_gl2.h
libImgui_la_LDFLAGS = -static
libImgui_la_SOURCES = ../third_party/repos/imgui/imgui.cpp \
 ../third_party/repos/imgui/imgui_draw.cpp \
 ../third_party/repos/imgui/examples/opengl3_example/imgui_impl_glfw_gl3.cpp \
 ../include/imguifilesystem.cpp

#../third_party/repos/imgui/examples/opengl2_example/imgui_impl_glfw_gl2.cpp

libImgui_la_CPPFLAGS = $(GL_CFLAGS) $(AM_CXXFLAGS)

libImgui_la_CPPFLAGS += -I ../third_party/repos/imgui -I ../third_party/repos/imgui/examples/libs/glfw/ -I ../third_party/repos/ #-std=c++17

