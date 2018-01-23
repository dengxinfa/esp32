#ifndef __MY_TCP_PERF_H__
#define __MY_TCP_PERF_H__



#ifdef __cplusplus
extern "C" {
#endif


/*Recv_Event*/
#define LED_TURN_ON  1<<0
#define LED_TURN_OFF 1<<1

/*test options*/
#define EXAMPLE_ESP_WIFI_MODE_AP 1 //TRUE:AP FALSE:STA
#define EXAMPLE_ESP_TCP_MODE_SERVER 1 //TRUE:server FALSE:client
#define EXAMPLE_ESP_TCP_PERF_TX 0 //TRUE:send FALSE:receive
#define EXAMPLE_ESP_TCP_DELAY_INFO 1 //TRUE:show delay time info

/*AP info and tcp_server info*/
#define EXAMPLE_DEFAULT_SSID "deng_tcp_server"
#define EXAMPLE_DEFAULT_PWD "12345678"
#define EXAMPLE_DEFAULT_PORT 8000
#define EXAMPLE_DEFAULT_PKTSIZE 1024
#define EXAMPLE_MAX_STA_CONN 1 //how many sta can be connected(AP mode)

#ifdef CONFIG_TCP_PERF_SERVER_IP
#define EXAMPLE_DEFAULT_SERVER_IP CONFIG_TCP_PERF_SERVER_IP
#else
#define EXAMPLE_DEFAULT_SERVER_IP "192.168.4.1"
#endif /*CONFIG_TCP_PERF_SERVER_IP*/



#define EXAMPLE_PACK_BYTE_IS 97 //'a'
#define TAG "tcp_perf:"

/* FreeRTOS event group to signal when we are connected to wifi*/
extern EventGroupHandle_t tcp_event_group;
#define WIFI_CONNECTED_BIT BIT0

extern int  g_total_data;
extern bool g_rxtx_need_restart;

#if EXAMPLE_ESP_TCP_PERF_TX && EXAMPLE_ESP_TCP_DELAY_INFO
extern int g_total_pack;
extern int g_send_success;
extern int g_send_fail;
extern int g_delay_classify[5];
#endif/*EXAMPLE_ESP_TCP_PERF_TX && EXAMPLE_ESP_TCP_DELAY_INFO*/

extern EventGroupHandle_t Recv_Event_Handle; //receive event group handle

/*tcp client command*/
extern EventBits_t deng_EVENT_bits;

typedef struct
	{
	char *deng_turn_off ;
	char *deng_turn_on  ;
	}MY_COMMAND;

int deng_strcmp(const char* str1, const char* str2);	


//using esp as station
void wifi_init_sta();
//using esp as softap
void wifi_init_softap();

//create a tcp server socket. return ESP_OK:success ESP_FAIL:error
esp_err_t create_tcp_server();
//create a tcp client socket. return ESP_OK:success ESP_FAIL:error
esp_err_t create_tcp_client();

//send data task
void send_data(void *pvParameters);
//receive data task
void recv_data(void *pvParameters);

//close all socket
void close_socket();

//get socket error code. return: error code
int get_socket_error_code(int socket);

//show socket error code. return: error code
int show_socket_error_reason(const char* str, int socket);

//check working socket
int check_working_socket();


#ifdef __cplusplus
}
#endif


#endif /*#ifndef __TCP_PERF_H__*/

