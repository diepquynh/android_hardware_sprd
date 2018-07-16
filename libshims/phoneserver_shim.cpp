/*
 * Copyright (C) 2018 The LineageOS Project
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
#include <pthread.h>

#define LOG_TAG "PHONESERVER_SHIM"
#define LOG_NDEBUG 0
#include <cutils/log.h>

extern "C" int pthread_setschedparam(pthread_t thread, int policy, const struct sched_param *param)
{
    ALOGI("%s: pointer=%p value=%lx", __FUNCTION__, (pthread_t*)thread, *((pthread_t*)thread));
    return ((int (*)(pthread_t, int, const struct sched_param*)) dlsym(RTLD_NEXT, "pthread_setschedparam"))(*((pthread_t *)thread), policy, param);
}

extern "C" int pthread_getschedparam(pthread_t thread, int* policy, struct sched_param* param)
{
    ALOGI("%s: pointer=%p value=%lx", __FUNCTION__, (pthread_t *)thread, *((pthread_t *)thread));
    return ((int (*)(pthread_t, int*, struct sched_param*)) dlsym(RTLD_NEXT, "pthread_getschedparam"))(*((pthread_t *)thread), policy, param);
}

extern "C" int pthread_create(pthread_t* thread, const pthread_attr_t* attr,void *(*start_rountine)(void*), void* arg)
{
    int result = ((int (*)(pthread_t*, const pthread_attr_t*,void *(*)(void*), void*)) dlsym(RTLD_NEXT, "pthread_create"))(thread, attr, start_rountine, arg);
    ALOGI("%s: pointer=%p, value=%lx", __FUNCTION__, thread, *thread);
    return result;
}
