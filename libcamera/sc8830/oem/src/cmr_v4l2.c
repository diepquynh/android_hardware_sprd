/*
 * Copyright (C) 2012 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#define LOG_TAG "cmr_v4l2"

#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <linux/videodev2.h>
#include <pthread.h>
#include <unistd.h>
#include "cmr_v4l2.h"
#include <sprd_v4l2.h>


#define CMR_CHECK_FD \
		do { \
			if (-1 == p_v4l2->fd) { \
				CMR_LOGE("V4L2 device not opened"); \
				return -1; \
			} \
		} while(0)

#define CMR_V4L2_DEV_NAME      "/dev/video0"

#define CMR_CHECK_HANDLE \
		do { \
			if (!p_v4l2) { \
				CMR_LOGE("Invalid handle"); \
				return -1; \
			} \
		} while(0)

struct cmr_v4l2
{
	cmr_s32                 fd;
	cmr_evt_cb              v4l2_evt_cb;
	pthread_mutex_t         cb_mutex;
	pthread_mutex_t         status_mutex;
	pthread_mutex_t         path_mutex[CHN_MAX];
	cmr_u32                 is_on;
	pthread_t               thread_handle;
	cmr_u32                 chn_status[CHN_MAX];
	cmr_s32                 chn_frm_num[CHN_MAX];
	cmr_u32                 is_prev_trace;
	cmr_u32                 is_cap_trace;
	struct v4l2_init_param  init_param;
	v4l2_stream_on          stream_on_cb;
};

static cmr_int cmr_v4l2_create_thread(cmr_handle v4l2_handle);
static cmr_int cmr_v4l2_kill_thread(cmr_handle v4l2_handle);
static void* cmr_v4l2_thread_proc(void* data);
static cmr_u32 cmr_v4l2_get_4cc(cmr_u32 img_type);
static cmr_u32 cmr_v4l2_get_img_type(uint32_t fourcc);
static cmr_u32 cmr_v4l2_get_data_endian(struct img_data_end* in_endian, struct img_data_end* out_endian);

cmr_int cmr_v4l2_init(struct v4l2_init_param *init_param_ptr, cmr_handle *v4l2_handle)
{
	cmr_int                  ret = 0;
	cmr_u32                  channel_id;
	struct cmr_v4l2          *p_v4l2;

	p_v4l2 = (struct cmr_v4l2*)malloc(sizeof(struct cmr_v4l2));
	if (!p_v4l2) {
		CMR_LOGE("No mem");
		return -1;
	}
	*v4l2_handle = (cmr_handle)p_v4l2;
	p_v4l2->init_param = *init_param_ptr;

	CMR_LOGI("Start to open V4L2 device. 0x%x", (cmr_u32)*v4l2_handle);
	p_v4l2->fd = open(CMR_V4L2_DEV_NAME, O_RDWR, 0);
	if (-1 == p_v4l2->fd) {
		CMR_LOGE("Failed to open dcam device.errno : %d", errno);
		fprintf(stderr, "Cannot open '%s': %d, %s\n", CMR_V4L2_DEV_NAME, errno,  strerror(errno));
		exit(EXIT_FAILURE);
	} else {
		CMR_LOGI("OK to open device.");
	}

	ret = pthread_mutex_init(&p_v4l2->cb_mutex, NULL);
	if (ret) {
		CMR_LOGE("Failed to init mutex : %d", errno);
		exit(EXIT_FAILURE);
	}

	ret = pthread_mutex_init(&p_v4l2->status_mutex, NULL);
	if (ret) {
		CMR_LOGE("Failed to init status mutex : %d", errno);
		exit(EXIT_FAILURE);
	}

	for (channel_id = 0; channel_id < CHN_MAX; channel_id++) {
		ret = pthread_mutex_init(&p_v4l2->path_mutex[channel_id], NULL);
		if (ret) {
			CMR_LOGE("Failed to init path_mutex %d : %d", channel_id, errno);
			exit(EXIT_FAILURE);
		}
	}

	ret = cmr_v4l2_create_thread((cmr_handle)p_v4l2);
	p_v4l2->v4l2_evt_cb = NULL;
	p_v4l2->stream_on_cb = NULL;
	p_v4l2->is_prev_trace = 0;
	p_v4l2->is_cap_trace = 0;
	memset(p_v4l2->chn_status, 0, sizeof(p_v4l2->chn_status));
	return ret;
}

cmr_int cmr_v4l2_deinit(cmr_handle v4l2_handle)
{
	cmr_int                  ret = 0;
	cmr_u32                  channel_id = 0;
	struct cmr_v4l2          *p_v4l2;

	CMR_LOGI("Start to close V4L2 device. 0x%x", (cmr_u32)v4l2_handle);
	p_v4l2 = (struct cmr_v4l2*)v4l2_handle;

	CMR_CHECK_HANDLE;
	/* thread should be killed before fd deinited */
	ret = cmr_v4l2_kill_thread(v4l2_handle);
	if (ret) {
		CMR_LOGE("Failed to kill the thread. errno : %d", errno);
		exit(EXIT_FAILURE);
	}

	/* then close fd */
	if (-1 != p_v4l2->fd) {
		if (-1 == close(p_v4l2->fd)) {
			fprintf(stderr, "error %d, %s\n", errno, strerror(errno));
			exit(EXIT_FAILURE);
		}
		p_v4l2->fd = -1;
	}
	CMR_LOGI("thread kill done.");
	pthread_mutex_lock(&p_v4l2->cb_mutex);
	p_v4l2->v4l2_evt_cb = NULL;
	p_v4l2->is_prev_trace = 0;
	p_v4l2->is_cap_trace = 0;
	pthread_mutex_unlock(&p_v4l2->cb_mutex);
	pthread_mutex_destroy(&p_v4l2->cb_mutex);
	pthread_mutex_destroy(&p_v4l2->status_mutex);
	for (channel_id = 0; channel_id < CHN_MAX; channel_id++) {
		pthread_mutex_destroy(&p_v4l2->path_mutex[channel_id]);
	}
	free((void*)v4l2_handle);
	CMR_LOGI("close device.\n");
	return 0;
}

