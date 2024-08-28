#include <stdint.h>
#include <stdlib.h>
#include "connect_example.h"
#include <nrf_modem_at.h>
#include <modem/at_monitor.h>
#include <zephyr/logging/log.h>

/*Register logging for the module*/
LOG_MODULE_REGISTER(connect_example, LOG_LEVEL_DBG);

/* Network registration status semaphore */
static K_SEM_DEFINE(scan_status, 0, 1);

static int connected = 0;
/* AT monitor for +CEREG notifications */
AT_MONITOR(network_registration, "+CEREG", cereg_mon);
static void cereg_mon(const char *notif)
{
    LOG_INF("%s",notif);
    int32_t cereg_status;
    cereg_status = atoi(notif + strlen("+CEREG: "));
    if (cereg_status == 5 || cereg_status == 1) { //1=home network, 5 = roaming
        connected = 1;
    	k_sem_give(&scan_status);
	}
    if (cereg_status == 90) { //Sim failure
        connected = 0;
    	k_sem_give(&scan_status);
	}
}


static int32_t cfun_control(int mode)
{
    int32_t err;
    LOG_INF("Setting modem functional mode to %d",mode);
    err = nrf_modem_at_printf("AT+CFUN=%d", mode);
    if (err == 0) {
        LOG_INF("ok");
    } else if (err < 0) {
        LOG_INF("Error %d",errno);
    } else {
        /* Command was sent, but response is not "OK" */
        switch(nrf_modem_at_err_type(err)) {
        case NRF_MODEM_AT_ERROR:
            /* Modem returned "ERROR" */
            LOG_ERR("error");
            break;
        case NRF_MODEM_AT_CME_ERROR:
            /* Modem returned "+CME ERROR" */
            LOG_ERR("cme error: %d", nrf_modem_at_err(err));
            break;
        case NRF_MODEM_AT_CMS_ERROR:
            /* Modem returned "+CMS ERROR" */
            LOG_ERR("cms error: %d", nrf_modem_at_err(err));
            break;
        }
    }
    return err;
}

int32_t sim_activation_status()
{
    int32_t status;
    //Polling way to check state
    LOG_INF("Waiting for SIM activation");
    for(int i=0;i<60;i++){
        nrf_modem_at_scanf("AT%XSIM?", "%%XSIM: %d", &status);
        if(status == 1)break;
        k_msleep(1000);
    }
    return status;
}

int32_t connect_to_network()
{
    int32_t retval;
    int32_t sim_status = 0;
    char resp[256];
    char resp1[256];
    
    
    //Read modem firmware version
    retval = nrf_modem_at_scanf("AT+CGMR","%s",resp);
    if(retval == 1){
        LOG_INF("Modem fw version: %s",resp);
    }

    //Read modem firmware version
    retval = nrf_modem_at_scanf("AT+CPIN","%s",resp1);
    if(retval == 1){
        LOG_INF("PIN attempts %s",resp1);
    }

    //Register to cereg notifications
    retval = nrf_modem_at_printf("AT+CEREG=%d",CEREG_NOTIFICATION_LEVEL);
	if (retval) {
		LOG_INF("AT+CEREG failed");
		return -1;
	}

    //Set device to functional mode 1 (Full opeartion) Enabled services depend on AT%XSYSTEMMODE settings.
    retval = cfun_control(MODEM_FUNC_MODE_FULL);
    if(retval < 0){
        LOG_ERR("Connect failed");
        return -1;
    }

    sim_status = sim_activation_status();

    if(sim_status == 1){
        LOG_INF("SIM Activated, reading ICCID");
        retval = nrf_modem_at_cmd(resp, sizeof(resp), "AT%%XICCID");
        if (!retval) {
            LOG_INF("ICCID: %s",resp);
        }
    }

    //Wait until connected or timeout
    LOG_INF("Waiting for network");
	retval = k_sem_take(&scan_status, K_SECONDS(60*10));

    if(!connected || retval != 0){
        LOG_INF("Connect attempt failed, transition to flight mode");
        cfun_control(MODEM_FUNC_MODE_FLIGHT);
        return 0;
    }

    LOG_INF("Connected");
     

    
	

    return 1;
}