#define LOG_TAG "IAtChannel"

#include <string.h>

#include <algorithm>

#include <android/log.h>
#include <cutils/log.h>
#include <cutils/properties.h>

#include <secril-client.h>

#include "AtChannel.h"

static const char *RET_ERROR = "ERROR"; // libatchannel blob return a pointer to this string on error
static const char *RET_SUCCESS = "find sril_SendATHandle"; // libatchannel blob return a pointer to this string on success

static HRilClient rilClient = nullptr;
static HRilClient rilClientSecond = nullptr;

#define  MSMS_PHONE_COUNT_PROP             "persist.msms.phone_count"
#define  MODEM_TD_ENABLE_PROP              "persist.modem.t.enable"
#define  MODEM_TD_ID_PROP                  "ro.modem.t.id"
#define  MODEM_TD_COUNT_PROP               "ro.modem.t.count"
#define  MODEM_WCDMA_ENABLE_PROP           "persist.modem.w.enable"
#define  MODEM_WCDMA_ID_PROP               "ro.modem.w.id"
#define  MODEM_WCDMA_COUNT_PROP            "ro.modem.w.count"
#define  MODEM_TL_ENABLE_PROP              "persist.modem.tl.enable"
#define  MODEM_TL_COUNT_PROP               "ro.modem.tl.count"
#define  MODEM_LF_ENABLE_PROP              "persist.modem.lf.enable"
#define  MODEM_LF_COUNT_PROP               "ro.modem.lf.count"

static int getPhoneId(int modemId, int simId)
{
    /*
     * Because there is no docs, LTE should be supported later.
     */
    char prop[PROPERTY_VALUE_MAX] = "";
    int tdEnable, wEnable,tlEnable,lfEnable;
    int tdCount,wCount,tlCount,lfCount;

    property_get(MODEM_TD_ENABLE_PROP, prop, "0");
    tdEnable = atoi(prop);
    memset(prop, '\0', sizeof(prop));
    property_get(MODEM_WCDMA_ENABLE_PROP, prop, "0");
    wEnable = atoi(prop);
    property_get(MODEM_TL_ENABLE_PROP, prop, "0");
    tlEnable = atoi(prop);
    memset(prop, '\0', sizeof(prop));
    property_get(MODEM_LF_ENABLE_PROP,prop, "0");
    lfEnable = atoi(prop);

    memset(prop, '\0', sizeof(prop));
    property_get(MODEM_TD_COUNT_PROP, prop, "0");
    tdCount = atoi(prop);
    memset(prop, '\0', sizeof(prop));
    property_get(MODEM_WCDMA_COUNT_PROP, prop, "0");
    wCount = atoi(prop);
    memset(prop, '\0', sizeof(prop));
    property_get(MODEM_TL_COUNT_PROP, prop, "0");
    tlCount = atoi(prop);
    memset(prop, '\0', sizeof(prop));
    property_get(MODEM_LF_COUNT_PROP, prop, "0");
    lfCount = atoi(prop);

    if (tdEnable) {
        if (wEnable) {
            return modemId * tdCount + simId;
        } else {
            return simId;
        }
    } else if(wEnable) {
           return simId;
    } else if(tlEnable) {
           return simId;
    } else {
        if(lfEnable) {
            return simId;
        } else {
            ALOGE("Both TD modem and WCDMA modem are disable, use default phoneID\n");
        }
    }

    return 0;
}

const char* sendAt(int modemId, int simId, const char* atCmd)
{
    const int phoneId = getPhoneId(modemId, simId);
    const size_t atCmdLen = strlen(atCmd);
    char oemReqHookRaw[1024] = { 0x30, 0x0 };
    int error = RIL_CLIENT_ERR_SUCCESS;
    int retryCount = 0;

    ALOGI("Receive sendAt request: modemId=%d simId=%d atCmd=\"%s\"", modemId, simId, atCmd);

    // Initialize connection
    HRilClient& client = (phoneId == 0) ? rilClient : rilClientSecond;
    if (client == nullptr && (client = OpenClient_RILD()) == nullptr) {
        ALOGE("OpenClient_RILD() failed!");
        return RET_ERROR;
    }
    if (!isConnected_RILD(client)) {
        error = (phoneId == 0) ? Connect_RILD(client) : Connect_RILD_Second(client);
        if (error != RIL_CLIENT_ERR_SUCCESS) {
            ALOGE("Cannot open connection to RILD: phoneId=%d error=%d", phoneId, error);
            return RET_ERROR;
        }
    }

    // Build and send OEM request hook
    memcpy(oemReqHookRaw + 2, atCmd, std::min(atCmdLen, sizeof(oemReqHookRaw) - 3));
    ALOGI("Sending OEM request hook raw for AT command: atCmd=\"%s\"", atCmd);
    do {
        if (error == RIL_CLIENT_ERR_AGAIN)
            ALOGI("Resend request: retryCount=%d error=%d", ++retryCount, error);
        error = InvokeOemRequestHookRaw(client, oemReqHookRaw, atCmdLen + 3);
    } while (error == RIL_CLIENT_ERR_AGAIN && retryCount < 10);

    if (error == RIL_CLIENT_ERR_SUCCESS)
        ALOGI("Send OEM request hook raw successfully.");
    else
        ALOGE("Failed to send OEM request hook raw: error=%d", error);

    return error == RIL_CLIENT_ERR_SUCCESS ? RET_SUCCESS : RET_ERROR;
}