void cmr_v4l2_evt_reg(cmr_handle v4l2_handle, cmr_evt_cb  v4l2_event_cb)
{
	struct cmr_v4l2          *p_v4l2;

	p_v4l2 = (struct cmr_v4l2*)v4l2_handle;
	if (!p_v4l2)
		return;

	pthread_mutex_lock(&p_v4l2->cb_mutex);
	p_v4l2->v4l2_evt_cb = v4l2_event_cb;
	pthread_mutex_unlock(&p_v4l2->cb_mutex);
	return;
}

/*

For DV Timing parameter

timing_param[0]       img_fmt
timing_param[1]       img_ptn
timing_param[2]       res
timing_param[3]       deci_factor

if(CCIR)
timing_param[4]       ccir.v_sync_pol
timing_param[5]       ccir.h_sync_pol
timing_param[6]       ccir.pclk_pol
if(MIPI)
timing_param[4]       mipi.use_href
timing_param[5]       mipi.bits_per_pxl
timing_param[6]       mipi.is_loose
timing_param[7]       mipi.lane_num

timing_param[7]       width
timing_param[8]       height

timint_param[14]      IF status

*/
cmr_int cmr_v4l2_if_cfg(cmr_handle v4l2_handle, struct sensor_if *sn_if)
{
	cmr_int                  ret = 0;
	struct v4l2_control      ctrl;
	cmr_u32                  timing_param[V4L2_TIMING_LEN];
	struct cmr_v4l2          *p_v4l2;

	p_v4l2 = (struct cmr_v4l2*)v4l2_handle;
	CMR_CHECK_HANDLE;
	CMR_CHECK_FD;

	timing_param[V4L2_TIMING_LEN-1] = sn_if->if_type;
	timing_param[V4L2_TIMING_LEN-2] = IF_OPEN;
	timing_param[0] = sn_if->img_fmt;
	timing_param[1] = sn_if->img_ptn;
	timing_param[3] = sn_if->frm_deci;
	if (0 == sn_if->if_type) {
		/* CCIR interface */
		timing_param[4] = sn_if->if_spec.ccir.v_sync_pol;
		timing_param[5] = sn_if->if_spec.ccir.h_sync_pol;
		timing_param[6] = sn_if->if_spec.ccir.pclk_pol;
	} else {
		/* MIPI interface */
		timing_param[4] = sn_if->if_spec.mipi.use_href;
		timing_param[5] = sn_if->if_spec.mipi.bits_per_pxl;
		timing_param[6] = sn_if->if_spec.mipi.is_loose;
		timing_param[7] = sn_if->if_spec.mipi.lane_num;
		timing_param[8] = sn_if->if_spec.mipi.pclk;
	}

	ctrl.id = V4L2_CID_USER_CLASS;
	ctrl.value = (cmr_s32)&timing_param[0];

	ret = ioctl(p_v4l2->fd, VIDIOC_S_CTRL, &ctrl);

	CMR_LOGI("Set dv timing, ret %ld, if type %d, mode %d, deci %d, status %d",
		ret,
		timing_param[V4L2_TIMING_LEN-1],
		timing_param[0],
		timing_param[3],
		timing_param[V4L2_TIMING_LEN-2]);

	return ret;
}

cmr_int cmr_v4l2_if_decfg(cmr_handle v4l2_handle, struct sensor_if *sn_if)
{
	cmr_int                  ret = 0;
	struct v4l2_control      ctrl;
	cmr_u32                  timing_param[V4L2_TIMING_LEN];
	struct cmr_v4l2          *p_v4l2;

	p_v4l2 = (struct cmr_v4l2*)v4l2_handle;
	CMR_CHECK_HANDLE;
	CMR_CHECK_FD;

	timing_param[V4L2_TIMING_LEN-1] = sn_if->if_type;
	timing_param[V4L2_TIMING_LEN-2] = IF_CLOSE;

	ctrl.id = V4L2_CID_USER_CLASS;
	ctrl.value = (cmr_s32)&timing_param[0];

	ret = ioctl(p_v4l2->fd, VIDIOC_S_CTRL, &ctrl);

	CMR_LOGI("Set dv timing, ret %ld, if type %d, status %d.",
		ret,
		timing_param[V4L2_TIMING_LEN-1],
		timing_param[V4L2_TIMING_LEN-2]);

	return ret;
}

cmr_int cmr_v4l2_sn_cfg(cmr_handle v4l2_handle, struct sn_cfg *config)
{
	cmr_int                  ret = 0;
	struct v4l2_streamparm   stream_parm;
	struct cmr_v4l2          *p_v4l2;

	p_v4l2 = (struct cmr_v4l2*)v4l2_handle;
	CMR_CHECK_HANDLE;
	stream_parm.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	stream_parm.parm.capture.capability = CAPTURE_MODE;
	if (1 == config->frm_num) {
		stream_parm.parm.capture.capturemode = 0;/* means single-frame sample mode */
	} else {
		stream_parm.parm.capture.capturemode = 1;/* means multi-frame sample mode */
	}
	ret = ioctl(p_v4l2->fd, VIDIOC_S_PARM, &stream_parm);
	CMR_RTN_IF_ERR(ret);

	stream_parm.parm.capture.capability = CAPTURE_SENSOR_SIZE;
	stream_parm.parm.capture.reserved[2] = config->sn_size.width;
	stream_parm.parm.capture.reserved[3] = config->sn_size.height;
	ret = ioctl(p_v4l2->fd, VIDIOC_S_PARM, &stream_parm);

	CMR_LOGI("sn_trim x y w h %d, %d, %d, %d",
		config->sn_trim.start_x,
		config->sn_trim.start_y,
		config->sn_trim.width,
		config->sn_trim.height);

	stream_parm.parm.capture.capability = CAPTURE_SENSOR_TRIM;
	stream_parm.parm.capture.reserved[0] = config->sn_trim.start_x;
	stream_parm.parm.capture.reserved[1] = config->sn_trim.start_y;
	stream_parm.parm.capture.reserved[2] = config->sn_trim.width;
	stream_parm.parm.capture.reserved[3] = config->sn_trim.height;
	ret = ioctl(p_v4l2->fd, VIDIOC_S_PARM, &stream_parm);
exit:
	return ret;

}

