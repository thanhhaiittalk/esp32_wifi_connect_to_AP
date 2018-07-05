#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_event_loop.h"
#include "nvs_flash.h"
#include "sdkconfig.h"

esp_err_t event_handler(void *ctx, system_event_t *event)
{
	if (event->event_id == SYSTEM_EVENT_SCAN_DONE) {
		printf("Number of access points found: %d\n",
				event->event_info.scan_done.number);
		uint16_t apCount = event->event_info.scan_done.number;
		if (apCount == 0) {
			return ESP_OK;
		}
		wifi_ap_record_t *list = (wifi_ap_record_t *)malloc(sizeof(wifi_ap_record_t) * apCount);
		ESP_ERROR_CHECK(esp_wifi_scan_get_ap_records(&apCount, list));
		int i;
		for (i=0; i<apCount; i++) {
			char *authmode;
			//method of securing your network
			switch(list[i].authmode) {
				case WIFI_AUTH_OPEN:
					authmode = "WIFI_AUTH_OPEN";
					break;
				case WIFI_AUTH_WEP:
					authmode = "WIFI_AUTH_WEP";
					break;
				case WIFI_AUTH_WPA_PSK:
					authmode = "WIFI_AUTH_WPA_PSK";
					break;
				case WIFI_AUTH_WPA2_PSK:
					authmode = "WIFI_AUTH_WPA2_PSK";
					break;
				case WIFI_AUTH_WPA_WPA2_PSK:
					authmode = "WIFI_AUTH_WPA_WPA2_PSK";
					break;
				default:
					authmode = "Unknown";
					break;
			}
			printf("ssid=%s, rssi=%d, authmode=%s\n",
					list[i].ssid, list[i].rssi, authmode);
		}
		free(list);
	}
	if (event->event_id == SYSTEM_EVENT_STA_GOT_IP) {
		printf("Our IP address is " IPSTR "\n",
				IP2STR(&event->event_info.got_ip.ip_info.ip));
		printf("We have now connected to a station and can do things...\n");
	}
	return ESP_OK;
}

int app_main(void)
{
	nvs_flash_init();
	tcpip_adapter_init();
	ESP_ERROR_CHECK(esp_event_loop_init(event_handler, NULL));
	wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
	ESP_ERROR_CHECK(esp_wifi_init(&cfg));
	ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
	ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
	ESP_ERROR_CHECK(esp_wifi_start());
	// Let us test a WiFi scan ...
	wifi_scan_config_t scanConf = {
			.ssid = NULL,
			.bssid = NULL,
			.channel = 0,
			.show_hidden = 1
	};
	ESP_ERROR_CHECK(esp_wifi_scan_start(&scanConf, 0));
	wifi_config_t sta_config = {
			.sta = {
					.ssid = "V20_7435",
					.password = "84dda7591ee4",
					.bssid_set = 0
			},
		};
	ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &sta_config));
	ESP_ERROR_CHECK(esp_wifi_start());
	ESP_ERROR_CHECK(esp_wifi_connect());
	return 0;
}
