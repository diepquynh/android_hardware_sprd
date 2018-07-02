#include "secril-shim.h"

#define ATOI_NULL_HANDLED(x) (x ? atoi(x) : 0)

/* A copy of the original RIL function table. */
static const RIL_RadioFunctions *origRilFunctions;

/* A copy of the ril environment passed to RIL_Init. */
static const struct RIL_Env *rilEnv;

/* Response data for RIL_REQUEST_VOICE_REGISTRATION_STATE */
static const int VOICE_REGSTATE_SIZE = 15 * sizeof(char *);
static char *voiceRegStateResponse[VOICE_REGSTATE_SIZE];

/* Store voice radio technology */
static int voiceRadioTechnology = -1;

/* Store cdma subscription source */
static int cdmaSubscriptionSource = -1;

/* Store sim ruim status */
int simRuimStatus = -1;

/* Variables and methods for RIL_REQUEST_DEVICE_IDENTITY support */
static char imei[16];
static char imeisv[17];
static bool gotIMEI = false;
static bool gotIMEISV = false;
static bool inIMEIRequest = false;
static bool inIMEISVRequest = false;

static void onRequestDeviceIdentity(int request, void *data, size_t datalen, RIL_Token t);
static void onRequestCompleteDeviceIdentity(RIL_Token t, RIL_Errno e);


/* Response data for RIL_REQUEST_GET_CELL_INFO_LIST */
static RIL_CellInfo_v12 cellInfoWCDMA;
static RIL_CellInfo_v12 cellInfoGSM;
static RIL_CellInfo_v12 cellInfoList[2];

static void onRequestDial(int request, void *data, RIL_Token t) {
	RIL_Dial dial;
	RIL_UUS_Info uusInfo;

	dial.address = ((RIL_Dial *) data)->address;
	dial.clir = ((RIL_Dial *) data)->clir;
	dial.uusInfo = ((RIL_Dial *) data)->uusInfo;

	if (dial.uusInfo == NULL) {
		uusInfo.uusType = (RIL_UUS_Type) 0;
		uusInfo.uusDcs = (RIL_UUS_DCS) 0;
		uusInfo.uusData = NULL;
		uusInfo.uusLength = 0;
		dial.uusInfo = &uusInfo;
	}

	origRilFunctions->onRequest(request, &dial, sizeof(dial), t);
}


static int
decodeVoiceRadioTechnology (RIL_RadioState radioState) {
    switch (radioState) {
        case RADIO_STATE_SIM_NOT_READY:
        case RADIO_STATE_SIM_LOCKED_OR_ABSENT:
        case RADIO_STATE_SIM_READY:
            return RADIO_TECH_UMTS;

        case RADIO_STATE_RUIM_NOT_READY:
        case RADIO_STATE_RUIM_READY:
        case RADIO_STATE_RUIM_LOCKED_OR_ABSENT:
        case RADIO_STATE_NV_NOT_READY:
        case RADIO_STATE_NV_READY:
            return RADIO_TECH_1xRTT;

        default:
            RLOGD("decodeVoiceRadioTechnology: Invoked with incorrect RadioState");
            return -1;
    }
}

static void OnRequestGetCellInfoList(int request, void *data, size_t datalen, RIL_Token t) {
	RLOGI("%s: got request %s (data:%p datalen:%d)\n", __FUNCTION__,
		requestToString(request),
		data, datalen);

	cellInfoWCDMA.cellInfoType = RIL_CELL_INFO_TYPE_WCDMA;
	cellInfoWCDMA.CellInfo.wcdma.cellIdentityWcdma.mcc = -1;
	cellInfoWCDMA.CellInfo.wcdma.cellIdentityWcdma.mnc = -1;
	cellInfoWCDMA.CellInfo.wcdma.cellIdentityWcdma.psc = -1;

	cellInfoGSM.cellInfoType = RIL_CELL_INFO_TYPE_GSM;
	cellInfoGSM.CellInfo.gsm.cellIdentityGsm.mcc = -1;
	cellInfoGSM.CellInfo.gsm.cellIdentityGsm.mnc = -1;

	if (cellInfoGSM.CellInfo.gsm.cellIdentityGsm.lac > -1 &&
	    cellInfoGSM.CellInfo.gsm.cellIdentityGsm.cid > -1) {
		cellInfoList[0] = cellInfoGSM;
		cellInfoList[1] = cellInfoWCDMA;
		rilEnv->OnRequestComplete(t, RIL_E_SUCCESS, &cellInfoList, sizeof(cellInfoList));
	} else {
		rilEnv->OnRequestComplete(t, RIL_E_SUCCESS, &cellInfoWCDMA, sizeof(cellInfoWCDMA));
	}
}

