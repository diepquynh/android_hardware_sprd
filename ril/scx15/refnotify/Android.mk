LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SRC_FILES:= \
	refnotify.c \

LOCAL_SHARED_LIBRARIES := \
    libhardware_legacy \
    libc \
    libutils \
    libcutils \
    liblog \

LOCAL_MODULE := refnotify

LOCAL_MODULE_TAGS := optional

LOCAL_PROPRIETARY_MODULE := true

include $(BUILD_EXECUTABLE)