static cmr_int cmr_v4l2_cap_cfg_common(cmr_handle v4l2_handle, struct cap_cfg *config, cmr_u32 channel_id, struct img_data_end *endian)
{
	cmr_int                  ret = 0;
	struct v4l2_crop         crop;
	struct v4l2_fmtdesc      fmtdesc;
	struct v4l2_format       format;
	cmr_u32                  found = 0;
	cmr_u32                  pxl_fmt;
	cmr_u32                  cfg_id = 0;
	enum v4l2_buf_type       buf_type;
	struct v4l2_streamparm   stream_parm;
	struct cmr_v4l2          *p_v4l2;
	struct img_data_end      data_endian;

	if (NULL == config)
		return -1;
	p_v4l2 = (struct cmr_v4l2*)v4l2_handle;

	CMR_CHECK_HANDLE;
	CMR_CHECK_FD;

	cfg_id = channel_id;
	stream_parm.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	stream_parm.parm.capture.capability = PATH_FRM_DECI;

	stream_parm.parm.capture.reserved[0] = cfg_id;
	stream_parm.parm.capture.reserved[1] = config->chn_deci_factor;
	ret = ioctl(p_v4l2->fd, VIDIOC_S_PARM, &stream_parm);
	CMR_LOGI("channel_id  %d, deci_factor %d, ret %ld \n", cfg_id, config->chn_deci_factor, ret);

	buf_type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	if (CHN_1 == cfg_id) {
		p_v4l2->chn_frm_num[1] = config->frm_num;
	} else  if (CHN_2 == cfg_id) {
		p_v4l2->chn_frm_num[2] = config->frm_num;
	} else {
		/* CHN_0 */
		p_v4l2->chn_frm_num[0] = config->frm_num;
	}
#if 0
	/* firstly, set the crop rectangle PATH1 module, this should be called before VIDIOC_TRY_FMT called */
	crop.c.left   = config->cfg.src_img_rect.start_x;
	crop.c.top    = config->cfg.src_img_rect.start_y;
	crop.c.width  = config->cfg.src_img_rect.width;
	crop.c.height = config->cfg.src_img_rect.height;
	crop.type     = buf_type;
	ret = ioctl(fd, VIDIOC_S_CROP, &crop);
	CMR_RTN_IF_ERR(ret);
#endif

	stream_parm.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	stream_parm.parm.capture.capability = CAPTURE_SET_CROP;
	stream_parm.parm.capture.extendedmode = cfg_id;
	stream_parm.parm.capture.reserved[0] = config->cfg.src_img_rect.start_x;
	stream_parm.parm.capture.reserved[1] = config->cfg.src_img_rect.start_y;
	stream_parm.parm.capture.reserved[2] = config->cfg.src_img_rect.width;
	stream_parm.parm.capture.reserved[3] = config->cfg.src_img_rect.height;
	ret = ioctl(p_v4l2->fd, VIDIOC_S_PARM, &stream_parm);
	CMR_RTN_IF_ERR(ret);

	/* secondly,  check whether the output format described by config->cfg[cfg_id] can be supported by the low layer */
	pxl_fmt = cmr_v4l2_get_4cc(config->cfg.dst_img_fmt);
	found = 0;
	fmtdesc.index = 0;
	fmtdesc.type  = buf_type;
	fmtdesc.flags = cfg_id;
	//pxl_fmt = cmr_v4l2_get_4cc(config->cfg.dst_img_fmt);
	while (0 == ioctl(p_v4l2->fd, VIDIOC_ENUM_FMT, &fmtdesc)) {
		if (fmtdesc.pixelformat == pxl_fmt) {
			CMR_LOGI("FourCC 0x%x is supported by the low layer", pxl_fmt);
			found = 1;
			break;
		}
		fmtdesc.index++;
	}

	if (found) {
		bzero(&format, sizeof(struct v4l2_format));
		format.type   = buf_type;
		format.fmt.pix.colorspace = cfg_id;
		format.fmt.pix.width = config->cfg.dst_img_size.width;
		format.fmt.pix.height = config->cfg.dst_img_size.height;
		format.fmt.pix.pixelformat = pxl_fmt; //fourecc
		format.fmt.pix.priv = config->cfg.need_isp;
		pthread_mutex_lock(&p_v4l2->path_mutex[cfg_id]);
		CMR_LOGI("VIDIOC_TRY_FMT type %d,fmt %d %d %d 0x%x %d", format.type, format.fmt.pix.colorspace, format.fmt.pix.width,
			format.fmt.pix.height,format.fmt.pix.pixelformat,format.fmt.pix.priv);
		ret = ioctl(p_v4l2->fd, VIDIOC_TRY_FMT, &format);
		CMR_LOGI("need binning, %d", format.fmt.pix.sizeimage);
		if (format.fmt.pix.sizeimage) {
			config->cfg.need_binning = 1;
		}
		if (0 == ret) {
			p_v4l2->chn_status[cfg_id] = CHN_BUSY;
		} else if (ret > 0) {
			CMR_LOGI("need restart");
			ret = CMR_V4L2_RET_RESTART;
		}
		if (NULL != endian){
			memcpy((void*)&data_endian,  (void*)&format.fmt.pix.field, sizeof(struct img_data_end));
			cmr_v4l2_get_data_endian(&data_endian, endian);
		}
		pthread_mutex_unlock(&p_v4l2->path_mutex[cfg_id]);
	} else {
		CMR_LOGI("fourcc not founded dst_img_fmt=0x%x \n", config->cfg.dst_img_fmt);
	}

exit:
	CMR_LOGI("ret %ld", ret);
	return ret;
}

