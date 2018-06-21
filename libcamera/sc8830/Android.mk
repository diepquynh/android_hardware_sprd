LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

TARGET_BOARD_CAMERA_READOTP_METHOD ?= 0

ISP_HW_VER := 2.0

#ifeq ($(strip $(TARGET_BOARD_CAMERA_ISP_SOFTWARE_VERSION)),2 3)
#ISP_HW_VER := 2.0
#else
#ISP_HW_VER := 1.0
#endif

ISP_LIB_PATH := libs/isp$(ISP_HW_VER)

LOCAL_C_INCLUDES := \
	external/skia/include/images \
	external/skia/include/core \
	external/jhead \
	external/sqlite/dist \
	system/media/camera/include \
	$(TOP)/frameworks/native/include/media/openmax \
	$(LOCAL_PATH)/include \
	$(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/source/include/video \
	$(LOCAL_PATH)/../../libmemoryheapion \
	$(LOCAL_PATH)/arithmetic/omron/inc \
	$(LOCAL_PATH)/arithmetic/inc \
	$(LOCAL_PATH)/arithmetic/facebeauty/inc \
	$(LOCAL_PATH)/common/inc \
	$(LOCAL_PATH)/jpeg/inc \
	$(LOCAL_PATH)/tool/mtrace \
	$(LOCAL_PATH)/vsp/inc \
	$(LOCAL_PATH)/../../gralloc/sc8830

LOCAL_SRC_FILES := \
	common/src/cmr_msg.c \
	jpeg/src/jpegdec_parse.c \
	jpeg/src/jpegdec_api.c \
	jpeg/src/jpegdec_init.c \
	jpeg/src/jpegenc_bitstream.c \
	jpeg/src/jpegenc_header.c \
	jpeg/src/jpegdec_dequant.c \
	jpeg/src/jpegdec_out.c \
	jpeg/src/jpegcodec_bufmgr.c \
	jpeg/src/jpegenc_malloc.c \
	jpeg/src/jpegdec_vld.c \
	jpeg/src/jpegcodec_table.c \
	jpeg/src/jpegdec_bitstream.c \
	jpeg/src/jpegenc_api.c \
	jpeg/src/jpegdec_malloc.c \
	jpeg/src/jpegdec_pvld.c \
	jpeg/src/jpegdec_frame.c \
	jpeg/src/jpegcodec_global.c \
	jpeg/src/jpegenc_init.c \
	jpeg/src/jpegenc_interface.c \
	jpeg/src/jpeg_stream.c \
	jpeg/src/jpegenc_frame.c \
	jpeg/src/exif_writer.c \
	jpeg/src/jpegdec_interface.c \
	vsp/src/jpg_drv_sc8830.c \
	tool/auto_test/src/SprdCameraHardware_autest_Interface.cpp \
	tool/mtrace/mtrace.c

LOCAL_MODULE_RELATIVE_PATH := hw

LOCAL_SHARED_LIBRARIES := \
	libutils \
	libmemoryheapion \
	libcamera_client \
	libcutils \
	libhardware \
	libcamera_metadata \
	libdl \
	libui \
	libbinder

ifeq ($(strip $(ISP_HW_VER)),1.0)
LOCAL_C_INCLUDES += \
	$(LOCAL_PATH)/isp1.0/inc \
	$(LOCAL_PATH)/isp1.0/uv_denoise/inc \
	$(LOCAL_PATH)/oem/inc \
	$(LOCAL_PATH)/oem/isp_calibration/inc \
	$(LOCAL_PATH)/oem/inc/ydenoise_paten

LOCAL_SRC_FILES += \
	isp1.0/isp_app.c \
	isp1.0/isp_app_msg.c \
	isp1.0/isp_ctrl.c \
	isp1.0/isp_drv.c \
	isp1.0/isp_msg.c \
	isp1.0/isp_otp.c \
	isp1.0/isp_param_file_update.c \
	isp1.0/isp_param_size.c \
	isp1.0/isp_param_tune_com.c \
	isp1.0/isp_param_tune_v0000.c \
	isp1.0/isp_param_tune_v0001.c \
	isp1.0/isp_smart.c \
	isp1.0/isp_smart_fitting.c \
	isp1.0/isp_smart_light.c \
	isp1.0/isp_stub_msg.c \
	isp1.0/isp_stub_proc.c \
	isp1.0/isp_video.c \
	oem/src/jpeg_codec.c \
	oem/src/cmr_exif.c \
	oem/src/cmr_snr_uvdenoise.c \
	oem/src/cmr_ydenoise.c \
	oem/src/cmr_ipm.c \
	oem/src/cmr_copy.c \
	oem/src/cmr_hdr.c \
	oem/src/cmr_fd_omron.c \
	oem/src/cmr_rotate.c \
	oem/src/cmr_focus.c \
	oem/src/cmr_sensor.c \
	oem/src/cmr_oem.c \
	oem/src/cmr_grab.c \
	oem/src/sensor_drv_u.c \
	oem/src/cmr_common.c \
	oem/src/SprdOEMCamera.c \
	oem/src/cmr_mem.c \
	oem/src/cmr_uvdenoise.c \
	oem/src/sensor_cfg.c \
	oem/src/cmr_preview.c \
	oem/src/cmr_setting.c \
	oem/src/cmr_scale.c \
	oem/src/cmr_snapshot.c \
	oem/isp_calibration/src/port_isp2.0.c \
	oem/isp_calibration/src/isp_cali_interface.c \
	oem/isp_calibration/src/port_isp1.0.c \
	oem/isp_calibration/src/isp_calibration.c \
	oem/srcdec_parse.c \
	oem/srcdec_api.c \
	oem/srcdec_init.c \
	oem/srcenc_bitstream.c \
	oem/srcenc_header.c \
	oem/srcdec_dequant.c \
	oem/srcdec_out.c \
	oem/srccodec_bufmgr.c \
	oem/srcenc_malloc.c \
	oem/srcdec_vld.c \
	oem/srccodec_table.c \
	oem/srcdec_bitstream.c \
	oem/srcenc_api.c \
	oem/srcdec_malloc.c \
	oem/srcdec_pvld.c \
	oem/srcdec_frame.c \
	oem/srccodec_global.c \
	oem/srcenc_init.c \
	oem/srcenc_interface.c \
	oem/src_stream.c \
	oem/srcenc_frame.c \
	oem/src/exif_writer.c \
	oem/srcdec_interface.c

LOCAL_SHARED_LIBRARIES += \
	libae \
	libawb \
	libaf \
	liblsc \
	libev

endif # ISP_HW_VER 1.0



ifeq ($(strip $(ISP_HW_VER)),2.0)

LOCAL_C_INCLUDES += \
	$(LOCAL_PATH)/isp2.0/isp_app \
	$(LOCAL_PATH)/isp2.0/isp_tune \
	$(LOCAL_PATH)/isp2.0/calibration \
	$(LOCAL_PATH)/isp2.0/driver/inc \
	$(LOCAL_PATH)/isp2.0/param_manager \
	$(LOCAL_PATH)/isp2.0/ae/inc \
	$(LOCAL_PATH)/isp2.0/sft_ae/inc \
	$(LOCAL_PATH)/isp2.0/awb/inc \
	$(LOCAL_PATH)/isp2.0/awb/ \
	$(LOCAL_PATH)/isp2.0/af/inc \
	$(LOCAL_PATH)/isp2.0/lsc/inc \
	$(LOCAL_PATH)/isp2.0/lib_ctrl/ \
	$(LOCAL_PATH)/isp2.0/anti_flicker/inc \
	$(LOCAL_PATH)/isp2.0/smart \
	$(LOCAL_PATH)/isp2.0/utility \
	$(LOCAL_PATH)/isp2.0/calibration/inc \
	$(LOCAL_PATH)/isp2.0/sft_af/inc \
	$(LOCAL_PATH)/isp2.0/third_lib/alc_ip/inc \
	$(LOCAL_PATH)/isp2.0/third_lib/alc_ip/ais/inc \
	$(LOCAL_PATH)/isp2.0/third_lib/alc_awb \
	$(LOCAL_PATH)/isp2.0/third_lib/alc_awb/inc \
	$(LOCAL_PATH)/isp2.0/third_lib/alc_af/inc/ \
	$(LOCAL_PATH)/oem2v0/inc \
	$(LOCAL_PATH)/oem2v0/isp_calibration/inc \
	$(LOCAL_PATH)/oem2v0/inc/ydenoise_paten

LOCAL_SRC_FILES += \
	isp2.0/smart/isp_smart.c \
	isp2.0/smart/debug_file.c \
	isp2.0/smart/isp_smart_alg_v00.c \
	isp2.0/smart/smart_ctrl.c \
	isp2.0/isp_tune/isp_param_size.c \
	isp2.0/isp_tune/isp_video.c \
	isp2.0/isp_tune/isp_param_tune_com.c \
	isp2.0/isp_tune/isp_otp.c \
	isp2.0/isp_app/isp_app.c \
	isp2.0/awb/awb_dummy.c \
	isp2.0/awb/awb_sprd_ctrl.c \
	isp2.0/ae/ae_dummy.c \
	isp2.0/ae/ae_sprd_ctrl.c \
	isp2.0/calibration/isp_otp_calibration.c \
	isp2.0/calibration/backup \
	isp2.0/af/sprd_af_ctrl.c \
	isp2.0/af/af_ctrl.c \
	isp2.0/af/af_dummy.c \
	isp2.0/sft_ae/aem_binning.c \
	isp2.0/sft_af/sp_af_ctrl.c \
	isp2.0/driver/src/isp_u_pre_cdn.c \
	isp2.0/driver/src/isp_u_nlm.c \
	isp2.0/driver/src/isp_u_raw_awb.c \
	isp2.0/driver/src/isp_u_postcdn.c \
	isp2.0/driver/src/isp_u_dev.c \
	isp2.0/driver/src/isp_u_csa.c \
	isp2.0/driver/src/isp_u_gamma.c \
	isp2.0/driver/src/isp_u_ct.c \
	isp2.0/driver/src/isp_u_yiq.c \
	isp2.0/driver/src/isp_u_pstrz.c \
	isp2.0/driver/src/isp_u_capability.c \
	isp2.0/driver/src/isp_u_awb.c \
	isp2.0/driver/src/isp_u_binning4awb.c \
	isp2.0/driver/src/isp_u_feeder.c \
	isp2.0/driver/src/isp_u_hist.c \
	isp2.0/driver/src/isp_u_fetch.c \
	isp2.0/driver/src/isp_u_ydelay.c \
	isp2.0/driver/src/isp_u_comm_v1.c \
	isp2.0/driver/src/isp_u_interface.c \
	isp2.0/driver/src/isp_u_arbiter.c \
	isp2.0/driver/src/isp_u_emboss.c \
	isp2.0/driver/src/isp_u_2d_lsc.c \
	isp2.0/driver/src/isp_u_raw_afm.c \
	isp2.0/driver/src/isp_u_buf_queue.c \
	isp2.0/driver/src/isp_u_bpc.c \
	isp2.0/driver/src/isp_u_nawbm.c \
	isp2.0/driver/src/isp_u_raw_sizer.c \
	isp2.0/driver/src/isp_u_rgb_gain.c \
	isp2.0/driver/src/isp_u_contrast.c \
	isp2.0/driver/src/isp_u_edge.c \
	isp2.0/driver/src/isp_u_hdr.c \
	isp2.0/driver/src/isp_u_cmc.c \
	isp2.0/driver/src/isp_u_hue.c \
	isp2.0/driver/src/isp_u_cmc8.c \
	isp2.0/driver/src/isp_u_rgb_gain2.c \
	isp2.0/driver/src/isp_u_nbpc.c \
	isp2.0/driver/src/isp_u_prefilter.c \
	isp2.0/driver/src/isp_u_csc.c \
	isp2.0/driver/src/isp_u_store.c \
	isp2.0/driver/src/isp_u_yiq_afm.c \
	isp2.0/driver/src/isp_u_hist2.c \
	isp2.0/driver/src/isp_u_nlc.c \
	isp2.0/driver/src/isp_u_pwd.c \
	isp2.0/driver/src/isp_u_brightness.c \
	isp2.0/driver/src/isp_u_css.c \
	isp2.0/driver/src/isp_u_pre_wavelet.c \
	isp2.0/driver/src/isp_u_comm.c \
	isp2.0/driver/src/isp_u_afm.c \
	isp2.0/driver/src/isp_u_grgb.c \
	isp2.0/driver/src/isp_u_cdn.c \
	isp2.0/driver/src/isp_u_cce.c \
	isp2.0/driver/src/isp_u_cfa.c \
	isp2.0/driver/src/isp_u_bdn.c \
	isp2.0/driver/src/isp_u_1d_lsc.c \
	isp2.0/driver/src/isp_u_rgb2y.c \
	isp2.0/driver/src/isp_u_raw_aem.c \
	isp2.0/driver/src/isp_u_pre_cdn_rgb.c \
	isp2.0/driver/src/isp_u_yiq_aem.c \
	isp2.0/driver/src/isp_u_hsv.c \
	isp2.0/driver/src/isp_u_fcs.c \
	isp2.0/driver/src/isp_u_ygamma.c \
	isp2.0/driver/src/isp_u_uv_prefilter.c \
	isp2.0/driver/src/isp_u_pgg.c \
	isp2.0/driver/src/isp_u_yuv_nlm.c \
	isp2.0/driver/src/isp_u_iircnr.c \
	isp2.0/driver/src/isp_u_blc.c \
	isp2.0/driver/src/isp_u_anti_flicker.c \
	isp2.0/driver/src/isp_u_dispatch.c \
	isp2.0/driver/src/isp_u_glb_gain.c \
	isp2.0/driver/src/isp_u_aca.c \
	isp2.0/utility/isp_param_file_update.c \
	isp2.0/param_manager/isp_pm.c \
	isp2.0/param_manager/isp_com_alg.c \
	isp2.0/param_manager/isp_blocks_cfg.c \
	isp2.0/lib_ctrl/lib_ctrl.c \
	oem2v0/src/jpeg_codec.c \
	oem2v0/src/cmr_exif.c \
	oem2v0/src/cmr_ydenoise.c \
	oem2v0/src/cmr_ipm.c \
	oem2v0/src/cmr_copy.c \
	oem2v0/src/cmr_hdr.c \
	oem2v0/src/cmr_fd_omron.c \
	oem2v0/src/cmr_rotate.c \
	oem2v0/src/cmr_focus.c \
	oem2v0/src/cmr_sensor.c \
	oem2v0/src/cmr_oem.c \
	oem2v0/src/cmr_grab.c \
	oem2v0/src/sensor_drv_u.c \
	oem2v0/src/cmr_common.c \
	oem2v0/src/SprdOEMCamera.c \
	oem2v0/src/cmr_mem.c \
	oem2v0/src/cmr_uvdenoise.c \
	oem2v0/src/sensor_cfg.c \
	oem2v0/src/sensor_isp_param_awb_pac.c \
	oem2v0/src/cmr_preview.c \
	oem2v0/src/sensor_isp_param_merge.c \
	oem2v0/src/cmr_setting.c \
	oem2v0/src/cmr_scale.c \
	oem2v0/src/cmr_snapshot.c \
	oem2v0/isp_calibration/src/port_isp2.0.c \
	oem2v0/isp_calibration/src/isp_cali_interface.c \
	oem2v0/isp_calibration/src/port_isp1.0.c \
	oem2v0/isp_calibration/src/isp_calibration.c

ifeq ($(strip $(TARGET_BOARD_USE_THRID_LIB)),true)
	ifeq ($(strip $(TARGET_BOARD_USE_THIRD_AWB_LIB_A)),true)
		ifeq ($(strip $(TARGET_BOARD_USE_ALC_AE_AWB)),true)
			LOCAL_CFLAGS += \
				-DCONFIG_USE_ALC_AE \
				-DCONFIG_USE_ALC_AWB

			LOCAL_SHARED_LIBRARIES += \
				libAl_Ais \
				libAl_Ais_Sp

			LOCAL_SRC_FILES += \
				isp2.0/third_lib/alc_ip/ais/al_ais_ae_ctrl.c \
				isp2.0/third_lib/alc_ip/ais/al_ais_awb_ctrl.c \
				isp2.0/third_lib/alc_ip/ais/al_ais_command.c \
				isp2.0/third_lib/alc_ip/ais/sprd_lsc_util.c \
				isp2.0/third_lib/alc_ip/awb_lsc/awb_al_ctrl.c
		else
			LOCAL_CFLAGS += \
				-DCONFIG_USE_ALC_AWB

			LOCAL_SRC_FILES += \
				isp2.0/third_lib/alc_awb/awb_al_ctrl.c
		endif # TARGET_BOARD_USE_ALC_AE_AWB
	endif # TARGET_BOARD_USE_THIRD_AWB_LIB_A

	ifeq ($(strip $(TARGET_BOARD_USE_THIRD_AF_LIB_A)),true)
		LOCAL_SHARED_LIBRARIES += libaf_running

		LOCAL_SRC_FILES += \
			isp2.0/third_lib/alc_af/ALC_AF_Ctrl.c \
			isp2.0/third_lib/alc_af/ALC_AF_Interface.c
	endif # TARGET_BOARD_USE_THIRD_AF_LIB_A

endif # TARGET_BOARD_USE_THRID_LIB

LOCAL_SHARED_LIBRARIES += \
	libdeflicker \
	libspaf \
	libawb \
	liblsc \
	libcalibration \
	libae \
	libAF \
	libsft_af_ctrl \
	libaf_tune

endif # ISP_HW_VER 2.0



ifeq ($(strip $(TARGET_BOARD_CAMERA_HAL_VERSION)),1.0)
LOCAL_C_INCLUDES += \
	$(LOCAL_PATH)/hal1.0/inc \
	$(LOCAL_PATH)/hal1.0

LOCAL_SRC_FILES += \
	hal1.0/src/SprdCameraHardwareInterface.cpp \
	hal1.0/src/SprdCameraParameters.cpp

else
$(info because hal3 use hal1.0 head file and cpp !!!!!!!!!!!!)
$(info what the f* is it!!!!!!!!!!!!! We must change this code)
LOCAL_C_INCLUDES += \
	$(LOCAL_PATH)/hal1.0/inc

LOCAL_SRC_FILES += \
	hal1.0/src/SprdCameraParameters.cpp

endif # TARGET_BOARD_CAMERA_HAL_VERSION



ifneq ($(strip $(TARGET_BOARD_CAMERA_HAL_VERSION)),1.0)
LOCAL_C_INCLUDES += \
	$(LOCAL_PATH)/hal3/inc \
	$(LOCAL_PATH)/hal3

LOCAL_SRC_FILES += \
	hal3/SprdCamera3Channel.cpp \
	hal3/SprdCamera3Factory.cpp \
	hal3/SprdCamera3Flash.cpp \
	hal3/SprdCamera3Hal.cpp \
	hal3/SprdCamera3HWI.cpp \
	hal3/SprdCamera3Mem.cpp \
	hal3/SprdCamera3OEMIf.cpp \
	hal3/SprdCamera3Setting.cpp \
	hal3/SprdCamera3Stream.cpp \
	test/test.cpp

endif



include $(LOCAL_PATH)/sensor/Sprdroid.mk

ifeq ($(strip $(TARGET_BOARD_CAMERA_UV_DENOISE)),true)
LOCAL_SHARED_LIBRARIES += libuvdenoise
endif

ifeq ($(strip $(TARGET_BOARD_CAMERA_Y_DENOISE)),true)
LOCAL_SHARED_LIBRARIES += libynoise
endif

ifeq ($(strip $(TARGET_BOARD_CAMERA_SNR_UV_DENOISE)),true)
LOCAL_SHARED_LIBRARIES += libcnr
endif

ifeq ($(strip $(TARGET_BOARD_CAMERA_FACE_BEAUTY)),false)
else
LOCAL_SHARED_LIBRARIES += libts_face_beautify_hal
endif

ifeq ($(strip $(TARGET_BOARD_CAMERA_HDR_CAPTURE)),true)
    #default use sprd hdr
	LOCAL_CFLAGS += -DCONFIG_SPRD_HDR_LIB
	LOCAL_SHARED_LIBRARIES += libsprd_easy_hdr

endif

ifeq ($(strip $(TARGET_BOARD_CAMERA_FACE_DETECT)),true)
	ifeq ($(strip $(TARGET_BOARD_CAMERA_FD_LIB)),omron)
		LOCAL_STATIC_LIBRARIES +=libeUdnDt libeUdnCo
	endif
endif

LOCAL_CFLAGS += \
	-Wno-missing-braces \
	-fno-stack-protector \
	-fno-strict-aliasing \
	-Wno-unused-parameter \
	-DCHIP_ENDIAN_LITTLE \
	-DJPEG_ENC \
	-D_VSP_LINUX_ \
	-D_VSP_

# use media extension
ifeq ($(TARGET_USES_MEDIA_EXTENSIONS), true)
LOCAL_CFLAGS += -DUSE_MEDIA_EXTENSIONS
endif

ifeq ($(strip $(ISP_HW_VER)),2.0)
LOCAL_CFLAGS += \
	-DAE_WORK_MOD_V0 #AE_WORK_MOD_V0: Old ae algorithm + slow converge
	#-DAE_WORK_MOD_V1 #AE_WORK_MOD_V1: new ae algorithm + slow converge
	#-DAE_WORK_MOD_V2 #AE_WORK_MOD_V2: new ae algorithm + fast converge

endif

include $(LOCAL_PATH)/SprdCtrl.mk

LOCAL_MODULE := camera.$(TARGET_BOARD_PLATFORM)
LOCAL_MODULE_TAGS := optional

include $(BUILD_SHARED_LIBRARY)

include $(LOCAL_PATH)/SprdLib.mk