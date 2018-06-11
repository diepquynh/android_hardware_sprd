#ifndef __MODEMD_H__

#define __MODEMD_H__

#define MODEMD_DEBUG

#ifdef MODEMD_DEBUG
#define MODEMD_LOGD(x...) ALOGD( x )
#define MODEMD_LOGE(x...) ALOGE( x )
#else
#define MODEMD_LOGD(x...) do {} while(0)
#define MODEMD_LOGE(x...) do {} while(0)
#endif

#define TD_MODEM 0x3434
#define W_MODEM  0x5656
//#define WCN_MODEM  0x7878

#define MODEM_READY 0
#define MODEM_ASSERT 1
#define MODEM_RESET 2

/* indicate which modem is enable */
#define TD_MODEM_ENABLE	"persist.modem.t.enable"
#define W_MODEM_ENABLE	"persist.modem.w.enable"
#define WCN_MODEM_ENABLE	"ro.modem.wcn.enable"

/* sim card num property */
#define TD_SIM_NUM	"ro.modem.t.count"
#define W_SIM_NUM	"ro.modem.w.count"

/* stty interface property */
#define TD_TTY_DEV_PRO "ro.modem.t.tty"
#define W_TTY_DEV_PRO "ro.modem.w.tty"

/* cproc driver device node */
#define TD_PROC_PRO	"ro.modem.t.dev"
#define W_PROC_PRO	"ro.modem.w.dev"
#define WCN_PROC_PRO	"ro.modem.wcn.dev"
/* default value for cproc driver device node */
#define TD_PROC_DEV	"/dev/cpt"
#define W_PROC_DEV	"/dev/cpw"
//#define WCN_PROC_DEV	"/dev/cpwcn"

/* load modem image interface */
#define TD_MODEM_BANK	"/proc/cpt/modem"
#define TD_DSP_BANK	"/proc/cpt/dsp"
#define TD_MODEM_START	"/proc/cpt/start"
#define TD_MODEM_STOP	"/proc/cpt/stop"
#define W_MODEM_BANK	"/proc/cpw/modem"
#define W_DSP_BANK	"/proc/cpw/dsp"
#define W_MODEM_START	"/proc/cpw/start"
#define W_MODEM_STOP	"/proc/cpw/stop"
//#define WCN_MODEM_BANK	"/proc/cpwcn/modem"
//#define WCN_MODEM_START	"/proc/cpwcn/start"
//#define WCN_MODEM_STOP	"/proc/cpwcn/stop"

/* modem/dsp partition */
#define TD_MODEM_SIZE	(10*1024*1024)
#define TD_DSP_SIZE	(5*1024*1024)
#define W_MODEM_SIZE	(10*1024*1024)
#define W_DSP_SIZE	(5*1024*1024)
//#define WCN_MODEM_SIZE	(10*1024*1024)

/* detect assert/hangup interface */
#define TD_ASSERT_PRO	"ro.modem.t.assert"
#define W_ASSERT_PRO	"ro.modem.w.assert"
#define WCN_ASSERT_PRO	"ro.modem.wcn.assert"
#define TD_LOOP_PRO		"ro.modem.t.loop"
#define W_LOOP_PRO		"ro.modem.w.loop"
/* default value for detect assert/hangup interface */
#define TD_ASSERT_DEV	"/dev/spipe_td2"
#define W_ASSERT_DEV	"/dev/spipe_w2"
#define WCN_ASSERT_DEV	"/dev/spipe_wcn2"
#define TD_WATCHDOG_DEV	"/proc/cpt/wdtirq"
#define W_WATCHDOG_DEV	"/proc/cpw/wdtirq"
#define WCN_WATCHDOG_DEV	"/proc/cpwcn/wdtirq"
#define TD_LOOP_DEV		"/dev/spipe_td0"
#define W_LOOP_DEV		"/dev/spipe_w0"

#define PHONE_APP		"com.android.phone"
#define PROP_TTYDEV		"persist.ttydev"
#define PHONE_APP_PROP		"sys.phone.app"
#define MODEMRESET_PROPERTY	"persist.sys.sprd.modemreset"

#define min(A,B)       (((A) < (B)) ? (A) : (B))

#define MODEM_SOCKET_NAME	"modemd"
#define PHSTD_SOCKET_NAME	"phstd"
#define PHSW_SOCKET_NAME	"phsw"
#define MODEM_ENGCTRL_NAME "modemd_engpc"
#define MODEM_ENGCTRL_PRO  "persist.sys.engpc.disable"

#define ENGPC_REQUSETY_OPEN "0"
#define ENGPC_REQUSETY_CLOSE "1"
#define MAX_CLIENT_NUM	10
extern int  client_fd[MAX_CLIENT_NUM];

int stop_service(int modem, int is_vlx);
int start_service(int modem, int is_vlx, int restart);

int write_proc_file(char *file, int offset, char *string);
int vlx_reboot_init(void);
int detect_vlx_modem(int modem);

void* detect_sipc_modem(void *param);
void *detect_modem_blocked(void *par);

#endif
