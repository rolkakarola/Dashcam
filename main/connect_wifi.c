#include "connect_wifi.h"  
#include "esp_event.h"     // ESP32 event loop
#include "esp_log.h"       
#include "esp_netif.h"     // Network interface (Wi-Fi, Ethernet)
#include "esp_wifi.h"      // Wi-Fi functions
#include "nvs_flash.h"     
#include "lwip/ip4_addr.h" // IP address handling

static const char *TAG = "Connect_WiFi_AP"; // Log tag
int wifi_connect_status = 0;                // Tracks Wi-Fi status (unused here)
#define AP_MAX_CONNECTIONS 4                // Max devices that can connect

esp_err_t connect_wifi() {
    // ESP networking comp init with error checking
    ESP_ERROR_CHECK(esp_netif_init()); //TCP/IP stack
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    // Soft AP network interface creation
    esp_netif_t *ap_netif = esp_netif_create_default_wifi_ap();

    // Stop DHCP server before setting static IP
    ESP_ERROR_CHECK(esp_netif_dhcps_stop(ap_netif));

    // Set static IP info (10.0.0.1/255.255.255.0)
    esp_netif_ip_info_t ip_info; // struct holding IP config
    IP4_ADDR(&ip_info.ip, 10, 0, 0, 1);
    IP4_ADDR(&ip_info.gw, 10, 0, 0, 1);
    IP4_ADDR(&ip_info.netmask, 255, 255, 255, 0);
    ESP_ERROR_CHECK(esp_netif_set_ip_info(ap_netif, &ip_info)); // applies static IP

    // Restart DHCP server with the new IP config
    ESP_ERROR_CHECK(esp_netif_dhcps_start(ap_netif));

    // Intialize Wi-Fi driver
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    // Configure SoftAP SSID, Password
    wifi_config_t wifi_config = {
        .ap = {
            .ssid = AP_WIFI_SSID,                     // Set the AP name (SSID)
            .ssid_len = strlen(AP_WIFI_SSID),         // Set the SSID length
            .password = AP_WIFI_PASSWORD,             // Set the AP password
            .max_connection = AP_MAX_CONNECTIONS,     // Max number of clients that can connect
            .authmode = WIFI_AUTH_WPA_WPA2_PSK        // Use WPA/WPA2 authentication
        },
    };

    if (strlen(AP_WIFI_PASSWORD) == 0) { // password, if none = open network
        wifi_config.ap.authmode = WIFI_AUTH_OPEN;
    }

    // Start the Wi-Fi Access Point
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));   // acts as a hotspot
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config));  // applies config
    ESP_ERROR_CHECK(esp_wifi_start());   // statr boradcasting

    // Log AP Info and Return
    ESP_LOGI(TAG, "Wi-Fi AP started. SSID: %s, Password: %s, IP: 10.0.0.1", AP_WIFI_SSID, AP_WIFI_PASSWORD);

    return ESP_OK;  // Success
}
