ifneq ($(TARGET_SIMULATOR),true)
LOCAL_PATH:= $(call my-dir)


include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := optional
LOCAL_MODULE := libEGL_mali.so
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_MODULE_PATH := $(TARGET_OUT_SHARED_LIBRARIES)/egl
LOCAL_SRC_FILES := $(LOCAL_MODULE)

include $(BUILD_PREBUILT)

include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := optional
LOCAL_MODULE := libGLESv1_CM_mali.so
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_MODULE_PATH := $(TARGET_OUT_SHARED_LIBRARIES)/egl
LOCAL_SRC_FILES := $(LOCAL_MODULE)

include $(BUILD_PREBUILT)

include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := optional
LOCAL_MODULE := libGLESv2_mali.so
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_MODULE_PATH := $(TARGET_OUT_SHARED_LIBRARIES)/egl
LOCAL_SRC_FILES := $(LOCAL_MODULE)

include $(BUILD_PREBUILT)

include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := optional
LOCAL_MODULE := libMali.so
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_MODULE_PATH := $(TARGET_OUT_SHARED_LIBRARIES)
LOCAL_SRC_FILES := $(TARGET_BOARD_PLATFORM)/$(LOCAL_MODULE)

include $(BUILD_PREBUILT)

include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := optional
LOCAL_PRELINK_MODULE := false

LOCAL_C_INCLUDES := \
	$(LOCAL_PATH)/src/ump/include \
	$(LOCAL_PATH)/src/ump/include/ump

LOCAL_SRC_FILES := \
	src/ump/arch_011_udd/ump_frontend.c \
	src/ump/arch_011_udd/ump_ref_drv.c \
	src/ump/arch_011_udd/ump_arch.c \
	src/ump/os/linux/ump_uku.c \
	src/ump/os/linux/ump_osu_memory.c \
	src/ump/os/linux/ump_osu_locks.c

LOCAL_MODULE_TAGS := optional
LOCAL_MODULE := libUMP

LOCAL_ARM_MODE := arm

LOCAL_SHARED_LIBRARIES := \
       libutils \
       libcutils \
       libbinder \
       libdl \
       libandroid_runtime \
       liblog

include $(BUILD_SHARED_LIBRARY)

include $(LOCAL_PATH)/driver/Android.mk
endif
