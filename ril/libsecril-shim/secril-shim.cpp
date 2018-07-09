#include "secril-shim.h"

#define ATOI_NULL_HANDLED(x) (x ? atoi(x) : 0)

/* A copy of the original RIL function table. */
static const RIL_RadioFunctions *origRilFunctions;

/* A copy of the ril environment passed to RIL_Init. */
static const struct RIL_Env *rilEnv;

/* Variables and methods for RIL_REQUEST_DEVICE_IDENTITY support */
static char imei[16];
static char imeisv[17];
static bool gotIMEI = false;
static bool gotIMEISV = false;
static bool inIMEIRequest = false;
static bool inIMEISVRequest = false;

static void onRequestDeviceIdentity(int request, void *data, size_t datalen, RIL_Token t);
static void onRequestCompleteDeviceIdentity(RIL_Token t, RIL_Errno e);

static void onRequestDeviceIdentity(int request, void *data, size_t datalen, RIL_Token t) {
	RLOGI("%s: got request %s (data:%p datalen:%d)\n", __FUNCTION__,
		requestToString(request),
		data, datalen);
	onRequestCompleteDeviceIdentity(t, (gotIMEI && gotIMEISV) ? RIL_E_SUCCESS : RIL_E_GENERIC_FAILURE);
}

static void onRequestUnsupportedRequest(int request, void *data, size_t datalen, RIL_Token t) {
	RequestInfo *pRI = (RequestInfo *)t;
	if (pRI != NULL && pRI->pCI != NULL) {
		if (!gotIMEI && !inIMEIRequest) {
			// Use this unsupported request to extract IMEI
			inIMEIRequest = true;
			RLOGI("%s: got request %s: Using this unsupported request to extract IMEI for RIL_REQUEST_DEVICE_IDENTITY\n", __FUNCTION__, requestToString(request));
			pRI->pCI->requestNumber = RIL_REQUEST_GET_IMEI;
			origRilFunctions->onRequest(pRI->pCI->requestNumber, NULL, 0, t);
			return;
		} else if (!gotIMEISV && !inIMEISVRequest) {
			// Use this unsupported request to extract IMEISV
			inIMEISVRequest = true;
			RLOGI("%s: got request %s: Using this unsupported request to extract IMEISV for RIL_REQUEST_DEVICE_IDENTITY\n", __FUNCTION__, requestToString(request));
			pRI->pCI->requestNumber = RIL_REQUEST_GET_IMEISV;
			origRilFunctions->onRequest(pRI->pCI->requestNumber, NULL, 0, t);
			return;
		}
	}
	RLOGE("%s: got unsupported request %s (data:%p datalen:%d)\n", __FUNCTION__,
		requestToString(request),
		data, datalen);
	rilEnv->OnRequestComplete(t, RIL_E_REQUEST_NOT_SUPPORTED, NULL, 0);
}

static bool onRequestGetRadioCapability(RIL_Token t)
{
	RIL_RadioCapability rc[1] =
	{
		{ /* rc[0] */
			RIL_RADIO_CAPABILITY_VERSION, /* version */
			0, /* session */
			RC_PHASE_CONFIGURED, /* phase */
			RAF_GSM | RAF_GPRS | RAF_EDGE | RAF_HSUPA | RAF_HSDPA | RAF_HSPA | RAF_HSPAP | RAF_UMTS, /* rat */
			{ /* logicalModemUuid */
				0,
			},
			RC_STATUS_SUCCESS /* status */
		}
	};
	rilEnv->OnRequestComplete(t, RIL_E_SUCCESS, rc, sizeof(rc));
	return true;
}

/* scx30g2 doesn't need this, so let's have a check flag */
#ifndef SCX30G_V2
static void onRequestAllowData(int request, void *data, size_t datalen, RIL_Token t) {
	RLOGI("%s: got request %s (data:%p datalen:%d)\n", __FUNCTION__,
			requestToString(request),
			data, datalen);

	const char rawHookCmd[] = { 0x09, 0x04 }; // RAW_HOOK_OEM_CMD_SWITCH_DATAPREFER
	bool allowed = *((int *)data) == 0 ? false : true;

	if (allowed) {
		RequestInfo *pRI = (RequestInfo *)t;
		pRI->pCI->requestNumber = RIL_REQUEST_OEM_HOOK_RAW;
		origRilFunctions->onRequest(pRI->pCI->requestNumber, (void *)rawHookCmd, sizeof(rawHookCmd), t);
	} else {
		rilEnv->OnRequestComplete(t, RIL_E_SUCCESS, NULL, 0);
	}
}
#endif

