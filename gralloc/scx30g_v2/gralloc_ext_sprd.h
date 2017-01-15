#ifndef GRALLOC_PRIV_EXT_H_
#define GRALLOC_PRIV_EXT_H_
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/ion.h>
#include "ion_sprd.h"

enum
{
    /* OEM specific HAL formats */
    HAL_PIXEL_FORMAT_YCbCr_422_P        = 0x12,
    HAL_PIXEL_FORMAT_YCbCr_420_P        = 0x13,
    HAL_PIXEL_FORMAT_YCbCr_420_I        = 0x15,
    HAL_PIXEL_FORMAT_CbYCrY_422_I       = 0x16,
    HAL_PIXEL_FORMAT_CbYCrY_420_I       = 0x17,
    HAL_PIXEL_FORMAT_YCbCr_420_SP_TILED = 0x18,
    HAL_PIXEL_FORMAT_YCbCr_420_SP       = 0x19, /*OMX_COLOR_FormatYUV420SemiPlanar*/
    HAL_PIXEL_FORMAT_YCrCb_420_SP_TILED = 0x1A,
    HAL_PIXEL_FORMAT_YCrCb_422_SP       = 0x1B,
    HAL_PIXEL_FORMAT_YCrCb_420_P        = 0x1C,
}; 

enum
{
	GRALLOC_USAGE_SPRD_PRIVATE			=  0x01000000,
	GRALLOC_USAGE_OVERLAY_BUFFER		=  0x03000000,
	GRALLOC_USAGE_VIDEO_BUFFER			=  0x05000000,
	GRALLOC_USAGE_CAMERA_BUFFER 		=  0x05000000,
    GRALLOC_USAGE_HW_TILE_ALIGN         =  0x08000000,
};
#endif // GRALLOC_PRIV_EXT_H_
