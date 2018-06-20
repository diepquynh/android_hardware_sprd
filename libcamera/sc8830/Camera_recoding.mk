LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := libcamsensors
LOCAL_SRC_FILES := sensor/libcamsensors.so
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_SUFFIX := .so
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_MODULE_PATH := $(TARGET_OUT_VENDOR_SHARED_LIBRARIES)
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)

sc8830like:=0

ifeq ($(strip $(TARGET_BOARD_PLATFORM)),sc8830)
sc8830like=1
endif

ifeq ($(strip $(TARGET_BOARD_PLATFORM)),scx15)
sc8830like=1
endif

ifeq ($(strip $(sc8830like)),1)
LOCAL_C_INCLUDES := \
	$(LOCAL_PATH)/vsp/sc8830/inc	\
	$(LOCAL_PATH)/vsp/sc8830/src \
	$(LOCAL_PATH)/jpeg/jpeg_fw_8830/inc \
	$(LOCAL_PATH)/jpeg/jpeg_fw_8830/src \
	$(LOCAL_PATH)/common/inc \
	$(LOCAL_PATH)/hal1.0/inc \
	$(LOCAL_PATH)/auto_test/inc \
	$(LOCAL_PATH)/oem/inc \
	$(LOCAL_PATH)/oem/isp_calibration/inc \
	$(LOCAL_PATH)/isp1.0/inc \
	$(LOCAL_PATH)/mtrace \
	external/skia/include/images \
	external/skia/include/core\
	external/jhead \
	external/sqlite/dist \
	system/media/camera/include \
	$(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/source/include/video \
	$(LOCAL_PATH)/../../gralloc/sc8830

LOCAL_SRC_FILES:= \
	oem/src/SprdOEMCamera.c \
	hal1.0/src/SprdCameraParameters.cpp \
	auto_test/src/SprdCameraHardware_autest_Interface.cpp \
	oem/src/cmr_common.c \
	oem/src/cmr_oem.c \
	oem/src/cmr_setting.c \
	oem/src/cmr_mem.c \
	common/src/cmr_msg.c \
	oem/src/cmr_scale.c \
	oem/src/cmr_rotate.c \
	oem/src/cmr_v4l2.c \
	oem/src/jpeg_codec.c \
	oem/src/cmr_exif.c \
	oem/src/sensor_cfg.c \
	oem/src/cmr_preview.c \
	oem/src/cmr_snapshot.c \
	oem/src/cmr_sensor.c \
	oem/src/cmr_ipm.c \
	oem/src/cmr_hdr.c \
	oem/src/cmr_fd.c \
	oem/src/cmr_uvdenoise.c \
	oem/src/cmr_focus.c \
	oem/src/sensor_drv_u.c \
	sensor/sensor_autotest_ccir_yuv.c \
	sensor/sensor_pattern.c \
	sensor/sensor_s5k4ecgx_mipi.c \
	sensor/sensor_s5k4ecgx.c \
	vsp/sc8830/src/jpg_drv_sc8830.c \
	jpeg/jpeg_fw_8830/src/jpegcodec_bufmgr.c \
	jpeg/jpeg_fw_8830/src/jpegcodec_global.c \
	jpeg/jpeg_fw_8830/src/jpegcodec_table.c \
	jpeg/jpeg_fw_8830/src/jpegenc_bitstream.c \
	jpeg/jpeg_fw_8830/src/jpegenc_frame.c \
	jpeg/jpeg_fw_8830/src/jpegenc_header.c \
	jpeg/jpeg_fw_8830/src/jpegenc_init.c \
	jpeg/jpeg_fw_8830/src/jpegenc_interface.c \
	jpeg/jpeg_fw_8830/src/jpegenc_malloc.c \
	jpeg/jpeg_fw_8830/src/jpegenc_api.c \
	jpeg/jpeg_fw_8830/src/jpegdec_bitstream.c \
	jpeg/jpeg_fw_8830/src/jpegdec_frame.c \
	jpeg/jpeg_fw_8830/src/jpegdec_init.c \
	jpeg/jpeg_fw_8830/src/jpegdec_interface.c \
	jpeg/jpeg_fw_8830/src/jpegdec_malloc.c \
	jpeg/jpeg_fw_8830/src/jpegdec_dequant.c	\
	jpeg/jpeg_fw_8830/src/jpegdec_out.c \
	jpeg/jpeg_fw_8830/src/jpegdec_parse.c \
	jpeg/jpeg_fw_8830/src/jpegdec_pvld.c \
	jpeg/jpeg_fw_8830/src/jpegdec_vld.c \
	jpeg/jpeg_fw_8830/src/jpegdec_api.c  \
	jpeg/jpeg_fw_8830/src/exif_writer.c  \
	jpeg/jpeg_fw_8830/src/jpeg_stream.c \
	mtrace/mtrace.c \
	oem/isp_calibration/src/isp_calibration.c \
	oem/isp_calibration/src/isp_cali_interface.c
LOCAL_SRC_FILES+= \
	hal1.0/src/SprdCameraHardwareInterface.cpp
include $(LOCAL_PATH)/isp1.0/isp1_0.mk
endif

LOCAL_MODULE_RELATIVE_PATH := hw
LOCAL_CFLAGS := -fno-strict-aliasing -D_VSP_ -DJPEG_ENC -D_VSP_LINUX_ -DCHIP_ENDIAN_LITTLE -DCONFIG_CAMERA_2M -DANDROID_4100

ifeq ($(strip $(TARGET_BOARD_PLATFORM)),scx15)
LOCAL_CFLAGS += -DCONFIG_CAMERA_SMALL_PREVSIZE
endif

ifeq ($(strip $(TARGET_BOARD_CAMERA_FLASH_CTRL)),true)
LOCAL_CFLAGS += -DCONFIG_CAMERA_FLASH_CTRL
endif

ifeq ($(strip $(CAMERA_SUPPORT_SIZE)),13M)
LOCAL_CFLAGS += -DCONFIG_CAMERA_SUPPORT_13M
endif

ifeq ($(strip $(CAMERA_SUPPORT_SIZE)),8M)
LOCAL_CFLAGS += -DCONFIG_CAMERA_SUPPORT_8M
endif

ifeq ($(strip $(CAMERA_SUPPORT_SIZE)),5M)
LOCAL_CFLAGS += -DCONFIG_CAMERA_SUPPORT_5M
endif

ifeq ($(strip $(CAMERA_SUPPORT_SIZE)),3M)
LOCAL_CFLAGS += -DCONFIG_CAMERA_SUPPORT_3M
endif

ifeq ($(strip $(CAMERA_SUPPORT_SIZE)),2M)
LOCAL_CFLAGS += -DCONFIG_CAMERA_SUPPORT_2M
endif

ifeq ($(strip $(FRONT_CAMERA_SUPPORT_SIZE)),3M)
LOCAL_CFLAGS += -DCONFIG_FRONT_CAMERA_SUPPORT_3M
endif

ifeq ($(strip $(TARGET_BOARD_NO_FRONT_SENSOR)),true)
LOCAL_CFLAGS += -DCONFIG_DCAM_SENSOR_NO_FRONT_SUPPORT
endif

ifeq ($(strip $(TARGET_BOARD_PLATFORM)),sc8830)
LOCAL_CFLAGS += -DCONFIG_CAMERA_ISP
endif

LOCAL_CFLAGS += -DCONFIG_CAMERA_PREVIEW_YV12

ifeq ($(strip $(TARGET_BOARD_CAMERA_CAPTURE_MODE)),true)
LOCAL_CFLAGS += -DCONFIG_CAMERA_ZSL_CAPTURE
endif

ifeq ($(strip $(TARGET_BOARD_CAMERA_ANDROID_ZSL_MODE)),true)
LOCAL_CFLAGS += -DCONFIG_CAMERA_ANDROID_ZSL_CAPTURE
endif

ifeq ($(strip $(TARGET_BOARD_CAMERA_CAF)),true)
LOCAL_CFLAGS += -DCONFIG_CAMERA_CAF
endif

ifeq ($(strip $(TARGET_BOARD_CAMERA_ROTATION_CAPTURE)),true)
LOCAL_CFLAGS += -DCONFIG_CAMERA_ROTATION_CAPTURE
endif

ifeq ($(strip $(TARGET_BOARD_FRONT_CAMERA_ROTATION)),true)
LOCAL_CFLAGS += -DCONFIG_FRONT_CAMERA_ROTATION
endif

ifeq ($(strip $(TARGET_BOARD_BACK_CAMERA_ROTATION)),true)
LOCAL_CFLAGS += -DCONFIG_BACK_CAMERA_ROTATION
endif

ifeq ($(strip $(TARGET_BOARD_CAMERA_ROTATION)),true)
LOCAL_CFLAGS += -DCONFIG_CAMERA_ROTATION
endif

ifeq ($(strip $(TARGET_BOARD_CAMERA_ANTI_SHAKE)),true)
LOCAL_CFLAGS += -DCONFIG_CAMERA_ANTI_SHAKE
endif

ifeq ($(strip $(TARGET_BOARD_CAMERA_DMA_COPY)),true)
LOCAL_CFLAGS += -DCONFIG_CAMERA_DMA_COPY
endif

ifeq ($(strip $(TARGET_BOARD_BACK_CAMERA_INTERFACE)),mipi)
LOCAL_CFLAGS += -DCONFIG_BACK_CAMERA_MIPI
endif

ifeq ($(strip $(TARGET_BOARD_BACK_CAMERA_INTERFACE)),ccir)
LOCAL_CFLAGS += -DCONFIG_BACK_CAMERA_CCIR
endif

ifeq ($(strip $(TARGET_BOARD_FRONT_CAMERA_INTERFACE)),mipi)
LOCAL_CFLAGS += -DCONFIG_FRONT_CAMERA_MIPI
endif

ifeq ($(strip $(TARGET_BOARD_FRONT_CAMERA_INTERFACE)),ccir)
LOCAL_CFLAGS += -DCONFIG_FRONT_CAMERA_CCIR
endif

ifeq ($(strip $(TARGET_BOARD_CAMERA_SUPPORT_720P)),true)
LOCAL_CFLAGS += -DCONFIG_CAMERA_SUPPORT_720P
endif

ifeq ($(strip $(TARGET_BOARD_CAMERA_SUPPORT_CIF)),true)
LOCAL_CFLAGS += -DCONFIG_CAMERA_SUPPORT_CIF
endif

ifeq ($(strip $(CAMERA_DISP_ION)),true)
LOCAL_CFLAGS += -DUSE_ION_MEM
endif

ifeq ($(strip $(CAMERA_SENSOR_OUTPUT_ONLY)),true)
LOCAL_CFLAGS += -DCONFIG_SENSOR_OUTPUT_ONLY
endif

ifeq ($(strip $(TARGET_BOARD_CAMERA_FACE_DETECT)),true)
LOCAL_CFLAGS += -DCONFIG_CAMERA_FACE_DETECT
endif

ifeq ($(strip $(TARGET_BOARD_CAMERA_NO_FLASH_DEV)),true)
LOCAL_CFLAGS += -DCONFIG_CAMERA_FLASH_NOT_SUPPORT
endif

ifeq ($(strip $(TARGET_BOARD_CAMERA_NO_AUTOFOCUS_DEV)),true)
LOCAL_CFLAGS += -DCONFIG_CAMERA_AUTOFOCUS_NOT_SUPPORT
endif

ifeq ($(strip $(TARGET_BOARD_CAMERA_NO_720P_PREVIEW)),true)
LOCAL_CFLAGS += -DCONFIG_CAMERA_NO_720P_PREVIEW
endif

ifeq ($(strip $(TARGET_BOARD_CAMERA_ADAPTER_IMAGE)),180)
LOCAL_CFLAGS += -DCONFIG_CAMERA_IMAGE_180
endif

ifeq ($(strip $(TARGET_BOARD_CAMERA_PRE_ALLOC_CAPTURE_MEM)),true)
LOCAL_CFLAGS += -DCONFIG_PRE_ALLOC_CAPTURE_MEM
endif

ifeq ($(strip $(TARGET_BOARD_FRONT_CAMERA_MIPI)),phya)
LOCAL_CFLAGS += -DCONFIG_FRONT_CAMERA_MIPI_PHYA
endif

ifeq ($(strip $(TARGET_BOARD_FRONT_CAMERA_MIPI)),phyb)
LOCAL_CFLAGS += -DCONFIG_FRONT_CAMERA_MIPI_PHYB
endif

ifeq ($(strip $(TARGET_BOARD_FRONT_CAMERA_MIPI)),phyab)
LOCAL_CFLAGS += -DCONFIG_FRONT_CAMERA_MIPI_PHYAB
endif

ifeq ($(strip $(TARGET_BOARD_BACK_CAMERA_MIPI)),phya)
LOCAL_CFLAGS += -DCONFIG_BACK_CAMERA_MIPI_PHYA
endif

ifeq ($(strip $(TARGET_BOARD_BACK_CAMERA_MIPI)),phyb)
LOCAL_CFLAGS += -DCONFIG_BACK_CAMERA_MIPI_PHYB
endif

ifeq ($(strip $(TARGET_BOARD_BACK_CAMERA_MIPI)),phyab)
LOCAL_CFLAGS += -DCONFIG_BACK_CAMERA_MIPI_PHYAB
endif

ifeq ($(strip $(TARGET_BOARD_FRONT_CAMERA_CCIR_PCLK)),source0)
LOCAL_CFLAGS += -DCONFIG_FRONT_CAMERA_CCIR_PCLK_SOURCE0
endif

ifeq ($(strip $(TARGET_BOARD_FRONT_CAMERA_CCIR_PCLK)),source1)
LOCAL_CFLAGS += -DCONFIG_FRONT_CAMERA_CCIR_PCLK_SOURCE1
endif

ifeq ($(strip $(TARGET_BOARD_BACK_CAMERA_CCIR_PCLK)),source0)
LOCAL_CFLAGS += -DCONFIG_BACK_CAMERA_CCIR_PCLK_SOURCE0
endif

ifeq ($(strip $(TARGET_BOARD_BACK_CAMERA_CCIR_PCLK)),source1)
LOCAL_CFLAGS += -DCONFIG_BACK_CAMERA_CCIR_PCLK_SOURCE1
endif

ifeq ($(strip $(TARGET_BOARD_CAMERA_CAPTURE_DENOISE)),true)
LOCAL_CFLAGS += -DCONFIG_CAPTURE_DENOISE
endif

ifeq ($(strip $(TARGET_BOARD_CAMERA_NO_EXPOSURE_METERING)),true)
LOCAL_CFLAGS += -DCONFIG_EXPOSURE_METERING_NOT_SUPPORT
endif

ifeq ($(strip $(TARGET_BOARD_BACK_CAMERA_ISO_NOT_SUPPORT)),true)
LOCAL_CFLAGS += -DCONFIG_BACK_CAMERA_ISO_NOT_SUPPORT
endif

ifeq ($(strip $(TARGET_BOARD_FRONT_CAMERA_ISO_NOT_SUPPORT)),true)
LOCAL_CFLAGS += -DCONFIG_FRONT_CAMERA_ISO_NOT_SUPPORT
endif

ifeq ($(strip $(TARGET_BOARD_LOW_CAPTURE_MEM)),true)
LOCAL_CFLAGS += -DCONFIG_LOW_CAPTURE_MEM
endif

ifeq ($(strip $(TARGET_BOARD_CAMERA_HAL_VERSION)),HAL1.0)
LOCAL_MODULE := camera.$(TARGET_BOARD_PLATFORM)
else
LOCAL_MODULE := camera1.$(TARGET_BOARD_PLATFORM)
endif
LOCAL_MODULE_TAGS := optional

ifeq ($(strip $(sc8830like)),1)
LOCAL_SHARED_LIBRARIES := libandroidfw libexif libutils libbinder libcamera_client libskia libcutils libhardware libawb libae libaf liblsc libuvdenoise libmorpho_easy_hdr libcamera_metadata libmemoryheapion libui libgui libcamsensors

ifeq ($(strip $(TARGET_BOARD_CAMERA_FACE_DETECT)),true)
LOCAL_SHARED_LIBRARIES += libface_finder
endif
endif


include $(BUILD_SHARED_LIBRARY)

ifeq ($(strip $(sc8830like)),1)

include $(CLEAR_VARS)
LOCAL_MODULE := libawb
LOCAL_SRC_FILES := oem/isp1.0/libawb.so
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_SUFFIX := .so
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_MODULE_PATH := $(TARGET_OUT_VENDOR_SHARED_LIBRARIES)
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libae
LOCAL_SRC_FILES := oem/isp1.0/libae.so
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_SUFFIX := .so
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_MODULE_PATH := $(TARGET_OUT_VENDOR_SHARED_LIBRARIES)
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libaf
LOCAL_SRC_FILES := oem/isp1.0/libaf.so
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_SUFFIX := .so
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_MODULE_PATH := $(TARGET_OUT_VENDOR_SHARED_LIBRARIES)
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := liblsc
LOCAL_SRC_FILES := oem/isp1.0/liblsc.so
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_SUFFIX := .so
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_MODULE_PATH := $(TARGET_OUT_VENDOR_SHARED_LIBRARIES)
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libuvdenoise
LOCAL_SRC_FILES := oem/isp/libuvdenoise.so
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_SUFFIX := .so
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_MODULE_PATH := $(TARGET_OUT_VENDOR_SHARED_LIBRARIES)
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libmorpho_easy_hdr
LOCAL_SRC_FILES := arithmetic/sc8830/libmorpho_easy_hdr.so
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_SUFFIX := .so
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_MODULE_PATH := $(TARGET_OUT_VENDOR_SHARED_LIBRARIES)
include $(BUILD_PREBUILT)

ifeq ($(strip $(TARGET_BOARD_CAMERA_FACE_DETECT)),true)
include $(CLEAR_VARS)
LOCAL_MODULE := libface_finder
LOCAL_SRC_FILES := arithmetic/sc8830/libface_finder.so
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_SUFFIX := .so
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_MODULE_PATH := $(TARGET_OUT_VENDOR_SHARED_LIBRARIES)
include $(BUILD_PREBUILT)
endif
endif