static void onRequestDial(int request, void *data, RIL_Token t) {
	RIL_Dial dial;
	RIL_CallDetails *cds;

	dial.address = ((RIL_Dial *) data)->address;
	dial.clir = ((RIL_Dial *) data)->clir;
	dial.uusInfo = ((RIL_Dial *) data)->uusInfo;

	/* Handle Samsung CallDetails stuff */
	cds = (RIL_CallDetails *) malloc(sizeof(RIL_CallDetails));
	memset(cds, 0, sizeof(RIL_CallDetails));
	cds->call_type = 0;
	cds->call_domain = 1;
	cds->getCsvFromExtras = strdup("");
	dial.callDetails = cds;

	origRilFunctions->onRequest(request, &dial, sizeof(dial), t);

	/* Free this buffer (cds) */
	free(cds->getCsvFromExtras);
	free(cds);
	RLOGI("memory free'd");
}

static void onRequestShim(int request, void *data, size_t datalen, RIL_Token t)
{
	switch (request) {
		/* RIL_REQUEST_GET_IMEI is depricated */
		case RIL_REQUEST_DEVICE_IDENTITY:
			onRequestDeviceIdentity(request, data, datalen, t);
			RLOGI("%s: got request %s: replied with our implementation!\n", __FUNCTION__, requestToString(request));
			return;
		/* The Samsung RIL crashes if CallDetails is NULL... */
		case RIL_REQUEST_DIAL:
			if (datalen == sizeof(RIL_Dial) && data != NULL) {
				onRequestDial(request, data, t);
				RLOGI("%s: got request %s: replied with our implementation!\n", __FUNCTION__, requestToString(request));
				return;
			}
			break;
		/* Necessary; RILJ may fake this for us if we reply not supported, but we can just implement it. */
		case RIL_REQUEST_GET_RADIO_CAPABILITY:
			onRequestGetRadioCapability(t);
			RLOGI("%s: got request %s: replied with our implementation!\n", __FUNCTION__, requestToString(request));
			return;
	/* scx30g2 doesn't need this, so let's have a check flag */
	#ifndef SCX30G_V2
		case RIL_REQUEST_ALLOW_DATA:
			onRequestAllowData(request, data, datalen, t);
			RLOGI("%s: got request %s: replied with our implementation!\n", __FUNCTION__, requestToString(request));
			return;
	#endif
		/* The following requests were introduced post-4.3. */
		case RIL_REQUEST_SIM_TRANSMIT_APDU_BASIC:
		case RIL_REQUEST_SIM_OPEN_CHANNEL: /* !!! */
		case RIL_REQUEST_SIM_CLOSE_CHANNEL:
		case RIL_REQUEST_SIM_TRANSMIT_APDU_CHANNEL:
		case RIL_REQUEST_NV_READ_ITEM:
		case RIL_REQUEST_NV_WRITE_ITEM:
		case RIL_REQUEST_NV_WRITE_CDMA_PRL:
		case RIL_REQUEST_NV_RESET_CONFIG:
		case RIL_REQUEST_SET_UICC_SUBSCRIPTION:
		case RIL_REQUEST_GET_HARDWARE_CONFIG:
		case RIL_REQUEST_SIM_AUTHENTICATION:
		case RIL_REQUEST_GET_DC_RT_INFO:
		case RIL_REQUEST_SET_DC_RT_INFO_RATE:
		case RIL_REQUEST_SET_DATA_PROFILE:
		case RIL_REQUEST_SHUTDOWN: /* TODO: Is there something we can do for RIL_REQUEST_SHUTDOWN ? */
		case RIL_REQUEST_SET_RADIO_CAPABILITY:
		case RIL_REQUEST_START_LCE:
		case RIL_REQUEST_STOP_LCE:
		case RIL_REQUEST_PULL_LCEDATA:
			onRequestUnsupportedRequest(request, data, datalen, t);
			return;
	}

	RLOGD("%s: got request %s: forwarded to RIL.\n", __FUNCTION__, requestToString(request));
	origRilFunctions->onRequest(request, data, datalen, t);
}

