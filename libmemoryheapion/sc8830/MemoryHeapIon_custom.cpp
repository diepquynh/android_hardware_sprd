/*
 * Copyright (C) 2008 The Android Open Source Project
 * Copyright (c) 2011, Code Aurora Forum. All rights reserved.
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

#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/mman.h>

#include <cutils/log.h>
#include <cutils/atomic.h>

#include <linux/ion.h>
#include <video/ion_sprd.h>

#include "MemoryHeapIon.h"

namespace android {

#ifdef SCX30G_V2
int MemoryHeapIon::get_iova_custom(int master_id, unsigned long *mmu_addr, size_t *size) {
    if(mIonDeviceFd<0){
        ALOGE("%s:open dev ion error!",__func__);
        return -1;
    }else{
        int ret;
        struct ion_mmu_data mmu_data;
        struct ion_custom_data  custom_data;

        mmu_data.master_id = master_id;
        mmu_data.fd_buffer = mFD;
        custom_data.cmd = ION_SPRD_CUSTOM_MAP;
        custom_data.arg = (unsigned long)&mmu_data;
        ret = ioctl(mIonDeviceFd,ION_IOC_CUSTOM,&custom_data);
        *mmu_addr = mmu_data.iova_addr;
        *size = mmu_data.iova_size;
        if(ret)
        {
            ALOGE("%s: return error: %d", __func__, ret);
            return -2;
        }
     }

    return 0;
}
#endif

int MemoryHeapIon::get_iova(int master_id, unsigned long *mmu_addr, size_t *size) {
#ifdef SCX30G_V2
    return get_iova_custom(master_id, mmu_addr, size);
#else
    switch(master_id) {
        case ION_GSP:
            return get_gsp_iova(mmu_addr, size);
        case ION_MM:
            return get_mm_iova(mmu_addr, size);
        default:
            break;
    }
    return -2;
#endif
}

#ifdef SCX30G_V2
int MemoryHeapIon::free_iova_custom(int master_id, unsigned long mmu_addr, size_t size) {
    if(mIonDeviceFd<0){
        ALOGE("%s:open dev ion error!",__func__);
        return -1;
    }else{
        int ret;
        struct ion_mmu_data mmu_data;
        struct ion_custom_data  custom_data;

        mmu_data.master_id = master_id;
        mmu_data.fd_buffer = mFD;
        mmu_data.iova_addr = mmu_addr;
        mmu_data.iova_size = size;
        custom_data.cmd = ION_SPRD_CUSTOM_UNMAP;
        custom_data.arg = (unsigned long)&mmu_data;
        ret = ioctl(mIonDeviceFd,ION_IOC_CUSTOM,&custom_data);
        if(ret)
        {
            ALOGE("%s: return error: %d", __func__, ret);
            return -2;
        }
     }

    return 0;
}
#endif

int MemoryHeapIon::free_iova(int master_id, unsigned long mmu_addr, size_t size) {
#ifdef SCX30G_V2
    return free_iova_custom(master_id, mmu_addr, size);
#else
    switch(master_id) {
        case ION_GSP:
            return free_gsp_iova(mmu_addr, size);
        case ION_MM:
            return free_mm_iova(mmu_addr, size);
        default:
            break;
    }
    return -2;
#endif
}

#ifdef SCX30G_V2
int MemoryHeapIon::Get_iova_custom(int master_id, int buffer_fd,
        unsigned long *mmu_addr, size_t *size) {
    int fd = open("/dev/ion", O_SYNC);

    if(fd<0){
        ALOGE("%s:open dev ion error!",__func__);
        return -1;
    }else{
        int ret;
        struct ion_mmu_data mmu_data;
        struct ion_custom_data  custom_data;

        mmu_data.master_id = master_id;
        mmu_data.fd_buffer =  buffer_fd;
        custom_data.cmd = ION_SPRD_CUSTOM_MAP;
        custom_data.arg = (unsigned long)&mmu_data;
        ret = ioctl(fd,ION_IOC_CUSTOM,&custom_data);
        *mmu_addr = mmu_data.iova_addr;
        *size = mmu_data.iova_size;
        close(fd);
        if(ret)
        {
            ALOGE("%s: return error: %d", __func__, ret);
            return -2;
        }
    }

    return 0;
}
#endif

int MemoryHeapIon::Get_iova(int master_id, int buffer_fd,
        unsigned long *mmu_addr, size_t *size) {
#ifdef SCX30G_V2
    return Get_iova_custom(master_id, buffer_fd, mmu_addr, size);
#else
    switch(master_id) {
        case ION_GSP:
            return Get_gsp_iova(buffer_fd, mmu_addr, size);
        case ION_MM:
            return Get_mm_iova(buffer_fd, mmu_addr, size);
        default:
            break;
    }
    return -2;
#endif
}

#ifdef SCX30G_V2
int MemoryHeapIon::Free_iova_custom(int master_id, int buffer_fd,
        unsigned long mmu_addr, size_t size) {
    int fd = open("/dev/ion", O_SYNC);

    if(fd<0){
        ALOGE("%s:open dev ion error!",__func__);
        return -1;
    }else{
        int ret;
        struct ion_mmu_data mmu_data;
        struct ion_custom_data  custom_data;

        mmu_data.master_id = master_id;
        mmu_data.fd_buffer = buffer_fd;
        mmu_data.iova_addr = mmu_addr;
        mmu_data.iova_size = size;
        custom_data.cmd = ION_SPRD_CUSTOM_UNMAP;
        custom_data.arg = (unsigned long)&mmu_data;
        ret = ioctl(fd,ION_IOC_CUSTOM,&custom_data);
        close(fd);
        if(ret)
        {
            ALOGE("%s: return error: %d", __func__, ret);
            return -2;
        }
    }

    return 0;
}
#endif

int MemoryHeapIon::Free_iova(int master_id, int buffer_fd,
        unsigned long mmu_addr, size_t size) {
    int ret = 0;
#ifdef SCX30G_V2
    return Free_iova_custom(master_id, buffer_fd, mmu_addr, size);
#else
    switch(master_id) {
        case ION_GSP:
            return Free_gsp_iova(buffer_fd, mmu_addr, size);
        case ION_MM:
            return Free_mm_iova(buffer_fd, mmu_addr, size);
        default:
            break;
    }
    return -2;
#endif
}

}; // namespace android
