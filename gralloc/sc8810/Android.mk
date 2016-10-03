# 
# Copyright (C) 2010 ARM Limited. All rights reserved.
# 
# Copyright (C) 2008 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.


LOCAL_PATH := $(call my-dir)

# HAL module implemenation, not prelinked and stored in
# hw/<OVERLAY_HARDWARE_MODULE_ID>.<ro.product.board>.so
include $(CLEAR_VARS)
LOCAL_PRELINK_MODULE := false
LOCAL_MODULE_PATH := $(TARGET_OUT_SHARED_LIBRARIES)/hw

MALI_DDK_PATH := hardware/samsung/sprd/

LOCAL_MODULE := gralloc.$(TARGET_BOARD_PLATFORM)
LOCAL_MODULE_TAGS := optional

# Which DDK are we building for?
ifneq (,$(wildcard $(MALI_DDK_PATH)/ump/))
# Mali-T6xx DDK
LOCAL_SHARED_LIBRARIES := liblog libcutils libGLESv1_CM

# All include files are accessed from the DDK root
DDK_PATH := $(LOCAL_PATH)/../../..
UMP_HEADERS_PATH := $(DDK_PATH)/kernel/include
LOCAL_C_INCLUDES := $(DDK_PATH) $(UMP_HEADERS_PATH)

LOCAL_CFLAGS:= -DLOG_TAG=\"gralloc.$(TARGET_BOARD_PLATFORM)\"
# -DGRALLOC_16_BITS -DSTANDARD_LINUX_SCREEN
else
# Mali-200/300/400MP DDK
SHARED_MEM_LIBS := libUMP #libion
LOCAL_SHARED_LIBRARIES := liblog libcutils libGLESv1_CM $(SHARED_MEM_LIBS)

# Include the UMP header files
LOCAL_C_INCLUDES := $(MALI_DDK_PATH)/mali/sc8810/src/ump/include system/core/include/
LOCAL_C_INCLUDES += \
    $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr/include/video/ \
    $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr/include/ \
	$(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/
	
LOCAL_CFLAGS:= -DLOG_TAG=\"gralloc.$(TARGET_BOARD_PLATFORM)\" -w 
		
ifneq ($(TARGET_LCD_SIZE),)
LOCAL_CFLAGS += -DTARGET_LCD_SIZE=$(TARGET_LCD_SIZE)
endif

ifneq ($(TARGET_LCD_WIDTH),)
LOCAL_CFLAGS += -DTARGET_LCD_WIDTH=$(TARGET_LCD_WIDTH)
endif

ifneq ($(TARGET_LCD_HEIGHT),)
LOCAL_CFLAGS += -DTARGET_LCD_HEIGHT=$(TARGET_LCD_HEIGHT)
endif

ifneq ($(TARGET_SCREEN_HEIGHT),)
LOCAL_CFLAGS += -DTARGET_SCREEN_HEIGHT=$(TARGET_SCREEN_HEIGHT)
endif

ifneq ($(TARGET_SCREEN_WIDTH),)
LOCAL_CFLAGS += -DTARGET_SCREEN_WIDTH=$(TARGET_SCREEN_WIDTH)
endif

# -DGRALLOC_32_BITS -DSTANDARD_LINUX_SCREEN
endif

ifeq ($(strip $(USE_UI_OVERLAY)),true)
        LOCAL_CFLAGS += -DUSE_UI_OVERLAY
endif

LOCAL_SRC_FILES := \
	gralloc_module.cpp \
	alloc_device.cpp \
	framebuffer_device.cpp

#LOCAL_CFLAGS+= -DMALI_VSYNC_EVENT_REPORT_ENABLE
include $(BUILD_SHARED_LIBRARY)