static void onRequestCompleteDeviceIdentity(RIL_Token t, RIL_Errno e) {
	char empty[1] = "";
	char *deviceIdentityResponse[4];
	deviceIdentityResponse[0] = imei;
	deviceIdentityResponse[1] = imeisv;
	deviceIdentityResponse[2] = empty;
	deviceIdentityResponse[3] = empty;

	rilEnv->OnRequestComplete(t, e, deviceIdentityResponse, sizeof(deviceIdentityResponse));
}

static void onRequestCompleteGetImei(RIL_Token t, RIL_Errno /*e*/, void *response, size_t responselen) {
	memcpy(&imei, response, responselen);
	RLOGI("%s: RIL_REQUEST_DEVICE_IDENTITY [1/2]: Got IMEI:%s\n", __FUNCTION__, imei);
	rilEnv->OnRequestComplete(t, RIL_E_REQUEST_NOT_SUPPORTED, NULL, 0);
	inIMEIRequest = false;
	gotIMEI = true;
}

static void onRequestCompleteGetImeiSv(RIL_Token t, RIL_Errno /*e*/, void *response, size_t responselen) {
	memcpy(&imeisv, response, responselen);
	RLOGI("%s: RIL_REQUEST_DEVICE_IDENTITY [2/2]: Got IMEISV:%s\n", __FUNCTION__, imeisv);
	rilEnv->OnRequestComplete(t, RIL_E_REQUEST_NOT_SUPPORTED, NULL, 0);
	inIMEISVRequest = false;
	gotIMEISV = true;
}

static void fixupSignalStrength(void *response) {
	int gsmSignalStrength;

	RIL_SignalStrength_v10 *p_cur = ((RIL_SignalStrength_v10 *) response);

	gsmSignalStrength = p_cur->GW_SignalStrength.signalStrength & 0xFF;

	if (gsmSignalStrength < 0 ||
		(gsmSignalStrength > 31 && p_cur->GW_SignalStrength.signalStrength != 99)) {
		gsmSignalStrength = p_cur->CDMA_SignalStrength.dbm;
	}

	/* Fix GSM signal strength */
	p_cur->GW_SignalStrength.signalStrength = gsmSignalStrength;

	/* We don't support LTE - values should be set to INT_MAX */
	p_cur->LTE_SignalStrength.cqi = INT_MAX;
	p_cur->LTE_SignalStrength.rsrp = INT_MAX;
	p_cur->LTE_SignalStrength.rsrq = INT_MAX;
	p_cur->LTE_SignalStrength.rssnr = INT_MAX;
}

static void onCompleteRequestGetSimStatus(RIL_Token t, RIL_Errno e, void *response) {
	RIL_CardStatus_v6_samsung *p_cur = (RIL_CardStatus_v6_samsung *) response;
	RIL_CardStatus_v6 v6response;

	v6response.card_state = p_cur->card_state;
	v6response.universal_pin_state = p_cur->universal_pin_state;
	v6response.gsm_umts_subscription_app_index = p_cur->gsm_umts_subscription_app_index;
	v6response.cdma_subscription_app_index = p_cur->cdma_subscription_app_index;
	v6response.ims_subscription_app_index = p_cur->ims_subscription_app_index;
	v6response.num_applications = p_cur->num_applications;

	for (int i = 0; i < RIL_CARD_MAX_APPS; ++i)
		memcpy(&v6response.applications[i], &p_cur->applications[i], sizeof(RIL_AppStatus));

	/* Send the fixed response to libril */
	rilEnv->OnRequestComplete(t, e, &v6response, sizeof(RIL_CardStatus_v6));
}