cmr_int cmr_v4l2_cap_cfg(cmr_handle v4l2_handle, struct cap_cfg *config, cmr_u32 *channel_id, struct img_data_end *endian)
{
	cmr_int                  ret = 0;
	cmr_u32                  pxl_fmt;
	struct v4l2_streamparm   stream_parm;
	struct cmr_v4l2          *p_v4l2;

	if (NULL == config || NULL == channel_id)
		return -1;
	p_v4l2 = (struct cmr_v4l2*)v4l2_handle;

	CMR_CHECK_HANDLE;
	CMR_CHECK_FD;

	CMR_LOGI("frm_num %d dst width %d dst height %d.", config->frm_num, config->cfg.dst_img_size.width, config->cfg.dst_img_size.height);

	stream_parm.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	stream_parm.parm.capture.capability = CAPTURE_SET_OUTPUT_SIZE;
	stream_parm.parm.capture.reserved[0] = config->cfg.dst_img_size.width;
	stream_parm.parm.capture.reserved[1] = config->cfg.dst_img_size.height;
	pxl_fmt = cmr_v4l2_get_4cc(config->cfg.dst_img_fmt);
	stream_parm.parm.capture.reserved[2] = pxl_fmt;
	stream_parm.parm.capture.reserved[3] = config->cfg.need_isp_tool;
	ret = ioctl(p_v4l2->fd, VIDIOC_S_PARM, &stream_parm);
	CMR_RTN_IF_ERR(ret);

	stream_parm.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	ret = ioctl(p_v4l2->fd, VIDIOC_G_PARM, &stream_parm);
	CMR_RTN_IF_ERR(ret);

	*channel_id = stream_parm.parm.capture.extendedmode;
	ret = cmr_v4l2_cap_cfg_common(v4l2_handle, config, *channel_id, endian);

exit:
	CMR_LOGI("ret %ld", ret);
	return ret;
}

/*in cmr_v4l2_cap_cfg_lightly channel_id is in param*/
cmr_int cmr_v4l2_cap_cfg_lightly(cmr_handle v4l2_handle, struct cap_cfg *config, cmr_u32 channel_id)
{
	struct cmr_v4l2          *p_v4l2;

	if (NULL == config)
		return -1;
	p_v4l2 = (struct cmr_v4l2*)v4l2_handle;

	CMR_CHECK_HANDLE;
	CMR_CHECK_FD;

	CMR_LOGI("frm_num %d dst width %d dst height %d.", config->frm_num, config->cfg.dst_img_size.width, config->cfg.dst_img_size.height);

	return cmr_v4l2_cap_cfg_common(v4l2_handle, config, channel_id, NULL);
}

cmr_int cmr_v4l2_buff_cfg (cmr_handle v4l2_handle, struct buffer_cfg *buf_cfg)
{
	cmr_int                  ret = 0;
	cmr_u32                  i;
	struct v4l2_buffer       v4l2_buf;
	struct v4l2_streamparm   stream_parm;
	struct cmr_v4l2          *p_v4l2;

	if (NULL == buf_cfg || buf_cfg->count > CMR_BUF_MAX)
		return -1;
	p_v4l2 = (struct cmr_v4l2*)v4l2_handle;

	CMR_CHECK_HANDLE;
	CMR_CHECK_FD;

	CMR_LOGI("%d %d 0x%x ", buf_cfg->channel_id, buf_cfg->count, buf_cfg->base_id);

	v4l2_buf.type  = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	v4l2_buf.flags = buf_cfg->channel_id;

	/* firstly , set the base index for each channel */
	stream_parm.type                      = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	stream_parm.parm.capture.capability   = CAPTURE_FRM_ID_BASE;
	stream_parm.parm.capture.reserved[1]  = buf_cfg->base_id;
	stream_parm.parm.capture.extendedmode = buf_cfg->channel_id;
	ret = ioctl(p_v4l2->fd, VIDIOC_S_PARM, &stream_parm);
	CMR_RTN_IF_ERR(ret);

	/* secondly , set the frame address */
	for (i = 0; i < buf_cfg->count; i++) {
		v4l2_buf.m.userptr  = buf_cfg->addr[i].addr_y;
		v4l2_buf.length        = buf_cfg->addr[i].addr_u;
		v4l2_buf.reserved   = buf_cfg->addr[i].addr_v;
		v4l2_buf.field      = buf_cfg->index[i];
		CMR_LOGD("VIDIOC_QBUF: buf %d: Y=0x%lx, U=0x%lx, V=0x%lx \n",
			i, buf_cfg->addr[i].addr_y, buf_cfg->addr[i].addr_u, buf_cfg->addr[i].addr_v);
		ret = ioctl(p_v4l2->fd, VIDIOC_QBUF, &v4l2_buf);
		if (ret) {
			CMR_LOGE("Failed to QBuf, %ld", ret);
			break;
		}
	}

exit:
	return ret;

}

