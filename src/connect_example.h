#ifndef CONNECT_EXAMPLE_H
#define CONNECT_EXAMPLE_H
/*Module is intented to be a minimal example of using at- commands via nrf_modem_lib AT api*/

#define CEREG_NOTIFICATION_LEVEL    5
typedef enum MODEM_FUNCTIONAL_MODE_E
{
    MODEM_FUNC_MODE_ERROR               =   -1,
    MODEM_FUNC_MODE_POWEROFF            =   0,
    MODEM_FUNC_MODE_FULL                =   1,
    MODEM_FUNC_MODE_RECEIVE             =   2,
    MODEM_FUNC_MODE_FLIGHT              =   4,
    MODEM_FUNC_MODE_DEACTIVATE_LTE      =   20,
    MODEM_FUNC_MODE_ACTIVATE_LTE        =   21,
    MODEM_FUNC_MODE_DEACTIVATE_GNSS     =   30,
    MODEM_FUNC_MODE_ACTIVATE_GNSS       =   31,
    MODEM_FUNC_MODE_DEACTIVATE_UICC     =   40,
    MODEM_FUNC_MODE_ACTIVATE_UICC       =   41,
    MODEM_FUNC_MODE_FLIGHT_WITH_UICC    =   44
} MODEM_FUNCTIONAL_MODE;

/*@brief Connect to network.
@note If connection attempt fails, modem is set to flight mode
@return Returns -1 on error, 0 if connection was not established or 1 if connection was made*/
int32_t connect_to_network();

#endif