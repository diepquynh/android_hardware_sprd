#
# Copyright (C) 2012 The Android Open Source Project
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

# codecs
PRODUCT_PACKAGES := \
	libstagefright_sprd_mpeg4dec \
	libstagefright_sprd_mpeg4enc \
	libstagefright_sprd_h264dec \
	libstagefright_sprd_h264enc \
	libstagefright_sprd_vpxdec \
	libstagefright_sprd_aacdec \
	libstagefright_sprd_mp3dec

# stagefright and device specific modules
PRODUCT_PACKAGES += \
	libstagefrighthw

# hw composer HAL
PRODUCT_PACKAGES += \
	gralloc.sc8830 \
	hwcomposer.sc8830 \
	sprd_gsp.sc8830

# Audio HAL
PRODUCT_PACKAGES += \
	audio.primary.sc8830 \
	audio_policy.sc8830

# Memory Tracker HAL
PRODUCT_PACKAGES += \
	memtrack.sc8830