cmr_int cmr_v4l2_cap_start(cmr_handle v4l2_handle, cmr_u32 skip_num)
{
	cmr_int                  ret = 0;
	struct v4l2_streamparm   stream_parm;
	enum v4l2_buf_type       buf_type;
	struct cmr_v4l2          *p_v4l2;

	p_v4l2 = (struct cmr_v4l2*)v4l2_handle;
	CMR_CHECK_HANDLE;
	CMR_CHECK_FD;
	stream_parm.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	stream_parm.parm.capture.capability = CAPTURE_SKIP_NUM;
	stream_parm.parm.capture.reserved[0] = skip_num; /* just modify the skip number parameter */
	ret = ioctl(p_v4l2->fd, VIDIOC_S_PARM, &stream_parm);
	CMR_RTN_IF_ERR(ret);
	ret = ioctl(p_v4l2->fd, VIDIOC_STREAMON, &buf_type);
	if (0 == ret) {
		pthread_mutex_lock(&p_v4l2->status_mutex);
		p_v4l2->is_on = 1;
		pthread_mutex_unlock(&p_v4l2->status_mutex);
	}
	if (p_v4l2->stream_on_cb) {
		(*p_v4l2->stream_on_cb)(1, p_v4l2->init_param.oem_handle);
	}
exit:
	CMR_LOGI("ret = %ld.",ret);
	return ret;
}

cmr_int cmr_v4l2_cap_stop(cmr_handle v4l2_handle)
{
	cmr_int                  ret = 0;
	cmr_s32                  i;
	enum v4l2_buf_type       buf_type;
	struct cmr_v4l2          *p_v4l2;

	p_v4l2 = (struct cmr_v4l2*)v4l2_handle;
	CMR_CHECK_HANDLE;
	CMR_CHECK_FD;
	pthread_mutex_lock(&p_v4l2->status_mutex);
	p_v4l2->is_on = 0;
	pthread_mutex_unlock(&p_v4l2->status_mutex);

	for (i = 0; i < CHN_MAX; i++) {
		pthread_mutex_lock(&p_v4l2->path_mutex[i]);
	}
	ret = ioctl(p_v4l2->fd, VIDIOC_STREAMOFF, &buf_type);
	for (i = 0; i < CHN_MAX; i ++) {
		p_v4l2->chn_status[i] = CHN_IDLE;
		pthread_mutex_unlock(&p_v4l2->path_mutex[i]);
	}
	if (p_v4l2->stream_on_cb) {
		(*p_v4l2->stream_on_cb)(0, p_v4l2->init_param.oem_handle);
	}

exit:
	CMR_LOGI("ret = %ld.",ret);
	return ret;
}

cmr_int cmr_v4l2_set_trace_flag(cmr_handle v4l2_handle, cmr_u32 trace_owner, cmr_u32 val)
{
	struct cmr_v4l2          *p_v4l2;
	cmr_int                  ret = 0;

	p_v4l2 = (struct cmr_v4l2*)v4l2_handle;
	CMR_CHECK_HANDLE;
	CMR_CHECK_FD;

	if (PREV_TRACE == trace_owner) {
		p_v4l2->is_prev_trace = val;
	} else if (CAP_TRACE == trace_owner) {
		p_v4l2->is_cap_trace = val;
	} else {
		CMR_LOGE("unknown trace owner!");
	}
	return ret;
}

/*
parameters for v4l2_ext_controls
 ctrl_class,     should be reserved by V4L2, can be V4L2_CTRL_CLASS_USER or
                       V4L2_CTRL_CLASS_CAMERA, etc;
 count,          pause/resume control , 0 means pause, 1 means resume;
 error_idx,      channel id;
 reserved[0],    channel id;
 reserved[1],    deci_factor;
 struct v4l2_ext_control *controls;
*/
cmr_int cmr_v4l2_cap_resume(cmr_handle v4l2_handle, cmr_u32 channel_id, cmr_u32 skip_number, cmr_u32 deci_factor, cmr_s32 frm_num)
{
	cmr_int                  ret = 0;
	struct v4l2_streamparm   stream_parm;
	struct cmr_v4l2          *p_v4l2;

	p_v4l2 = (struct cmr_v4l2*)v4l2_handle;
	CMR_CHECK_HANDLE;
	CMR_CHECK_FD;
	CMR_LOGI("channel_id %d, frm_num %d, status %d", channel_id, frm_num, p_v4l2->chn_status[channel_id]);

	if (CHN_1 == channel_id) {
		p_v4l2->chn_frm_num[1] = frm_num;
	} else  if (CHN_2 == channel_id) {
		p_v4l2->chn_frm_num[2] = frm_num;
	} else {
		/* CHN_0 */
		p_v4l2->chn_frm_num[0] = frm_num;
	}

	stream_parm.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	stream_parm.parm.capture.capability = PATH_RESUME;

	stream_parm.parm.capture.reserved[0] = channel_id;
	pthread_mutex_lock(&p_v4l2->path_mutex[channel_id]);
	p_v4l2->chn_status[channel_id] = CHN_BUSY;
	ret = ioctl(p_v4l2->fd, VIDIOC_S_PARM, &stream_parm);
	pthread_mutex_unlock(&p_v4l2->path_mutex[channel_id]);
	return ret;
}

