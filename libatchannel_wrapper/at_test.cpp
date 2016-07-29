#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LOG_TAG "AT_TEST"
#define LOG_NDEBUG 0
#include <cutils/log.h>

#include <AtChannelWrapper.h>

int main(int argc, char **argv)
{
    if (argc == 2) {
        char buf[128] = { 0 };
        int ret = sendAt(buf, sizeof(buf), 0, argv[1]);
        ALOGE("cmd:%s ret:%d err:%s", argv[1], ret, buf);
    }
    return 0;
}
