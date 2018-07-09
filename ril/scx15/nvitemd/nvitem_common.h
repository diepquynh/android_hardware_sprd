
#ifdef WIN32
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#define NVITEM_PRINT printf
#else
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <pthread.h>
//#include "engapi.h"
//#include "engat.h"

#define LOG_TAG "NVITEM"
//#define LOG_NDEBUG 0
#include <cutils/log.h>
#define NVITEM_PRINT ALOGD
#endif

#ifndef _NVITEM_COMMON_H_
#define _NVITEM_COMMON_H_

typedef unsigned char		BOOLEAN;
typedef unsigned char 		uint8;
typedef unsigned short		uint16;
typedef unsigned  int		uint32;

typedef signed char		int8;
typedef signed short		int16;
typedef signed int			int32;

//-------------------------------------------------
//				Const config: can not be changed
//-------------------------------------------------
#define RAMNV_NUM					15		// max number of ramdisk, can not >= 15.
#define RAMNV_SECT_SIZE			512		// sect size of ramdisk

//-------------------------------------------------
//				Config: can be changed if nessarry
//-------------------------------------------------
#define RAMNV_DIRTYTABLE_MAXSIZE	32		// max sect number is (RAMNV_DIRTYTABLE_MAXSIZE << 5), 32 means 512k ramdisk


#endif