cmr_int cmr_v4l2_cap_pause(cmr_handle v4l2_handle, cmr_u32 channel_id, cmr_u32 reconfig_flag)
{
	cmr_int                  ret = 0;
	struct v4l2_streamparm   stream_parm;
	struct cmr_v4l2          *p_v4l2;

	p_v4l2 = (struct cmr_v4l2*)v4l2_handle;
	CMR_CHECK_HANDLE;
	CMR_CHECK_FD;
	CMR_LOGI("channel_id %d,reconfig_flag %d.", channel_id,reconfig_flag);

	stream_parm.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	stream_parm.parm.capture.capability = PATH_PAUSE;

	stream_parm.parm.capture.reserved[0] = channel_id;
	stream_parm.parm.capture.reserved[1] = reconfig_flag;
	pthread_mutex_lock(&p_v4l2->path_mutex[channel_id]);
	p_v4l2->chn_status[channel_id] = CHN_IDLE;
	ret = ioctl(p_v4l2->fd, VIDIOC_S_PARM, &stream_parm);
	pthread_mutex_unlock(&p_v4l2->path_mutex[channel_id]);
	return ret;
}

cmr_int cmr_v4l2_get_cap_time(cmr_handle v4l2_handle, cmr_u32 *sec, cmr_u32 *usec)
{
	cmr_int                  ret = 0;
	struct v4l2_streamparm   stream_parm;
	struct cmr_v4l2          *p_v4l2 = (struct cmr_v4l2*)v4l2_handle;

	p_v4l2 = (struct cmr_v4l2*)v4l2_handle;
	CMR_CHECK_HANDLE;
	stream_parm.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	ret = ioctl(p_v4l2->fd, VIDIOC_G_PARM, &stream_parm);
	CMR_RTN_IF_ERR(ret);

	*sec  = stream_parm.parm.capture.timeperframe.numerator;
	*usec = stream_parm.parm.capture.timeperframe.denominator;
	CMR_LOGI("sec=%d, usec=%d \n", *sec, *usec);

exit:
	return ret;

}

cmr_int cmr_v4l2_free_frame(cmr_handle v4l2_handle, cmr_u32 channel_id, cmr_u32 index)
{
	cmr_int                  ret = 0;
	struct v4l2_buffer       v4l2_buf;
	struct cmr_v4l2          *p_v4l2;

	p_v4l2 = (struct cmr_v4l2*)v4l2_handle;
	CMR_CHECK_HANDLE;
	CMR_CHECK_FD;
	CMR_LOGV("channel id %d, index 0x%x", channel_id, index);
	pthread_mutex_lock(&p_v4l2->status_mutex);
	if (0 == p_v4l2->is_on) {
		pthread_mutex_unlock(&p_v4l2->status_mutex);
		return ret;
	}
	pthread_mutex_unlock(&p_v4l2->status_mutex);
	pthread_mutex_lock(&p_v4l2->path_mutex[channel_id]);
	if (CHN_BUSY != p_v4l2->chn_status[channel_id]) {
		CMR_LOGI("channel %d not on, no need to free current frame", channel_id);
		pthread_mutex_unlock(&p_v4l2->path_mutex[channel_id]);
		return ret;
	}
	bzero(&v4l2_buf, sizeof(struct v4l2_buffer));
	v4l2_buf.flags = CMR_V4L2_WRITE_FREE_FRAME;
	v4l2_buf.type  = channel_id;
	v4l2_buf.index = index;
	ret = write(p_v4l2->fd, &v4l2_buf, sizeof(struct v4l2_buffer));
	pthread_mutex_unlock(&p_v4l2->path_mutex[channel_id]);
	if (ret) {
		CMR_LOGE("Failed to free frame, %ld", ret);
		ret = 0;
	}
	return ret;
}

cmr_int cmr_v4l2_scale_capability(cmr_handle v4l2_handle, cmr_u32 *width, cmr_u32 *sc_factor, cmr_u32 *sc_threshold)
{
	cmr_u32                  rd_word[3];
	cmr_int                  ret = 0;
	struct cmr_v4l2          *p_v4l2;

	if (NULL == width || NULL == sc_factor) {
		CMR_LOGE("Wrong param, 0x%x 0x%x", (cmr_u32)width, (cmr_u32)sc_factor);
		return -ENODEV;
	}
	p_v4l2 = (struct cmr_v4l2*)v4l2_handle;
	CMR_CHECK_HANDLE;
	CMR_CHECK_FD;

	ret = read(p_v4l2->fd, rd_word, 3*sizeof(cmr_u32));
	*width           = rd_word[0];
	*sc_factor       = rd_word[1];
	*sc_threshold    = rd_word[2];
	CMR_LOGI("width %d, sc_factor %d, sc_threshold %d", *width, *sc_factor, *sc_threshold);
	return ret;
}


static cmr_s32 cmr_v4l2_evt_id(cmr_s32 isr_flag)
{
	cmr_s32                      ret = CMR_V4L2_MAX;

	switch (isr_flag) {
	case V4L2_TX_DONE:
		ret = CMR_V4L2_TX_DONE;
		break;

	case V4L2_NO_MEM:
		ret = CMR_V4L2_TX_NO_MEM;
		break;

	case V4L2_TX_ERR:
		ret = CMR_V4L2_TX_ERROR;
		break;

	case V4L2_CSI2_ERR:
		ret = CMR_V4L2_CSI2_ERR;
		break;

	case V4L2_TIMEOUT:
		ret = CMR_V4L2_TIME_OUT;
		break;

	default:
		CMR_LOGI("isr_flag 0x%x", isr_flag);
		break;
	}

	return ret;
}

static cmr_int   cmr_v4l2_create_thread(cmr_handle v4l2_handle)
{
	cmr_int                  ret = 0;
	pthread_attr_t           attr;
	struct cmr_v4l2          *p_v4l2;

	p_v4l2 = (struct cmr_v4l2*)v4l2_handle;
	CMR_CHECK_HANDLE;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
	ret = pthread_create(&p_v4l2->thread_handle, &attr, cmr_v4l2_thread_proc, (void*)v4l2_handle);
	pthread_attr_destroy(&attr);
	return ret;
}

