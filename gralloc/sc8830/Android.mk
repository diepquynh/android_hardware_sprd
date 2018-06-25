# 
# Copyright (C) 2010 ARM Limited. All rights reserved.
# 
# Copyright (C) 2008 The Android Open Source Project
#
# Copyright (C) 2016 The CyanogenMod Project
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
#

ifneq ($(TARGET_SIMULATOR),true)

LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_PRELINK_MODULE := false

LOCAL_MODULE_RELATIVE_PATH := hw

LOCAL_PROPRIETARY_MODULE := true

LOCAL_MODULE := gralloc.$(TARGET_BOARD_PLATFORM)

LOCAL_MODULE_TAGS := optional

SHARED_MEM_LIBS := \
	libion_sprd \
	libhardware

LOCAL_SHARED_LIBRARIES := \
	liblog \
	libcutils \
	libui \
	libsync \
	libGLESv1_CM \
	$(SHARED_MEM_LIBS) \

LOCAL_STATIC_LIBRARIES := \
	libarect

ifeq ($(TARGET_USES_GRALLOC1), true)
LOCAL_STATIC_LIBRARIES += \
	libgralloc1-adapter
endif

LOCAL_C_INCLUDES := \
	$(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr/include/video/ \
	$(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr/include/ \
	$(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/ \

LOCAL_ADDITIONAL_DEPENDENCIES += \
	$(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr \

LOCAL_EXPORT_C_INCLUDE_DIRS := \
	$(LOCAL_PATH) \
	$(LOCAL_C_INCLUDES) \

LOCAL_CFLAGS := \
	-DLOG_TAG=\"gralloc.$(TARGET_BOARD_PLATFORM)\" \

ifeq ($(TARGET_USES_GRALLOC1), true)
LOCAL_CFLAGS += -DADVERTISE_GRALLOC1
endif

ifeq ($(USE_SPRD_DITHER),true)
LOCAL_CFLAGS += -DSPRD_DITHER_ENABLE
LOCAL_SHARED_LIBRARIES += libdither
endif

ifeq ($(SOC_SCX30G_V2),true)
LOCAL_CFLAGS += -DSCX30G_V2
endif

LOCAL_SRC_FILES := \
	gralloc_module.cpp \
	alloc_device.cpp \
	framebuffer_device.cpp

include $(BUILD_SHARED_LIBRARY)

endif