static void onRequestVoiceRadioTech(int request, void *data, size_t datalen, RIL_Token t) {
	RLOGI("%s: got request %s (data:%p datalen:%d)\n", __FUNCTION__,
		requestToString(request),
		data, datalen);
        RIL_RadioState radioState = origRilFunctions->onStateRequest();

	voiceRadioTechnology = decodeVoiceRadioTechnology(radioState);
	if (voiceRadioTechnology < 0) {
		rilEnv->OnRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
		return;
	}
	rilEnv->OnRequestComplete(t, RIL_E_SUCCESS, &voiceRadioTechnology, sizeof(voiceRadioTechnology));
}

static int
decodeCdmaSubscriptionSource (RIL_RadioState radioState) {
    switch (radioState) {
        case RADIO_STATE_SIM_NOT_READY:
        case RADIO_STATE_SIM_LOCKED_OR_ABSENT:
        case RADIO_STATE_SIM_READY:
        case RADIO_STATE_RUIM_NOT_READY:
        case RADIO_STATE_RUIM_READY:
        case RADIO_STATE_RUIM_LOCKED_OR_ABSENT:
            return CDMA_SUBSCRIPTION_SOURCE_RUIM_SIM;

        case RADIO_STATE_NV_NOT_READY:
        case RADIO_STATE_NV_READY:
            return CDMA_SUBSCRIPTION_SOURCE_NV;

        default:
            RLOGD("decodeCdmaSubscriptionSource: Invoked with incorrect RadioState");
            return -1;
    }
}

static void onRequestCdmaGetSubscriptionSource(int request, void *data, size_t datalen, RIL_Token t) {
	RLOGI("%s: got request %s (data:%p datalen:%d)\n", __FUNCTION__,
		requestToString(request),
		data, datalen);
        RIL_RadioState radioState = (RIL_RadioState)origRilFunctions->onStateRequest();

	cdmaSubscriptionSource = decodeCdmaSubscriptionSource(radioState);
	if (cdmaSubscriptionSource < 0) {
		rilEnv->OnRequestComplete(t, RIL_E_GENERIC_FAILURE, NULL, 0);
		return;
	}
	rilEnv->OnRequestComplete(t, RIL_E_SUCCESS, &cdmaSubscriptionSource, sizeof(cdmaSubscriptionSource));
}

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


static bool is3gpp2(int radioTech) {
    switch (radioTech) {
        case RADIO_TECH_IS95A:
        case RADIO_TECH_IS95B:
        case RADIO_TECH_1xRTT:
        case RADIO_TECH_EVDO_0:
        case RADIO_TECH_EVDO_A:
        case RADIO_TECH_EVDO_B:
        case RADIO_TECH_EHRPD:
            return true;
        default:
            return false;
    }
}

static int
decodeSimStatus (RIL_RadioState radioState) {
   switch (radioState) {
       case RADIO_STATE_SIM_NOT_READY:
       case RADIO_STATE_RUIM_NOT_READY:
       case RADIO_STATE_NV_NOT_READY:
       case RADIO_STATE_NV_READY:
           return -1;
       case RADIO_STATE_SIM_LOCKED_OR_ABSENT:
       case RADIO_STATE_SIM_READY:
       case RADIO_STATE_RUIM_READY:
       case RADIO_STATE_RUIM_LOCKED_OR_ABSENT:
           return radioState;
       default:
           RLOGD("decodeSimStatus: Invoked with incorrect RadioState");
           return -1;
   }
}

