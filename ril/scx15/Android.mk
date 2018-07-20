LOCAL_PATH := $(call my-dir)

ifeq ($(TARGET_BOARD_PLATFORM),scx15)
include $(call all-makefiles-under,$(LOCAL_PATH))
endif
