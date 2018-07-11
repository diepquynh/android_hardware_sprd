LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)
LOCAL_SRC_FILES:= \
	main.c \
    modem_boot.c \
    packet.c \
    crc16.c \
    modem_control.c \
    nv_read.c

LOCAL_SHARED_LIBRARIES := \
	libcutils \
	libhardware_legacy \
    liblog

ifeq ($(strip $(TARGET_USERIMAGES_USE_EXT4)),true)
LOCAL_CFLAGS := -DCONFIG_EMMC
endif

LOCAL_MODULE := modem_control

LOCAL_PROPRIETARY_MODULE := true

LOCAL_MODULE_TAGS := optional

include $(BUILD_EXECUTABLE)
