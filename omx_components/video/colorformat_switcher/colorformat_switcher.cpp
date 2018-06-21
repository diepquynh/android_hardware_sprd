/*
 * Copyright (C) 2010 The Android Open Source Project
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

#include <inttypes.h>
#include <utils/Trace.h>
#include <gralloc_priv.h>
#include "colorformat_switcher.h"

void setColorFormat(OMX_COLOR_FORMATTYPE &eNativeColorFormat)
{
    switch (eNativeColorFormat) {
        case OMX_COLOR_FormatYUV420SemiPlanar:
            eNativeColorFormat = (OMX_COLOR_FORMATTYPE)HAL_PIXEL_FORMAT_YCbCr_420_SP;
            break;
        case OMX_COLOR_FormatYUV420Planar:
        default:
            eNativeColorFormat = (OMX_COLOR_FORMATTYPE)HAL_PIXEL_FORMAT_YCbCr_420_P;
            break;
    }
}