static void onRequestCompleteShim(RIL_Token t, RIL_Errno e, void *response, size_t responselen) {
	int request;
	RequestInfo *pRI;

	pRI = (RequestInfo *)t;

	/* If pRI is null, this entire function is useless. */
	if (pRI == NULL) {
		RLOGE("pRI is NULL!");
		goto null_token_exit;
	}

	/* If pCI is null or invalid pointer, this entire function is useless. */
	if (!pRI->pCI || (0 <(int)pRI->pCI && (int)pRI->pCI < 0xff)) {
		RLOGE("Invalid CommandInterface pointer: %p", pRI->pCI);
		goto null_token_exit;
	}

	request = pRI->pCI->requestNumber;
	RLOGD("Request: %d", request);

	switch (request) {
		case RIL_REQUEST_GET_IMEI:
			RLOGD("%s: got request %s to support %s and shimming response!\n",
				__FUNCTION__, requestToString(request), requestToString(RIL_REQUEST_DEVICE_IDENTITY));
			onRequestCompleteGetImei(t, e, response, responselen);
			return;
		case RIL_REQUEST_GET_IMEISV:
			RLOGD("%s: got request %s to support %s and shimming response!\n",
				__FUNCTION__, requestToString(request), requestToString(RIL_REQUEST_DEVICE_IDENTITY));
			onRequestCompleteGetImeiSv(t, e, response, responselen);
			return;
		case RIL_REQUEST_GET_SIM_STATUS:
			/* Remove unknown elements from Samsung's struct */
			if (response != NULL && responselen == sizeof(RIL_CardStatus_v6_samsung)) {
				RLOGD("%s: got request %s and shimming response!\n", __FUNCTION__, requestToString(request));
				onCompleteRequestGetSimStatus(t, e, response);
				return;
			}
			break;
		case RIL_REQUEST_LAST_CALL_FAIL_CAUSE:
			/* Remove extra element (ignored on pre-M, now crashing the framework) */
			if (responselen > sizeof(int)) {
				RLOGD("%s: got request %s and shimming response!\n", __FUNCTION__, requestToString(request));
				rilEnv->OnRequestComplete(t, e, response, sizeof(int));
				return;
			}
			break;
		case RIL_REQUEST_SIGNAL_STRENGTH:
			/* The Samsung RIL reports the signal strength in a strange way... */
			if (response != NULL && responselen >= sizeof(RIL_SignalStrength_v5)) {
				RLOGD("%s: got request %s and shimming response!\n", __FUNCTION__, requestToString(request));
				fixupSignalStrength(response);
				rilEnv->OnRequestComplete(t, e, response, responselen);
				return;
			}
			break;
	}
	RLOGD("%s: got request %s: forwarded to libril.\n", __FUNCTION__, requestToString(request));

null_token_exit:
	rilEnv->OnRequestComplete(t, e, response, responselen);
}

static void onUnsolicitedResponseShim(int unsolResponse, const void *data, size_t datalen)
{
	switch (unsolResponse) {
		case RIL_UNSOL_SIGNAL_STRENGTH:
			/* The Samsung RIL reports the signal strength in a strange way... */
			if (data != NULL && datalen >= sizeof(RIL_SignalStrength_v5))
				fixupSignalStrength((void*) data);
			break;
	}

	rilEnv->OnUnsolicitedResponse(unsolResponse, data, datalen);
}

const RIL_RadioFunctions* RIL_Init(const struct RIL_Env *env, int argc, char **argv)
{
	RIL_RadioFunctions const* (*origRilInit)(const struct RIL_Env *env, int argc, char **argv);
	static RIL_RadioFunctions shimmedFunctions;
	static struct RIL_Env shimmedEnv;
	void *origRil;

	/* Shim the RIL_Env passed to the real RIL, saving a copy of the original */
	rilEnv = env;
	shimmedEnv = *env;
	shimmedEnv.OnRequestComplete = onRequestCompleteShim;
	shimmedEnv.OnUnsolicitedResponse = onUnsolicitedResponseShim;

	/* Open and Init the original RIL. */

	origRil = dlopen(RIL_LIB_PATH, RTLD_GLOBAL);
	if (CC_UNLIKELY(!origRil)) {
		RLOGE("%s: failed to load '" RIL_LIB_PATH  "': %s\n", __FUNCTION__, dlerror());
		return NULL;
	}

	origRilInit = (const RIL_RadioFunctions *(*)(const struct RIL_Env *, int, char **))(dlsym(origRil, "RIL_Init"));
	if (CC_UNLIKELY(!origRilInit)) {
		RLOGE("%s: couldn't find original RIL_Init!\n", __FUNCTION__);
		goto fail_after_dlopen;
	}

	origRilFunctions = origRilInit(&shimmedEnv, argc, argv);
	RLOGD("Assigning original RIL funcs...");
	if (CC_UNLIKELY(!origRilFunctions)) {
		RLOGE("%s: the original RIL_Init derped.\n", __FUNCTION__);
		goto fail_after_dlopen;
	}

	/* Shim functions as needed. */
	shimmedFunctions = *origRilFunctions;
	shimmedFunctions.onRequest = onRequestShim;

	RLOGD("RIL_Init completed");
	return &shimmedFunctions;

fail_after_dlopen:
	dlclose(origRil);
	return NULL;
}
