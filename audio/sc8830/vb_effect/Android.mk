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


LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_CFLAGS := \
	-D_POSIX_SOURCE \
	-Wno-multichar \
	-g

ifneq (,$(filter sc8830 scx15,$(TARGET_BOARD_PLATFORM)))
BOARD_EQ_DIR := v2
else
BOARD_EQ_DIR := v1
endif

LOCAL_SRC_FILES := \
	$(BOARD_EQ_DIR)/vb_effect_if.c \
	$(BOARD_EQ_DIR)/vbc_codec_eq.c \
	$(BOARD_EQ_DIR)/filter_calc.c \
	$(BOARD_EQ_DIR)/vb_hal_if.c \
	$(BOARD_EQ_DIR)/vb_hal_adp.c \

LOCAL_C_INCLUDES := \
	external/tinyalsa/include \
	$(LOCAL_PATH)/../ \
	$(LOCAL_PATH)/../audio/nv_exchange \

LOCAL_EXPORT_C_INCLUDE_DIRS := \
	$(LOCAL_PATH) \
	$(LOCAL_PATH)/$(BOARD_EQ_DIR) \
	$(LOCAL_C_INCLUDES) \

LOCAL_SHARED_LIBRARIES := \
	liblog \
	libc \
	libcutils \
	libtinyalsa \
	libtinyalsautils \
	libnvexchange \

LOCAL_MODULE := libvbeffect

LOCAL_MODULE_TAGS := optional

include $(BUILD_SHARED_LIBRARY)