static cmr_int cmr_v4l2_kill_thread(cmr_handle v4l2_handle)
{
	cmr_int                  ret = 0;
	struct v4l2_buffer       v4l2_buf;
	void                     *dummy;
	struct cmr_v4l2          *p_v4l2;

	p_v4l2 = (struct cmr_v4l2*)v4l2_handle;
	CMR_CHECK_HANDLE;
	CMR_CHECK_FD;

	CMR_LOGI("Call write function to kill v4l2 manage thread");
	bzero(&v4l2_buf, sizeof(struct v4l2_buffer));
	v4l2_buf.flags = CMR_V4L2_WRITE_STOP;
	ret = write(p_v4l2->fd, &v4l2_buf, sizeof(struct v4l2_buffer)); // kill thread;
	if (ret > 0) {
		CMR_LOGI("write OK!");
		ret = pthread_join(p_v4l2->thread_handle, &dummy);
		p_v4l2->thread_handle = 0;
	}

	return ret;
}

static void* cmr_v4l2_thread_proc(void* data)
{
	struct v4l2_buffer       buf;
	cmr_s32                  evt_id;
	struct frm_info          frame;
	cmr_u32                  on_flag = 0;
	cmr_s32                  frm_num = -1;
	struct cmr_v4l2          *p_v4l2;
	struct img_data_end      endian;

	p_v4l2 = (struct cmr_v4l2*)data;
	if (!p_v4l2)
		return NULL;
	if (-1 == p_v4l2->fd)
		return NULL;

	CMR_LOGI("In");

	while(1) {
		buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		if (-1 == ioctl(p_v4l2->fd, VIDIOC_DQBUF, &buf)) {
			CMR_LOGI("Failed to DQBuf");
			break;
		} else {
			if (V4L2_TX_STOP == buf.flags) {
				// stopped , to do release resource
				CMR_LOGI("TX Stopped, exit thread");
				break;
			} else if (V4L2_SYS_BUSY == buf.flags || V4L2_TX_CLEAR == buf.flags) {
				usleep(10000);
				CMR_LOGI("continue. flag %d", buf.flags);
				continue;
			} else {
				// normal irq
				evt_id = cmr_v4l2_evt_id(buf.flags);
				if (CMR_V4L2_MAX == evt_id) {
					continue;
				}

				pthread_mutex_lock(&p_v4l2->path_mutex[buf.type]);
				if (CHN_IDLE == p_v4l2->chn_status[buf.type]) {
					pthread_mutex_unlock(&p_v4l2->path_mutex[buf.type]);
					continue;
				}
				pthread_mutex_unlock(&p_v4l2->path_mutex[buf.type]);

				frame.channel_id = buf.type;
				frm_num = p_v4l2->chn_frm_num[buf.type];
				frame.free = 0;
				if (CMR_V4L2_TX_DONE == evt_id) {
					if (frm_num == 0) {
						frame.free = 1;
					} else if (-1 != frm_num) {
						frm_num--;
					}
				}

				if (CHN_1 == frame.channel_id) {
					p_v4l2->chn_frm_num[CHN_1] = frm_num;
				} else if (CHN_2 == frame.channel_id) {
					p_v4l2->chn_frm_num[CHN_2] = frm_num;
				} else if (CHN_0 == frame.channel_id) {
					p_v4l2->chn_frm_num[CHN_0] = frm_num;
				}

				if ((p_v4l2->is_prev_trace && CHN_1 == frame.channel_id)
					|| (p_v4l2->is_cap_trace && CHN_1 != frame.channel_id))
					CMR_LOGI("got one frame! type 0x%x, id 0x%x, evt_id 0x%x sec %lu usec %lu",
						buf.type,
						buf.index,
						evt_id,
						(cmr_uint)buf.timestamp.tv_sec,
						(cmr_uint)buf.timestamp.tv_usec);
				else
					CMR_LOGV("got one frame! type 0x%x, id 0x%x, evt_id 0x%x sec %lu usec %lu",
						buf.type,
						buf.index,
						evt_id,
						(cmr_uint)buf.timestamp.tv_sec,
						(cmr_uint)buf.timestamp.tv_usec);

				frame.height          = buf.reserved;
				frame.frame_id        = buf.index;
				frame.frame_real_id   = buf.field;
				frame.sec             = buf.timestamp.tv_sec;
				frame.usec            = buf.timestamp.tv_usec;
				frame.length          = buf.sequence;
				frame.base            = buf.length;
				frame.fmt             = cmr_v4l2_get_img_type(buf.memory);
				pthread_mutex_lock(&p_v4l2->status_mutex);
				on_flag = p_v4l2->is_on;
				pthread_mutex_unlock(&p_v4l2->status_mutex);
				if (on_flag) {
					pthread_mutex_lock(&p_v4l2->cb_mutex);
					if (p_v4l2->v4l2_evt_cb) {
						(*p_v4l2->v4l2_evt_cb)(evt_id, &frame, (void*)p_v4l2->init_param.oem_handle);
					}
					pthread_mutex_unlock(&p_v4l2->cb_mutex);
				}

			}

		}
	}

	CMR_LOGI("Out");
	return NULL;
}

static cmr_u32 cmr_v4l2_get_4cc(cmr_u32 img_type)
{
	cmr_u32                 ret_4cc;

	switch (img_type) {
	case IMG_DATA_TYPE_YUV422:
		ret_4cc = V4L2_PIX_FMT_YUV422P;
		break;

	case IMG_DATA_TYPE_YUV420:
		ret_4cc = V4L2_PIX_FMT_NV21;
		break;

	case IMG_DATA_TYPE_YVU420:
		ret_4cc = V4L2_PIX_FMT_NV12;
		break;

	case IMG_DATA_TYPE_YUV420_3PLANE:
		ret_4cc = V4L2_PIX_FMT_YUV420;
		break;

	case IMG_DATA_TYPE_RGB565:
		ret_4cc = V4L2_PIX_FMT_RGB565;
		break;

	case IMG_DATA_TYPE_RAW:
		ret_4cc = V4L2_PIX_FMT_GREY;
		break;

	case IMG_DATA_TYPE_JPEG:
		ret_4cc = V4L2_PIX_FMT_JPEG;
		break;

	default:
		ret_4cc = V4L2_PIX_FMT_NV21;
		break;
	}

	CMR_LOGI("fmt %d", img_type);

	return ret_4cc;
}

