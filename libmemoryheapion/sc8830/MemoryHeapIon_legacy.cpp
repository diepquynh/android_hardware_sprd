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

#include <stdint.h>

#include "MemoryHeapIon.h"

namespace android {

int MemoryHeapIon::get_phy_addr_from_ion(int *phy_addr, int *size) {
    unsigned long *phyaddr = (unsigned long *)phy_addr;
    size_t *sz = (size_t *)size;

    return get_phy_addr_from_ion(phyaddr, sz);
}

int MemoryHeapIon::Get_phy_addr_from_ion(int fd, int *phy_addr, int *size) {
    unsigned long *phyaddr = (unsigned long *)phy_addr;
    size_t *sz = (size_t *)size;

    return Get_phy_addr_from_ion(fd, phyaddr, sz);
}

int MemoryHeapIon::flush_ion_buffer(void *v_addr, void *p_addr,int size) {
    size_t sz = (size_t)size;

    return flush_ion_buffer(v_addr, p_addr, sz);
}

int MemoryHeapIon::Flush_ion_buffer(int buffer_fd, void *v_addr,void *p_addr, int size) {
    size_t sz = (size_t)size;

    return Flush_ion_buffer(buffer_fd, v_addr, p_addr, sz);
}

int MemoryHeapIon::get_gsp_iova(int *mmu_addr, int *size) {
    unsigned long *mmuaddr = (unsigned long *)mmu_addr;
    size_t *sz = (size_t *)size;

    return get_gsp_iova(mmuaddr, sz);
}

int MemoryHeapIon::free_gsp_iova(int mmu_addr, int size) {
    unsigned long mmuaddr = (unsigned long)mmu_addr;
    size_t sz = (size_t)size;

    return free_gsp_iova(mmuaddr, sz);
}

int MemoryHeapIon::get_mm_iova(int *mmu_addr, int *size) {
    unsigned long *mmuaddr = (unsigned long *)mmu_addr;
    size_t *sz = (size_t *)size;

    return get_mm_iova(mmuaddr, sz);
}

int MemoryHeapIon::free_mm_iova(int mmu_addr, int size) {
    unsigned long mmuaddr = (unsigned long)mmu_addr;
    size_t sz = (size_t)size;

    return free_mm_iova(mmuaddr, sz);
}

int MemoryHeapIon::Get_gsp_iova(int buffer_fd, int *mmu_addr, int *size) {
    unsigned long *mmuaddr = (unsigned long *)mmu_addr;
    size_t *sz = (size_t *)size;

    return Get_gsp_iova(buffer_fd, mmuaddr, sz);
}

int MemoryHeapIon::Free_gsp_iova(int buffer_fd, int mmu_addr, int size) {
    unsigned long mmuaddr = (unsigned long)mmu_addr;
    size_t sz = (size_t)size;

    return Free_gsp_iova(buffer_fd, mmuaddr, sz);
}

int MemoryHeapIon::Get_mm_iova(int buffer_fd,int *mmu_addr, int *size) {
    unsigned long *mmuaddr = (unsigned long *)mmu_addr;
    size_t *sz = (size_t *)size;

    return Get_mm_iova(buffer_fd, mmuaddr, sz);
}

int MemoryHeapIon::Free_mm_iova(int buffer_fd,int mmu_addr, int size) {
    unsigned long mmuaddr = (unsigned long)mmu_addr;
    size_t sz = (size_t)size;

    return Free_mm_iova(buffer_fd, mmuaddr, sz);
}

}; // namespace android
