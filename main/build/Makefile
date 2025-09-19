DEBUG=0
PSS_STYLE=1
EXTERNAL_ZLIB=0
HAVE_GRIFFIN=1
STATIC_LINKING=0
ENDIANNESS_DEFINES=

SPACE :=
SPACE := $(SPACE) $(SPACE)
BACKSLASH :=
BACKSLASH := \$(BACKSLASH)
filter_out1 = $(filter-out $(firstword $1),$1)
filter_out2 = $(call filter_out1,$(call filter_out1,$1))
unixpath = $(subst \,/,$1)
unixcygpath = /$(subst :,,$(call unixpath,$1))

ifeq ($(platform),)
	platform = unix
	ifeq ($(shell uname -a),)
		platform = win
	else ifneq ($(findstring MINGW,$(shell uname -a)),)
		platform = win
	else ifneq ($(findstring Darwin,$(shell uname -a)),)
		platform = osx
		arch = intel
		ifeq ($(shell uname -p),powerpc)
			arch = ppc
		endif
	else ifneq ($(findstring win,$(shell uname -a)),)
		platform = win
	endif
endif
$(info [libretro-hook] platform=$(platform))

# system platform
system_platform = unix
ifeq ($(shell uname -a),)
	EXE_EXT = .exe
	system_platform = win
else ifneq ($(findstring Darwin,$(shell uname -a)),)
	system_platform = osx
	arch = intel
	ifeq ($(shell uname -p),powerpc)
		arch = ppc
	endif
else ifneq ($(findstring MINGW,$(shell uname -a)),)
	system_platform = win
endif

# Core base name
TARGET_NAME := hook
GIT_VERSION := " $(shell git rev-parse --short HEAD || echo unknown)"
ifneq ($(GIT_VERSION)," unknown")
	CFLAGS += -DGIT_VERSION=\"$(GIT_VERSION)\"
	CXXFLAGS += -DGIT_VERSION=\"$(GIT_VERSION)\"
endif

ifneq (,$(findstring msvc,$(platform)))
LIBM :=
else
LIBM := -lm
endif
LIBS :=

CORE_DIR := ../..

# Unix
ifeq ($(platform), unix)
	TARGET := $(TARGET_NAME)_libretro.so
	fpic := -fPIC
ifneq ($(findstring SunOS,$(shell uname -a)),)
	CC = gcc
	SHARED := -shared -z defs
else
	SHARED := -shared -Wl,--version-script=link.T -Wl,-no-undefined
endif

# Android ARMv7
else ifeq ($(platform), armeabi-v7a)
	TARGET := $(TARGET_NAME)_libretro.so
	JNI_DIR := jni
	NDK_BUILD := $(shell which ndk-build 2>/dev/null || echo /opt/android-ndk-r27c/ndk-build)
	NDK_CFLAGS := -O3 -march=armv7-a -mtune=cortex-a9 -mfpu=neon -mfloat-abi=softfp -flto -ffunction-sections -fdata-sections
	NDK_LDFLAGS := -flto -Wl,--gc-sections -Wl,--strip-all -Wl,--as-needed -Wl,-O1
	PLATFORM_DEFINES += -DANDROID
	STATIC_LINKING=0

# Android ARMv8 (64-bit)
else ifeq ($(platform), arm64-v8a)
	TARGET := $(TARGET_NAME)_libretro.so
	JNI_DIR := jni
	NDK_BUILD := $(shell which ndk-build 2>/dev/null || echo /opt/android-ndk-r27c/ndk-build)
	NDK_CFLAGS := -O3 -march=armv8-a -flto -ffunction-sections -fdata-sections
	NDK_LDFLAGS := -flto -Wl,--gc-sections -Wl,--strip-all -Wl,--as-needed -Wl,-O1
	PLATFORM_DEFINES += -DANDROID
	STATIC_LINKING=0

# Windows Cross-compilation (64-bit)
else ifeq ($(platform), wincross64)
	TARGET := $(TARGET_NAME)_libretro.dll
	CC = x86_64-w64-mingw32-gcc
	CXX = x86_64-w64-mingw32-g++
	AR = x86_64-w64-mingw32-ar
	SHARED := -shared -static-libgcc -static-libstdc++ -s -Wl,--version-script=link.T
	PSS_STYLE := 2
	PLATFORM_DEFINES += -DWIN32
	# Disable glibc-specific fortified symbols which MinGW headers might reference when
	# code was compiled with _FORTIFY_SOURCE on Linux; ensure we don't emit __*_chk calls.
	CFLAGS += -U_FORTIFY_SOURCE -D_FORTIFY_SOURCE=0
	CXXFLAGS += -U_FORTIFY_SOURCE -D_FORTIFY_SOURCE=0
	# Some functions like __ctype_*_loc are not available; use legacy ctype by defining this.
	CFLAGS += -D__USE_MINGW_ANSI_STDIO=1
	CXXFLAGS += -D__USE_MINGW_ANSI_STDIO=1
	# Disable POSIX definitions that cause glibc-specific functions to be used
	CFLAGS += -U_POSIX_C_SOURCE -U_XOPEN_SOURCE -U__USE_POSIX
	CXXFLAGS += -U_POSIX_C_SOURCE -U_XOPEN_SOURCE -U__USE_POSIX

# Default (unsupported platform)
else
	$(error Unsupported platform: $(platform). Supported platforms are: unix, armeabi-v7a, arm64-v8a, wincross64)
endif

