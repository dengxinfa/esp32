#include <errno.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_log.h"
#include "esp_err.h"
#include "nvs_flash.h"

#include "my_tcp_perf.h"

EventBits_t deng_EVENT_bits = 0;


//this task establish a TCP connection and receive data from TCP
static void tcp_conn(void *pvParameters)
{
    while (1) {

        g_rxtx_need_restart = false;

        ESP_LOGI(TAG, "task tcp_conn.");

        /*wating for connecting to AP*/
        xEventGroupWaitBits(tcp_event_group, WIFI_CONNECTED_BIT, false, true, portMAX_DELAY);

        ESP_LOGI(TAG, "sta has connected to ap.");

        int socket_ret = ESP_FAIL;

        TaskHandle_t tx_rx_task = NULL;

#if EXAMPLE_ESP_TCP_MODE_SERVER
        if (socket_ret == ESP_FAIL) {
            /*create tcp socket*/
            ESP_LOGI(TAG, "tcp_server will start after 3s...");
            vTaskDelay(3000 / portTICK_RATE_MS);
            ESP_LOGI(TAG, "create_tcp_server.");
            socket_ret = create_tcp_server();
        }
#else /*EXAMPLE_ESP_TCP_MODE_SERVER*/
        if (socket_ret == ESP_FAIL) {
            ESP_LOGI(TAG, "tcp_client will start after 20s...");
            vTaskDelay(20000 / portTICK_RATE_MS);
            ESP_LOGI(TAG, "create_tcp_client.");
            socket_ret = create_tcp_client();
        }
#endif
        if (socket_ret == ESP_FAIL) {
            ESP_LOGI(TAG, "create tcp socket error,stop.");
            continue;
        }

        /*create a task to tx/rx data*/

#if EXAMPLE_ESP_TCP_PERF_TX
        if (tx_rx_task == NULL) {
            if (pdPASS != xTaskCreate(&send_data, "send_data", 4096, NULL, 4, &tx_rx_task)) {
                ESP_LOGE(TAG, "Send task create fail!");
            }
        }
#else /*EXAMPLE_ESP_TCP_PERF_TX*/
        if (tx_rx_task == NULL) {
			Recv_Event_Handle = xEventGroupCreate();
	 			if(Recv_Event_Handle == NULL)
					ESP_LOGI(TAG,"creat event group error");
            if (pdPASS != xTaskCreate(&recv_data, "recv_data", 4096, NULL, 4, &tx_rx_task)) {
                ESP_LOGE(TAG, "Recv task create fail!");
				vEventGroupDelete(Recv_Event_Handle);
            }
			
        }
#endif

        while (1) {
			#if EXAMPLE_ESP_TCP_PERF_TX
			#else /*EXAMPLE_ESP_TCP_PERF_TX*/
				deng_EVENT_bits = xEventGroupWaitBits(Recv_Event_Handle,LED_TURN_ON | LED_TURN_OFF, pdTRUE, pdFALSE, portMAX_DELAY);
				if(LED_TURN_ON == (deng_EVENT_bits & LED_TURN_ON))
				ESP_LOGI(TAG, "receive the LED_TURN_ON event");
				if(LED_TURN_OFF == (deng_EVENT_bits & LED_TURN_OFF))
				ESP_LOGI(TAG, "receive the LED_TURN_OFF event");
				deng_EVENT_bits = 0;
			#endif
				ESP_LOGI(TAG, "check loop");
            	if (g_rxtx_need_restart) {
                	printf("send or receive task encoutner error, need to restart\n");
					vEventGroupDelete(Recv_Event_Handle);
                	break;
            }

        }

        close_socket();
    }

    vTaskDelete(NULL);
}

void app_main(void)
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK( ret );

#if EXAMPLE_ESP_WIFI_MODE_AP
    ESP_LOGI(TAG, "EXAMPLE_ESP_WIFI_MODE_AP");
    wifi_init_softap();
#else
    ESP_LOGI(TAG, "ESP_WIFI_MODE_STA");
    wifi_init_sta();
#endif /*EXAMPLE_ESP_WIFI_MODE_AP*/
    xTaskCreate(&tcp_conn, "tcp_conn", 4096, NULL, 5, NULL);
}

