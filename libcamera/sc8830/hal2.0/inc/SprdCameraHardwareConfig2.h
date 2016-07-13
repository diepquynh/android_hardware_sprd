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
#ifndef _SPRD_CAMERA_HARDWARE_CONFIG_H_
#define _SPRD_CAMERA_HARDWARE_CONFIG_H_

#define CAMERA_DIGITAL_ZOOM_MAX     2
#define JPEG_THUMBNAIL_WIDTH        320
#define JPEG_THUMBNAIL_HEIGHT       240

typedef enum _ae_state {
	AE_STATE_INACTIVE = 1,
	AE_STATE_SEARCHING,
	AE_STATE_CONVERGED,
	AE_STATE_LOCKED,
	AE_STATE_FLASH_REQUIRED,
	AE_STATE_PRECAPTURE
}ae_state;

typedef enum _awb_lock {
	AWB_LOCK_OFF,
	AWB_LOCK_ON
}awb_lock;

typedef enum _ae_lock {
	AE_LOCK_OFF,
	AE_LOCK_ON
}ae_lock;

#endif //_SPRD_CAMERA_HARDWARE_CONFIG_H_