static RIL_RadioState
processRadioState(RIL_RadioState newRadioState) {
    if((newRadioState > RADIO_STATE_UNAVAILABLE) && (newRadioState < RADIO_STATE_ON)) {
        int newVoiceRadioTech;
        int newCdmaSubscriptionSource;
        int newSimStatus;

        /* This is old RIL. Decode Subscription source and Voice Radio Technology
           from Radio State and send change notifications if there has been a change */
        newVoiceRadioTech = decodeVoiceRadioTechnology(newRadioState);
        if(newVoiceRadioTech != voiceRadioTechnology) {
            voiceRadioTechnology = newVoiceRadioTech;
            rilEnv->OnUnsolicitedResponse(RIL_UNSOL_VOICE_RADIO_TECH_CHANGED,
                &voiceRadioTechnology, sizeof(voiceRadioTechnology));
        }
        if(is3gpp2(newVoiceRadioTech)) {
            newCdmaSubscriptionSource = decodeCdmaSubscriptionSource(newRadioState);
            if(newCdmaSubscriptionSource != cdmaSubscriptionSource) {
                cdmaSubscriptionSource = newCdmaSubscriptionSource;
                rilEnv->OnUnsolicitedResponse(RIL_UNSOL_CDMA_SUBSCRIPTION_SOURCE_CHANGED,
                        &cdmaSubscriptionSource, sizeof(cdmaSubscriptionSource));
            }
        }
        newSimStatus = decodeSimStatus(newRadioState);
        if(newSimStatus != simRuimStatus) {
            simRuimStatus = newSimStatus;
            rilEnv->OnUnsolicitedResponse(RIL_UNSOL_RESPONSE_SIM_STATUS_CHANGED, NULL, 0);
        }

        /* Send RADIO_ON to telephony */
        newRadioState = RADIO_STATE_ON;
    }

    return newRadioState;
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

static RIL_RadioState onStateRequestShim() {
    RIL_RadioState radioState = RADIO_STATE_OFF;
    RIL_RadioState newRadioState = RADIO_STATE_OFF;

    radioState = origRilFunctions->onStateRequest();
    newRadioState = processRadioState(radioState);

    RLOGI("%s: RIL legacy radiostate converted from %d to %d\n", __FUNCTION__, radioState, newRadioState);
    return newRadioState;
}

static void onRequestShim(int request, void *data, size_t datalen, RIL_Token t)
{
	switch (request) {
                /* Our RIL doesn't support this, so we implement this ourself */
		case RIL_REQUEST_GET_CELL_INFO_LIST:
			OnRequestGetCellInfoList(request, data, datalen, t);
			RLOGI("%s: got request %s: replied with our implementation!\n", __FUNCTION__, requestToString(request));
			return;
			/* Our RIL doesn't support this, so we implement this ourself */
		case RIL_REQUEST_VOICE_RADIO_TECH:
			onRequestVoiceRadioTech(request, data, datalen, t);
			RLOGI("%s: got request %s: replied with our implementation!\n", __FUNCTION__, requestToString(request));
			return;
		/* Our RIL doesn't support this, so we implement this ourself */
		case RIL_REQUEST_CDMA_GET_SUBSCRIPTION_SOURCE:
			onRequestCdmaGetSubscriptionSource(request, data, datalen, t);
			RLOGI("%s: got request %s: replied with our implementation!\n", __FUNCTION__, requestToString(request));
			return;
		/* RIL_REQUEST_GET_IMEI is depricated */
		case RIL_REQUEST_DEVICE_IDENTITY:
			onRequestDeviceIdentity(request, data, datalen, t);
			RLOGI("%s: got request %s: replied with our implementation!\n", __FUNCTION__, requestToString(request));
			return;
		/* The Samsung RIL crashes if uusInfo is NULL... */
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
		case RIL_REQUEST_ALLOW_DATA:
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

static void onCompleteRequestGetSimStatus(RIL_Token t, RIL_Errno e, void *response) {
	/* While at it, upgrade the response to RIL_CardStatus_v6 */
	RIL_CardStatus_v5_samsung *p_cur = ((RIL_CardStatus_v5_samsung *) response);
	RIL_CardStatus_v6 v6response;

	v6response.card_state = p_cur->card_state;
	v6response.universal_pin_state = p_cur->universal_pin_state;
	v6response.gsm_umts_subscription_app_index = p_cur->gsm_umts_subscription_app_index;
	v6response.cdma_subscription_app_index = p_cur->cdma_subscription_app_index;
	v6response.ims_subscription_app_index = -1;
	v6response.num_applications = p_cur->num_applications;

	int i;
	for (i = 0; i < RIL_CARD_MAX_APPS; ++i)
		memcpy(&v6response.applications[i], &p_cur->applications[i], sizeof(RIL_AppStatus));

	/* Send the fixed response to libril */
	rilEnv->OnRequestComplete(t, e, &v6response, sizeof(RIL_CardStatus_v6));
}

static void onRequestCompleteVoiceRegistrationState(RIL_Token t, RIL_Errno e, void *response, size_t responselen) {
	char **resp = (char **) response;
        char radioTechUmts = '3';
	memset(voiceRegStateResponse, 0, VOICE_REGSTATE_SIZE);
	for (int index = 0; index < (int)responselen; index++) {
		voiceRegStateResponse[index] = resp[index];
		switch (index) {
			case 1: {
				cellInfoWCDMA.CellInfo.wcdma.cellIdentityWcdma.lac = atoi(voiceRegStateResponse[index]);
				break;
			}
			case 2: {
				cellInfoWCDMA.CellInfo.wcdma.cellIdentityWcdma.cid = atoi(voiceRegStateResponse[index]);
				break;
			}
			case 3:	{
			        // Add RADIO_TECH_UMTS because our RIL doesn't provide this here
				voiceRegStateResponse[index] = &radioTechUmts;
				break;
		        }
			default:
				break;
		}
	}
	rilEnv->OnRequestComplete(t, e, voiceRegStateResponse, VOICE_REGSTATE_SIZE);
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

static void fixupDataCallList(void *response, size_t responselen) {
	RIL_Data_Call_Response_v6 *p_cur = (RIL_Data_Call_Response_v6 *) response;
	int num = responselen / sizeof(RIL_Data_Call_Response_v6);

	int i;
	for (i = 0; i < num; ++i)
		p_cur[i].gateways = p_cur[i].addresses;
}

static void onCompleteQueryAvailableNetworks(RIL_Token t, RIL_Errno e, void *response, size_t responselen) {
	/* Response is a char **, pointing to an array of char *'s */
	size_t numStrings = responselen / sizeof(char *);
	size_t newResponseLen = (numStrings - (numStrings / 3)) * sizeof(char *);

	void *newResponse = malloc(newResponseLen);

	/* Remove every 5th and 6th strings (qan elements) */
	char **p_cur = (char **) response;
	char **p_new = (char **) newResponse;
	size_t i, j;
	for (i = 0, j = 0; i < numStrings; i += 6) {
		p_new[j++] = p_cur[i];
		p_new[j++] = p_cur[i + 1];
		p_new[j++] = p_cur[i + 2];
		p_new[j++] = p_cur[i + 3];
	}

	/* Send the fixed response to libril */
	rilEnv->OnRequestComplete(t, e, newResponse, newResponseLen);

	free(newResponse);
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

static void onRequestCompleteShim(RIL_Token t, RIL_Errno e, void *response, size_t responselen) {
	int request;
	RequestInfo *pRI;

	pRI = (RequestInfo *)t;

	/* If pRI is null, this entire function is useless. */
	if (pRI == NULL) {
		RLOGE("pRI is NULL!");
		goto null_token_exit;
	}

	/* If pCI is null, this entire function is useless. */
	if (pRI->pCI == NULL) {
		RLOGE("pRI->pCI is NULL");
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
		case RIL_REQUEST_VOICE_REGISTRATION_STATE:
			/* libsecril expects responselen of 60 (bytes) */
			/* numstrings (15 * sizeof(char *) = 60) */
			if (response != NULL && responselen < VOICE_REGSTATE_SIZE) {
				RLOGD("%s: got request %s and shimming response!\n", __FUNCTION__, requestToString(request));
				onRequestCompleteVoiceRegistrationState(t, e, response, responselen);
				return;
			}
			break;
		case RIL_REQUEST_GET_SIM_STATUS:
			/* Remove unused extra elements from RIL_AppStatus */
			if (response != NULL && responselen == sizeof(RIL_CardStatus_v5_samsung)) {
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
		case RIL_REQUEST_DATA_CALL_LIST:
		case RIL_REQUEST_SETUP_DATA_CALL:
			/* According to the Samsung RIL, the addresses are the gateways?
			 * This fixes mobile data. */
			if (response != NULL && responselen != 0 && (responselen % sizeof(RIL_Data_Call_Response_v6) == 0)) {
				RLOGD("%s: got request %s and shimming response!\n", __FUNCTION__, requestToString(request));
				fixupDataCallList(response, responselen);
				rilEnv->OnRequestComplete(t, e, response, responselen);
				return;
			}
			break;
		case RIL_REQUEST_QUERY_AVAILABLE_NETWORKS:
			/* Remove the extra (unused) elements from the operator info, freaking out the framework.
			 * Formerly, this is know as the mQANElements override. */
			if (response != NULL && responselen != 0 && (responselen % sizeof(char *) == 0)) {
				RLOGD("%s: got request %s and shimming response!\n", __FUNCTION__, requestToString(request));
				onCompleteQueryAvailableNetworks(t, e, response, responselen);
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
		case RIL_UNSOL_DATA_CALL_LIST_CHANGED:
			/* According to the Samsung RIL, the addresses are the gateways?
			 * This fixes mobile data. */
			if (data != NULL && datalen != 0 && (datalen % sizeof(RIL_Data_Call_Response_v6) == 0))
				fixupDataCallList((void*) data, datalen);
			break;
		case RIL_UNSOL_SIGNAL_STRENGTH:
			/* The Samsung RIL reports the signal strength in a strange way... */
			if (data != NULL && datalen >= sizeof(RIL_SignalStrength_v5))
				fixupSignalStrength((void*) data);
			break;
	}

	rilEnv->OnUnsolicitedResponse(unsolResponse, data, datalen);
}

static void patchMem(void *libHandle) {
	/*
	 * MAX_TIMEOUT is used for a call to pthread_cond_timedwait_relative_np.
	 * The issue is bionic has switched to using absolute timeouts instead of
	 * relative timeouts, and a maximum time value can cause an overflow in
	 * the function converting relative to absolute timespecs if unpatched.
	 *
	 * By patching this to 0x01FFFFFF from 0x7FFFFFFF, the timeout should
	 * expire in about a year rather than 68 years, and the RIL should be good
	 * up until the year 2036 or so.
	 */
	uint32_t *MAX_TIMEOUT;

	MAX_TIMEOUT = (uint32_t *)dlsym(libHandle, "MAX_TIMEOUT");
	if (CC_UNLIKELY(!MAX_TIMEOUT)) {
		RLOGE("%s: MAX_TIMEOUT could not be found!", __FUNCTION__);
		return;
	}
	RLOGD("%s: MAX_TIMEOUT found at %p!", __FUNCTION__, MAX_TIMEOUT);
	RLOGD("%s: MAX_TIMEOUT is currently 0x%" PRIX32, __FUNCTION__, *MAX_TIMEOUT);
	if (CC_LIKELY(*MAX_TIMEOUT == 0x7FFFFFFF)) {
		*MAX_TIMEOUT = 0x01FFFFFF;
		RLOGI("%s: MAX_TIMEOUT was changed to 0x0%" PRIX32, __FUNCTION__, *MAX_TIMEOUT);
	} else {
		RLOGW("%s: MAX_TIMEOUT was not 0x7FFFFFFF; leaving alone", __FUNCTION__);
	}

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

	// Fix RIL issues by patching memory
	patchMem(origRil);

	//remove "-c" command line as Samsung's RIL does not understand it - it just barfs instead
	RLOGD("Ditching -c command...");
	for (int i = 0; i < argc; i++) {
		if (!strcmp(argv[i], "-c") && i != argc -1) {	//found it
			memcpy(argv + i, argv + i + 2, sizeof(char*[argc - i - 2]));
			argc -= 2;
		}
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
	shimmedFunctions.onStateRequest = onStateRequestShim;

	RLOGD("RIL_Init completed");
	return &shimmedFunctions;

fail_after_dlopen:
	dlclose(origRil);
	return NULL;
}
