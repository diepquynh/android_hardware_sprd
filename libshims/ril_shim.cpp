/*
 * Copyright (C) 2015 The CyanogenMod Project
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

#include <dlfcn.h>
#include <sys/types.h>

/* status_t Parcel::writeString16 */
extern "C" int _ZN7android6Parcel13writeString16EPKDsj();
extern "C" int _ZN7android6Parcel13writeString16EPKtj() {
    return _ZN7android6Parcel13writeString16EPKDsj();
}

extern "C" int strncmp(const char *s1, const char *s2, size_t n) {
    typedef int (*libc_strncmp_t)(const char *, const char *, size_t);
    static libc_strncmp_t libc_strncmp = (libc_strncmp_t)
            dlsym(dlopen("libc.so", RTLD_LAZY), "strncmp");
    if (!s1 && libc_strncmp("GLB", s2, 3) == 0)
        return libc_strncmp("0", s2, n);
    return libc_strncmp(s1 , s2, n);
}
