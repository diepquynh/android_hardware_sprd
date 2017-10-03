# add lib

include $(shell find $(LOCAL_PATH) -name 'SprdSharedLib.mk')

######################################################
######################################################

include $(shell find $(LOCAL_PATH) -name 'SprdPreLib.mk')

ifeq ($(strip $(TARGET_BOARD_CAMERA_UV_DENOISE)),true)
include $(CLEAR_VARS)
LOCAL_MODULE := libuvdenoise
LOCAL_SRC_FILES := libs/libuvdenoise.so
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_SUFFIX := .so
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_MODULE_PATH := $(TARGET_OUT)/lib
include $(BUILD_PREBUILT)
endif

ifeq ($(strip $(TARGET_BOARD_CAMERA_Y_DENOISE)),true)
include $(CLEAR_VARS)
LOCAL_MODULE := libynoise
LOCAL_SRC_FILES := libs/libynoise.so
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_SUFFIX := .so
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_MODULE_PATH := $(TARGET_OUT)/lib
include $(BUILD_PREBUILT)
endif

ifeq ($(strip $(TARGET_BOARD_CAMERA_SNR_UV_DENOISE)),true)
include $(CLEAR_VARS)
LOCAL_MODULE := libcnr
LOCAL_SRC_FILES := libs/libcnr.so
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_SUFFIX := .so
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_MODULE_PATH := $(TARGET_OUT)/lib
include $(BUILD_PREBUILT)
endif

ifeq ($(strip $(TARGET_BOARD_CAMERA_FACE_BEAUTY)),false)
else
include $(CLEAR_VARS)
LOCAL_MODULE := libts_face_beautify_hal
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_SUFFIX := .so
ifeq ($(strip $(TARGET_ARCH)),arm64)
LOCAL_MULTILIB := both
LOCAL_MODULE_STEM_32 := libts_face_beautify_hal.so
LOCAL_MODULE_STEM_64 := libts_face_beautify_hal.so
LOCAL_SRC_FILES_32 :=  arithmetic/facebeauty/libts_face_beautify_hal.so
LOCAL_SRC_FILES_64 :=  arithmetic/facebeauty/libts_face_beautify_hal_64.so
else
LOCAL_SRC_FILES := arithmetic/facebeauty/libts_face_beautify_hal.so
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_MODULE_PATH := $(TARGET_OUT)/lib
endif
include $(BUILD_PREBUILT)
endif

ifeq ($(strip $(TARGET_BOARD_CAMERA_HDR_CAPTURE)),true)
		include $(CLEAR_VARS)
		LOCAL_MODULE := libsprd_easy_hdr
		LOCAL_MODULE_CLASS := SHARED_LIBRARIES
		LOCAL_MODULE_TAGS := optional
		LOCAL_MODULE_SUFFIX := .so
	ifeq ($(strip $(TARGET_ARCH)),arm64)
		LOCAL_MULTILIB := both
		LOCAL_MODULE_STEM_32 := libsprd_easy_hdr.so
		LOCAL_MODULE_STEM_64 := libsprd_easy_hdr.so
		LOCAL_SRC_FILES_32 :=  arithmetic/libsprd_easy_hdr.so
		LOCAL_SRC_FILES_64 :=  arithmetic/lib64/libsprd_easy_hdr.so
	else
		LOCAL_SRC_FILES := arithmetic/libsprd_easy_hdr.so
		LOCAL_MODULE_CLASS := SHARED_LIBRARIES
		LOCAL_MODULE_PATH := $(TARGET_OUT)/lib
	endif #end SPRDLIB
		include $(BUILD_PREBUILT)
endif

ifeq ($(strip $(TARGET_BOARD_CAMERA_FACE_DETECT)),true)
	ifeq ($(strip $(TARGET_BOARD_CAMERA_FD_LIB)),omron)
	ifeq ($(strip $(TARGET_ARCH)),arm64)
		include $(CLEAR_VARS)
		LOCAL_MODULE := libeUdnDt
		LOCAL_MODULE_CLASS := STATIC_LIBRARIES
		LOCAL_MULTILIB := both
		LOCAL_MODULE_STEM_32 := libeUdnDt.a
		LOCAL_MODULE_STEM_64 := libeUdnDt.a
		LOCAL_SRC_FILES_32 := arithmetic/omron/lib32/libeUdnDt.a
		LOCAL_SRC_FILES_64 := arithmetic/omron/lib64/libeUdnDt.a
		LOCAL_MODULE_TAGS := optional
		include $(BUILD_PREBUILT)

		include $(CLEAR_VARS)
		LOCAL_MODULE := libeUdnCo
		LOCAL_MODULE_CLASS := STATIC_LIBRARIES
		LOCAL_MULTILIB := both
		LOCAL_MODULE_STEM_32 := libeUdnCo.a
		LOCAL_MODULE_STEM_64 := libeUdnCo.a
		LOCAL_SRC_FILES_32 := arithmetic/omron/lib32/libeUdnCo.a
		LOCAL_SRC_FILES_64 := arithmetic/omron/lib64/libeUdnCo.a
		LOCAL_MODULE_TAGS := optional
		include $(BUILD_PREBUILT)
	else
		include $(CLEAR_VARS)
		LOCAL_MODULE := libeUdnDt
		LOCAL_SRC_FILES := arithmetic/omron/lib32/libeUdnDt.a
		LOCAL_MODULE_TAGS := optional
		LOCAL_MODULE_SUFFIX := .a
		LOCAL_MODULE_CLASS := STATIC_LIBRARIES
		include $(PREBUILT_STATIC_LIBRARY)

		include $(CLEAR_VARS)
		LOCAL_MODULE := libeUdnCo
		LOCAL_SRC_FILES := arithmetic/omron/lib32/libeUdnCo.a
		LOCAL_MODULE_TAGS := optional
		LOCAL_MODULE_SUFFIX := .a
		LOCAL_MODULE_CLASS := STATIC_LIBRARIES
		include $(PREBUILT_STATIC_LIBRARY)

		include $(CLEAR_VARS)
		LOCAL_MODULE := libeUdnDt
		LOCAL_SRC_FILES := arithmetic/omron/lib32/libeUdnDt.a
		LOCAL_MODULE_TAGS := optional
		LOCAL_MODULE_SUFFIX := .a
		LOCAL_MODULE_CLASS := STATIC_LIBRARIES
		include $(BUILD_PREBUILT)

		include $(CLEAR_VARS)
		LOCAL_MODULE := libeUdnCo
		LOCAL_SRC_FILES := arithmetic/omron/lib32/libeUdnCo.a
		LOCAL_MODULE_TAGS := optional
		LOCAL_MODULE_SUFFIX := .a
		LOCAL_MODULE_CLASS := STATIC_LIBRARIES
		include $(BUILD_PREBUILT)
	endif # end TARGET_ARCH
	endif # fd_lib
endif

######################################################
