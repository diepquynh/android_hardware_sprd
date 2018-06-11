
#include "nvitem_common.h"
#include "nvitem_channel.h"
#include "nvitem_sync.h"
#include "nvitem_packet.h"
#include "nvitem_os.h"
#include "nvitem_buf.h"
#include "cutils/properties.h"

#define PRODUCT_PARTITION_PATH    "ro.product.partitionpath"
#define RO_MODEM_CHAR             "ro.modem."
#define RO_MODEM_FIXNVSIZE        ".fixnv_size"
#define RO_MODEM_RUNNVSIZE        ".runnv_size"
#define RO_MODEM_NV_PATH          ".nvp"
#define NV_TAIL                   ".nv"
/*note:
#define PROPERTY_KEY_MAX   32   //strlen(PRODUCT_PARTITION_PATH) will be letter than MAX
#define PROPERTY_VALUE_MAX 92
*/

static void *pSaveTask(void* ptr)
{
	do
	{
		waiteEvent();
		NVITEM_PRINT("pSaveTask up\n");
		saveToDisk();
	}while(1);
	return 0;
}

extern char channel_path[95];
extern char fixnv_ori_path[100];//length = _ramdiskCfg.image_path len
extern char fixnv_bak_path[100];
extern char runnv_ori_path[100];
extern char runnv_bak_path[100];
uint32 fixnv_size,runnv_size;

BOOLEAN is_cali_mode;

int main(int argc, char *argv[])
{
#ifndef WIN32
	pthread_t pTheadHandle;
#endif
	char fixnv[95],runnv[95];
	char   fixnv_property[50],runnv_property[50];
	char system_prop[20];
	char partition_path[100];
	char nvp[50];
	char path_char[95];
	char channel_char[50];
	uint32 test = 0;
	if(3 != argc)
	{
		NVITEM_PRINT("Usage:\n");
		NVITEM_PRINT("\tnvitemd channelPath configPath is_cali_mode\n");
		return 0;
	}
	//get system prop eg:ro.modem.t;ro.modem.w ....
	if(strlen(RO_MODEM_CHAR)+strlen(argv[1]) < 20){
		strcpy(system_prop,RO_MODEM_CHAR);
		strcat(system_prop,argv[1]);
	}
	NVITEM_PRINT("system_prop %s\n",system_prop);

	//get channel
	if(strlen(system_prop)+strlen(NV_TAIL) < 50){
		strcpy(channel_char,system_prop);
		strcat(channel_char,NV_TAIL);
	}
	NVITEM_PRINT("channel_char %s\n",channel_char);

	property_get(channel_char, channel_path, "");
	NVITEM_PRINT("channel_path %s \n",channel_path);

	//get nvsize
	if(strlen(system_prop) + strlen(RO_MODEM_FIXNVSIZE) < 50){
		strcpy(fixnv_property,system_prop);
		strcat(fixnv_property,RO_MODEM_FIXNVSIZE);
		property_get(fixnv_property,fixnv,"");
		NVITEM_PRINT("NVITEM: fixnv_property is %s fixnv %s\n",fixnv_property,fixnv);
	}

	if(strlen(system_prop) + strlen(RO_MODEM_RUNNVSIZE) < 50){
		strcpy(runnv_property,system_prop);
		strcat(runnv_property,RO_MODEM_RUNNVSIZE);
		property_get(runnv_property,runnv,"");
		NVITEM_PRINT("NVITEM: runnv_property is %s runnv %s\n",runnv_property,runnv);
	}
	fixnv_size = strtol(fixnv,0,16);
	runnv_size = strtol(runnv,0,16);

	NVITEM_PRINT("NVITEM fixnv_size %x,runnv_size %x\n",fixnv_size,runnv_size);

	//get nv path: eg: partition_path+td+fixnv
	property_get(PRODUCT_PARTITION_PATH, partition_path, "");
	NVITEM_PRINT("partition_path %s\n",partition_path);

	if(strlen(system_prop) + strlen(RO_MODEM_NV_PATH) < 50){
		strcpy(nvp,system_prop);
		strcat(nvp,RO_MODEM_NV_PATH);
		property_get(nvp,path_char,"");
		NVITEM_PRINT("NVITEM path_char %s\n",path_char);
	}

	if(100 > strlen(partition_path)+strlen(path_char)+strlen("FIXNV1")){
		strcpy(fixnv_ori_path,partition_path);
		strcat(fixnv_ori_path,path_char);
		strcat(fixnv_ori_path,"FIXNV1");

		strcpy(fixnv_bak_path,partition_path);
		strcat(fixnv_bak_path,path_char);
		strcat(fixnv_bak_path,"FIXNV2");
		NVITEM_PRINT("fixnv_ori_path %s fixnv_bak_path %s\n",fixnv_ori_path,fixnv_bak_path);
	}

	if(100 > strlen(partition_path)+strlen(path_char)+strlen("RUNTIMENV1")){
		strcpy(runnv_ori_path,partition_path);
		strcat(runnv_ori_path,path_char);
		strcat(runnv_ori_path,"RUNTIMENV1");

		strcpy(runnv_bak_path,partition_path);
		strcat(runnv_bak_path,path_char);
		strcat(runnv_bak_path,"RUNTIMENV2");
		NVITEM_PRINT("runnv_ori_path %s runnv_bak_path %s\n",runnv_ori_path,runnv_bak_path);
	}
	is_cali_mode = atoi(argv[2]);
    NVITEM_PRINT("is_cali_mode %d \n",is_cali_mode);
	initEvent();
	initBuf();

//---------------------------------------------------
#ifndef WIN32
// create another task
	pthread_create(&pTheadHandle, NULL, (void*)pSaveTask, NULL);
#endif
//---------------------------------------------------

	do
	{
		channel_open();
		NVITEM_PRINT("NVITEM:channel open\n");
		_initPacket();
		_syncInit();
		syncAnalyzer();
		NVITEM_PRINT("NVITEM:channel close\n");
		channel_close();
	}while(1);
	return 0;
}


