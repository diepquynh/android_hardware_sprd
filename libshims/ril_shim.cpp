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

#include <string.h>
#include <stdio.h>

#include <dlfcn.h>

#include <algorithm>
#include <exception>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#define LOG_NDEBUG 0
#define LOG_TAG "RIL_SHIM"
#include <cutils/log.h>

#define PROPERTY_VALUE_MAX 92

static int (*__real_property_set)(const char *, const char *);
static int (*__real_property_get)(const char *, char *, const char *);
static void constructor() __attribute__((constructor));
static void destructor() __attribute__((destructor));

static void *libcutils_handle;

/* { RIL_KEYS, { ANDROID_KEY, ANDROID_VALUE_INDEXES } } */
static std::unordered_map<std::string, std::pair<std::string, int>> KEY_MAP
{
    { "gsm.operator.numeric",
            { "gsm.operator.numeric", 0 } },
    { "gsm.operator.numeric2",
            { "gsm.operator.numeric", 1 } },
    { "gsm.sim.operator.numeric",
            { "gsm.sim.operator.numeric", 0 } },
    { "gsm.sim.operator.numeric2",
            { "gsm.sim.operator.numeric", 1 } },
    { "gsm.network.type",
            { "gsm.network.type", 0 } },
    { "gsm.network.type2",
            { "gsm.network.type", 1 } },
};

void constructor()
{
    bool error = true;
    if ((libcutils_handle = dlopen("/system/lib/libcutils.so", RTLD_LAZY))) {
        __real_property_set = (typeof __real_property_set) dlsym(libcutils_handle, "property_set");
        __real_property_get = (typeof __real_property_get) dlsym(libcutils_handle, "property_get");
        if (__real_property_set && __real_property_get)
            error = false;
    }
    if (error)
        throw new std::exception();
}

void destructor()
{
    if (!dlclose(libcutils_handle))
        throw new std::exception();
}

static std::vector<std::string> split(const std::string &s, char delim)
{
    std::vector<std::string> result;
    std::stringstream ss;
    ss.str(s);
    std::string item;
    while (getline(ss, item, delim)) result.push_back(item);
    return result;
}

static std::string concat(const std::vector<std::string> &v, char delim)
{
    if (v.size()) {
        auto s = *v.begin();
        for (auto i = v.begin() + 1; i != v.end(); ++i) {
            s += delim;
            s += *i;
        }
        return s;
    }
    return std::string();
}

/* status_t Parcel::writeString16 */
extern "C" int _ZN7android6Parcel13writeString16EPKDsj();
extern "C" int _ZN7android6Parcel13writeString16EPKtj() {
    return _ZN7android6Parcel13writeString16EPKDsj();
}

extern "C" int property_get(const char *key, char *value, const char *default_value)
{
    auto search = KEY_MAP.find(key);
    if (search != KEY_MAP.end()) {
        std::string actual_key = (*search).second.first;
        int index = (*search).second.second;
        if (__real_property_get(actual_key.c_str(), value, default_value) > 0) {
            std::vector<std::string> v = split(value, ',');
            if (index < v.size())
                strcpy(value, v[index].c_str());
            else
                strcpy(value, default_value);
        }
        return strlen(value);
    }
    return __real_property_get(key, value, default_value);
}

extern "C" int property_set(const char *key, const char *value)
{
    auto search = KEY_MAP.find(key);
    if (search != KEY_MAP.end()) {
        std::string actual_key = (*search).second.first;
        int index = (*search).second.second;
        char tmp[PROPERTY_VALUE_MAX];
        if (__real_property_get(actual_key.c_str(), tmp, "") >= 0) {
            std::vector<std::string> v = split(tmp, ',');
            if (!(index < v.size()))
                v.resize(index + 1);
            v[index] = value;
            std::string actual_value = concat(v, ',');
            return __real_property_set(actual_key.c_str(), actual_value.c_str());
        }
    }
    return __real_property_set(key, value);
}
