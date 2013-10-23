#############################################
# module : freetype
# desc : auto gen by hjj
#############################################


LOCAL_PATH := $(call my-dir)

MODULE_freetype_SRC_FILES := \
	src/base/ftsystem.c \
	src/base/ftinit.c \
	src/base/ftdebug.c \
	src/base/ftbase.c \
	src/base/ftbbox.c \
	src/base/ftglyph.c \
	src/base/ftbdf.c \
	src/base/ftbitmap.c \
	src/base/ftcid.c \
	src/base/ftfstype.c \
	src/base/ftgasp.c  \
	src/base/ftgxval.c \
	src/base/ftlcdfil.c \
	src/base/ftmm.c  \
	src/base/ftotval.c \
	src/base/ftpatent.c \
	src/base/ftpfr.c  \
	src/base/ftstroke.c   \
	src/base/ftsynth.c \
	src/base/fttype1.c \
	src/base/ftwinfnt.c \
	src/base/ftxf86.c \
	src/bdf/bdf.c \
	src/cid/type1cid.c \
	src/cff/cff.c \
	src/pcf/pcf.c \
	src/pfr/pfr.c \
	src/sfnt/sfnt.c \
	src/truetype/truetype.c \
	src/type1/type1.c \
	src/type42/type42.c \
	src/winfonts/winfnt.c \
	src/raster/raster.c \
	src/smooth/smooth.c \
	src/autofit/autofit.c \
	src/cache/ftcache.c \
	src/gzip/ftgzip.c \
	src/lzw/ftlzw.c \
	src/gxvalid/gxvalid.c \
	src/otvalid/otvalid.c \
	src/psaux/psaux.c \
	src/pshinter/pshinter.c \
	src/psnames/psnames.c

MODULE_freetype_INCLUDES := \
	$(LOCAL_PATH)/include \
	$(LOCAL_PATH)/include/freetype

MODULE_freetype_CPP_FEATURES := \
	rtti \
	exception

MODULE_freetype_C_FLAGS := \
	-W -g \
	-DFT2_BUILD_LIBRARY

#############################################
# static module : freetype
#############################################
include $(CLEAR_VARS)

LOCAL_MODULE := freetype

LOCAL_SRC_FILES := $(MODULE_freetype_SRC_FILES)

LOCAL_EXPORT_C_INCLUDES := $(MODULE_freetype_INCLUDES)

LOCAL_C_INCLUDES := $(MODULE_freetype_INCLUDES)

LOCAL_CPP_FEATURES := $(MODULE_freetype_CPP_FEATURES)

LOCAL_CFLAGS := $(MODULE_freetype_C_FLAGS)

LOCAL_CPPFLAGS := $(MODULE_freetype_C_FLAGS)

LOCAL_EXPORT_CFLAGS := $(MODULE_freetype_C_FLAGS)

LOCAL_EXPORT_CPPFLAGS := $(MODULE_freetype_C_FLAGS)

LOCAL_ARM_MODE := arm

TARGET_ARCH := arm

LOCAL_ALLOW_UNDEFINED_SYMBOLS := false

include $(BUILD_STATIC_LIBRARY)