ifeq ($(DEBUG), 1)
	ifneq (,$(findstring msvc,$(platform)))
		ifeq ($(STATIC_LINKING),1)
			CFLAGS += -MTd
			CXXFLAGS += -MTd
		else
			CFLAGS += -MDd
			CXXFLAGS += -MDd
		endif

		CFLAGS += -Od -Zi -DDEBUG -D_DEBUG
		CXXFLAGS += -Od -Zi -DDEBUG -D_DEBUG
	else
		CFLAGS += -O0 -g -DDEBUG
		CXXFLAGS += -O0 -g -DDEBUG
	endif
else
	ifneq (,$(findstring msvc,$(platform)))
		ifeq ($(STATIC_LINKING),1)
			CFLAGS += -MT
			CXXFLAGS += -MT
		else
			CFLAGS += -MD
			CXXFLAGS += -MD
		endif

		CFLAGS += -O2 -DNDEBUG
		CXXFLAGS += -O2 -DNDEBUG
	else
		CFLAGS += -O2 -DNDEBUG
		CXXFLAGS += -O2 -DNDEBUG
	endif
endif

ifeq ($(EXTERNAL_ZLIB), 1)
	CFLAGS += -DHAVE_EXTERNAL_ZLIB
	CXXFLAGS += -DHAVE_EXTERNAL_ZLIB
endif

include Makefile.common

OBJECTS := $(SOURCES_C:.c=.o) $(SOURCES_CXX:.cpp=.o)

DEFINES := $(COREDEFINES) $(PLATFORM_DEFINES)

ifeq ($(STATIC_LINKING),1)
	DEFINES += -DSTATIC_LINKING
endif

ifneq (,$(findstring msvc,$(platform)))
WARNING_DEFINES =
LIBM :=
else
WARNING_DEFINES = -Wno-write-strings
endif

CFLAGS   += $(COREDEFINES) $(fpic) $(WARNING_DEFINES) $(DEFINES) $(ENDIANNESS_DEFINES)
CXXFLAGS += $(COREDEFINES) $(fpic) $(WARNING_DEFINES) $(DEFINES) $(ENDIANNESS_DEFINES)
LDFLAGS  += $(LIBM)

ifeq ($(platform), psp1)
	INCFLAGS += -I$(shell psp-config --pspsdk-path)/include
endif


ifneq (,$(findstring msvc,$(platform)))
	OBJOUT = -Fo
	LINKOUT = -out:
ifeq ($(STATIC_LINKING),1)
	LD ?= lib.exe
	STATIC_LINKING=0
else
	LD = link.exe
endif
else
	OBJOUT   = -o
	LINKOUT  = -o
	LD = $(CC)
endif

ifeq ($(platform), armeabi-v7a)
all: $(TARGET)
$(TARGET):
	cd $(JNI_DIR) && $(NDK_BUILD) NDK_DEBUG=0 APP_PLATFORM=android-21 APP_ABI=armeabi-v7a APP_CFLAGS="$(NDK_CFLAGS)" APP_CXXFLAGS="$(NDK_CFLAGS)" APP_LDFLAGS="$(NDK_LDFLAGS)"
	mkdir -p ../generated/armeabi-v7a
	cp libs/armeabi-v7a/libhook.so ../generated/armeabi-v7a/$(TARGET)
else ifeq ($(platform), arm64-v8a)
all: $(TARGET)
$(TARGET):
	cd $(JNI_DIR) && $(NDK_BUILD) NDK_DEBUG=0 APP_PLATFORM=android-28 APP_ABI=arm64-v8a APP_CFLAGS="$(NDK_CFLAGS)" APP_CXXFLAGS="$(NDK_CFLAGS)" APP_LDFLAGS="$(NDK_LDFLAGS)"
	mkdir -p ../generated/arm64-v8a
	cp libs/arm64-v8a/libhook.so ../generated/arm64-v8a/$(TARGET)
else
all: $(TARGET)
$(TARGET): $(OBJECTS)
ifeq ($(STATIC_LINKING),1)
ifneq (,$(findstring msvc,$(platform)))
	$(LD) $(LINKOUT)$@ $(OBJECTS)
else
	$(AR) rcs $@ $(OBJECTS)
endif
else
	$(LD) $(LINKOUT)$@ $(SHARED) $(OBJECTS) $(LDFLAGS) $(LIBS)
endif
	mkdir -p ../generated/$(platform)
	cp $(TARGET) ../generated/$(platform)/

%.o: %.cpp
	$(CXX) -c $(OBJOUT)$@ $< $(CXXFLAGS) $(INCFLAGS)

%.o: %.c
	$(CC) -c $(OBJOUT)$@ $< $(CFLAGS) $(INCFLAGS)
endif

clean:
ifeq ($(platform), armeabi-v7a)
	cd $(JNI_DIR) && $(NDK_BUILD) clean
	rm -f ../generated/armeabi-v7a/$(TARGET)
	rm -f libs/armeabi-v7a/libhook.so
else ifeq ($(platform), arm64-v8a)
	cd $(JNI_DIR) && $(NDK_BUILD) clean
	rm -f ../generated/arm64-v8a/$(TARGET)
	rm -f libs/arm64-v8a/libhook.so
else
	rm -f $(OBJECTS)
	rm -f $(TARGET)
	rm -f ../generated/$(platform)/$(TARGET)
endif

clean-objs:
ifeq ($(platform), armeabi-v7a)
	cd $(JNI_DIR) && $(NDK_BUILD) clean
else ifeq ($(platform), arm64-v8a)
	cd $(JNI_DIR) && $(NDK_BUILD) clean
else
	rm -f $(OBJECTS)
endif

# Platform aliases for easier building
windows64:
	$(MAKE) platform=wincross64

linux:
	$(MAKE) platform=unix

android64:
	$(MAKE) platform=arm64-v8a

android32:
	$(MAKE) platform=armeabi-v7a

.PHONY: clean clean-objs windows64 linux android64 android32 $(TARGET)

print-%:
	@echo '$*=$($*)'
