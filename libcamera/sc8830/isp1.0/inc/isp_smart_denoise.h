/*
 * Copyright (C) 2012 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *		http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef _ISP_SMART_DENOISE_H_
#define _ISP_SMART_DENOISE_H_
/*----------------------------------------------------------------------------*
 **				 Dependencies					*
 **---------------------------------------------------------------------------*/
#include <sys/types.h>
//#include "..\inc\isp_com.h"
/**---------------------------------------------------------------------------*
 **				 Compiler Flag					*
 **---------------------------------------------------------------------------*/
#ifdef	 __cplusplus
extern	 "C"
{
#endif

/**---------------------------------------------------------------------------*
**				 Micro Define					*
**----------------------------------------------------------------------------*/

#define DENOISE_INDEX_NUM 10
#define DENOISE_LUM_NUM 4

struct isp_smart_denoise_info{
	uint16_t enable;
	uint16_t index_thr_num;
	uint16_t lum_low_thr_num;
	uint16_t index_thr[DENOISE_INDEX_NUM];
	uint16_t index_level[DENOISE_INDEX_NUM];
	uint16_t bais_gain[DENOISE_INDEX_NUM];
	uint16_t lum_level[DENOISE_LUM_NUM];
	uint16_t lum_low_thr[DENOISE_LUM_NUM];
};

struct isp_smart_denoise{
	uint16_t cur_index;
	uint16_t min_index;
	uint16_t max_index;

	uint16_t cur_lum;
	uint16_t target_lum_thr;

	uint16_t cur_gain;
	uint16_t max_gain;

	uint16_t dependon_gain;
	uint16_t dependon_lum;

	struct isp_smart_denoise_info bil_dis;
	struct isp_smart_denoise_info bil_ran;
	struct isp_smart_denoise_info pref_y;
	struct isp_smart_denoise_info pref_uv;
	struct isp_smart_denoise_info y;
	struct isp_smart_denoise_info uv;
	struct isp_smart_denoise_info edge;
};

struct isp_smart_denoise_out{
	uint16_t bil_dis_level;
	uint16_t bil_ran_level;
	uint16_t pref_y_level;
	uint16_t pref_uv_level;
	uint16_t y_level;
	uint16_t uv_level;
	uint16_t edge_level;
};

/**---------------------------------------------------------------------------*
**				Data Prototype					*
**----------------------------------------------------------------------------*/
int32_t isp_smart_denoise_adjust(struct isp_smart_denoise* in_param_ptr, struct isp_smart_denoise_out* out_param_ptr);

/**----------------------------------------------------------------------------*
**					Compiler Flag				**
**----------------------------------------------------------------------------*/
#ifdef	__cplusplus
}
#endif
/**---------------------------------------------------------------------------*/
#endif

