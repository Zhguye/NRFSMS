#include <stdio.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <modem/sms.h>
#include <modem/lte_lc.h>
#include <modem/modem_info.h>
#include <modem/nrf_modem_lib.h>
#include <stdbool.h>
#include "connect_example.h"
#include <string.h>
#include <nrf_modem_at.h> 
#include <zephyr/sys/printk.h>




LOG_MODULE_REGISTER(sms_1, LOG_LEVEL_DBG);

char payload343[255];


static void sms_callback(struct sms_data *const data, void *context)
{
	if (data == NULL) {
		printk("%s with NULL data\n", __func__);
		return;
	}

	if (data->type == SMS_TYPE_DELIVER) {
		/* When SMS message is received, print information */
		struct sms_deliver_header *header = &data->header.deliver;

		printk("\nSMS received:\n");
		printk("\tTime:   %02d-%02d-%02d %02d:%02d:%02d\n",
			header->time.year,
			header->time.month,
			header->time.day,
			header->time.hour,
			header->time.minute,
			header->time.second);

		printk("\tText:   '%s'\n", data->payload);
		memset(payload343, 0, sizeof(payload343));
		memcpy(payload343, data->payload, data->payload_len);
		printk("\t payload343:%s", payload343);
		printk("\tLength: %d\n", data->payload_len);

		if (header->app_port.present) {
			printk("\tApplication port addressing scheme: dest_port=%d, src_port=%d\n",
				header->app_port.dest_port,
				header->app_port.src_port);
		}
		if (header->concatenated.present) {
			printk("\tConcatenated short message: ref_number=%d, msg %d/%d\n",
				header->concatenated.ref_number,
				header->concatenated.seq_number,
				header->concatenated.total_msgs);
		}
	} else if (data->type == SMS_TYPE_STATUS_REPORT) {
		printk("SMS status report received\n");
	} else {
		printk("SMS protocol message with unknown type received\n");
	}

	
}




 //char networkdata[];
 //char voltagedata[];
 //char temperaturedata[];
 char response_data[422];
 


		
void respond_with_modem_data() {
   
   

	char cell_id[128];
	char temperature[128];
	char rsrp[128];
	
 modem_info_string_get(MODEM_INFO_CELLID, cell_id, sizeof(cell_id));
 modem_info_string_get(MODEM_INFO_TEMP, temperature, sizeof(temperature));
 modem_info_string_get(MODEM_INFO_RSRP, rsrp, sizeof(rsrp));

	/* Format sensor data into the response buffer, 
	I tried different ideas to add all the responses into 1 string and snprintf seemed easier than sprintf, however did not test possible errors with different print fuctions*/
    snprintf(response_data, sizeof(response_data),
             "Temperature: %s  C, RSRP: %s dBm, Cell ID: %s \n",
             temperature, rsrp, cell_id);
    // Log the response data
    printk(" data in respond function:%s", response_data);
}

	//Main section of the code, what happens when code device receives SMS and what it decides to return.
void respond_to_payload() {
	int ret =0;
	printk(" Respond in payload function : %s\n",payload343);
    if (strcmp(payload343, "network") == 0) {
		 respond_with_modem_data();
		ret=sms_send_text(CONFIG_SMS_SEND_PHONE_NUMBER,response_data);
		printk("SMS sent to Onomondo\n");
			if (ret) {
				printk("Sending SMS failed with error: %d\n", ret);
				}
    } 
			if(strcmp(payload343, "stop") == 0){
				k_msleep(1000);
			}
				/*	Other Options, Unused		
		else if (strcmp(payload343, "voltage") == 0) {
        	printf("Response: %s\n", voltagedata);
    } 	else if (strcmp(payload343, "temp") == 0) {
        	printf("Response: %s\n", temperaturedata);
    } 	*/else {
        	printf("Error request, option not found. Try network, voltage, or temp \n");
    }
}





int main(void)
{
	int32_t retval = 0;
	int ret = 0;
	int handle = 0;
	//int err;
	struct modem_param_info modem_info;


	
	
	
  	modem_info_init();
    

    /* Get modem information */
    modem_info_params_init(&modem_info);
   
		

		//FW_VERSION is defined in CMakeLists.txt
	LOG_INF("Started SMS base application version %s", "3.20.0");
	retval = nrf_modem_lib_init();
	if(retval != 0){
		LOG_ERR("Failed to initialize nrf modem lib (%d)",retval);
		k_msleep(5000);
		return -1;
	}

	retval = connect_to_network();
	if(retval != 1){
		LOG_DBG("Connecting failed with %d",retval);
	}


	handle = sms_register_listener(sms_callback, NULL);
	if (handle) {
		printk("sms_register_listener returned err: %d\n", handle);
		return 0;
	}

	printk("SMS sample is ready for receiving messages\n");

	while(1){
		respond_to_payload();
		k_msleep(10000);
		

		};
		return 0;
	}
