#
#
# Copyright (C) 2009 The Android Open Source Project
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

ifeq ($(TARGET_BOARD_PLATFORM),sc8830)

sc8830_dirs += \
	audio \
	dither \
	gralloc \
	hwcomposer \
	libcamera \
	libmemoryheapion_sprd \
	libmemtrack \
	libstagefrighthw \
	omx_components

# HW rendering
USE_SPRD_HWCOMPOSER := true
USE_OVERLAY_COMPOSER_GPU := true
TARGET_FORCE_HWC_FOR_VIRTUAL_DISPLAYS := true

include $(call all-named-subdir-makefiles,$(sc8830_dirs))

endif
