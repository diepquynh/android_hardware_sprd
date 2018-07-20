LOCAL_PATH := $(call my-dir)

ifeq ($(TARGET_BOARD_PLATFORM),sc8830)
include $(call all-makefiles-under,$(LOCAL_PATH))
endif