static cmr_u32 cmr_v4l2_get_img_type(uint32_t fourcc)
{
	uint32_t                 img_type;

	switch (fourcc) {
	case V4L2_PIX_FMT_YUV422P:
		img_type = IMG_DATA_TYPE_YUV422;
		break;

	case V4L2_PIX_FMT_NV21:
		img_type = IMG_DATA_TYPE_YUV420;
		break;

	case V4L2_PIX_FMT_NV12:
		img_type = IMG_DATA_TYPE_YVU420;
		break;

	case V4L2_PIX_FMT_YUV420:
		img_type = IMG_DATA_TYPE_YUV420_3PLANE;
		break;

	case V4L2_PIX_FMT_RGB565:
		img_type = IMG_DATA_TYPE_RGB565;
		break;

	case V4L2_PIX_FMT_GREY:
		img_type = IMG_DATA_TYPE_RAW;
		break;

	case V4L2_PIX_FMT_JPEG:
		img_type = IMG_DATA_TYPE_JPEG;
		break;

	default:
		img_type = IMG_DATA_TYPE_YUV420;
		break;
	}

	CMR_LOGI("fmt %d", img_type);

	return img_type;
}

static cmr_u32 cmr_v4l2_get_data_endian(struct img_data_end* in_endian, struct img_data_end* out_endian)
{
	uint32_t                 img_type;

	if (NULL == in_endian || NULL == out_endian) {
		CMR_LOGE("Wrong param");
		return -ENODEV;
	}

	switch (in_endian->uv_endian) {
	case V4L2_ENDIAN_LITTLE:
		out_endian->uv_endian = IMG_DATA_ENDIAN_2PLANE_UVUV;
		break;

	case V4L2_ENDIAN_HALFBIG:
		out_endian->uv_endian = IMG_DATA_ENDIAN_2PLANE_VUVU;
		break;

	default:
		out_endian->uv_endian = IMG_DATA_ENDIAN_2PLANE_UVUV;
		break;
	}

	out_endian->y_endian = in_endian->y_endian;

	CMR_LOGI("y uv endian %d %d %d %d ", out_endian->y_endian, out_endian->uv_endian, in_endian->y_endian, in_endian->uv_endian);

	return 0;
}

cmr_u32 cmr_v4l2_get_dcam_endian(struct img_data_end* in_endian, struct img_data_end* out_endian)
{
	uint32_t                 img_type;

	if (NULL == in_endian || NULL == out_endian) {
		CMR_LOGE("Wrong param");
		return -ENODEV;
	}

	switch (in_endian->uv_endian) {
	case IMG_DATA_ENDIAN_2PLANE_UVUV:
		out_endian->uv_endian = V4L2_ENDIAN_LITTLE;
		break;

	case IMG_DATA_ENDIAN_2PLANE_VUVU:
		out_endian->uv_endian = V4L2_ENDIAN_HALFBIG;
		break;

	default:
		out_endian->uv_endian = V4L2_ENDIAN_LITTLE;
		break;
	}

	out_endian->y_endian = in_endian->y_endian;

	CMR_LOGI("y uv endian %d %d %d %d ", out_endian->y_endian, out_endian->uv_endian, in_endian->y_endian, in_endian->uv_endian);

	return 0;
}


cmr_int cmr_v4l2_flash_cb(cmr_handle v4l2_handle, cmr_u32 opt)
{
	cmr_int                  ret = 0;
	struct v4l2_streamparm   stream_parm;
	struct cmr_v4l2          *p_v4l2;

	p_v4l2 = (struct cmr_v4l2*)v4l2_handle;
	CMR_CHECK_HANDLE;
	CMR_CHECK_FD;
	stream_parm.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	stream_parm.parm.capture.capability = CAPTURE_SET_FLASH;
	stream_parm.parm.capture.reserved[0] = opt;
	ret = ioctl(p_v4l2->fd, VIDIOC_S_PARM, &stream_parm);
	if (ret) {
		CMR_LOGE("error");
	}
	return ret;
}
cmr_int cmr_v4l2_stream_cb(cmr_handle v4l2_handle, v4l2_stream_on str_on)
{
	cmr_int                  ret = 0;
	struct cmr_v4l2          *p_v4l2;

	p_v4l2 = (struct cmr_v4l2*)v4l2_handle;
	CMR_CHECK_HANDLE;
	p_v4l2->stream_on_cb = str_on;

	return ret;
}

cmr_int cmr_v4l2_set_zoom_mode(cmr_handle v4l2_handle, cmr_u32 opt)
{
	cmr_int                  ret = 0;
	struct v4l2_streamparm   stream_parm;
	struct cmr_v4l2          *p_v4l2;

	p_v4l2 = (struct cmr_v4l2*)v4l2_handle;
	CMR_CHECK_HANDLE;
	CMR_CHECK_FD;
	stream_parm.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	stream_parm.parm.capture.capability = CAPTURE_SET_ZOOM_MODE;
	stream_parm.parm.capture.reserved[0] = opt;
	ret = ioctl(p_v4l2->fd, VIDIOC_S_PARM, &stream_parm);
	if (ret) {
		CMR_LOGE("error");
	}
	return ret;
}
