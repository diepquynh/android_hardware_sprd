#
# Copyright (C) 2016 The Android Open Source Project
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

LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := libmemoryheapion

LOCAL_PROPRIETARY_MODULE := true

LOCAL_SRC_FILES := \
	MemoryHeapIon.cpp \
	MemoryHeapIon_custom.cpp

ifneq ($(SOC_SCX30G_V2),true)
LOCAL_SRC_FILES += \
	MemoryHeapIon_legacy.cpp
endif

LOCAL_ADDITIONAL_DEPENDENCIES += \
	$(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr

LOCAL_C_INCLUDES += \
	$(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr/include

LOCAL_EXPORT_C_INCLUDE_DIRS := \
	$(LOCAL_PATH)/ \

LOCAL_SHARED_LIBRARIES := \
	liblog \
	libcutils \
	libutils

LOCAL_CFLAGS += -Wno-conversion

ifeq ($(TARGET_UPDATED_MALI),true)
LOCAL_CFLAGS += -DUPDATED_MALI
endif

ifeq ($(SOC_SCX30G_V2),true)
LOCAL_CFLAGS += -DSCX30G_V2
endif

LOCAL_MODULE_TAGS := optional

LOCAL_MODULE_CLASS := SHARED_LIBRARIES

include $(BUILD_SHARED_LIBRARY)
