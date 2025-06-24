#include <esp_system.h>       // ESP32 system functions
#include <nvs_flash.h>         // Non-Volatile Storage (NVS) for storing settings
#include "freertos/FreeRTOS.h" // FreeRTOS (real-time OS)
#include "freertos/task.h"    // FreeRTOS task management
#include "esp_log.h"          // Logging functions
#include "esp_camera.h"       // ESP32-CAM camera functions

#include "connect_wifi.h"     // Custom Wi-Fi setup
#include "camera_pins.h"      // Defines camera pin configurations
#include "web_server.h"       // Custom web server setup
#include "camera_init.h"      // Custom camera initialization

// file-local const stre used for logging in ESP32 app
static const char *TAG = "MAIN"; 

void app_main(void)
{
    esp_err_t err; //ESP32 error type

    // Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        // if storage is full/corrupted, erase it and reinitialize
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }

    // wifi initialization
    if (connect_wifi() == ESP_OK) {
        ESP_LOGI(TAG, "\nWiFi AP running | SSID: %s\n", AP_WIFI_SSID );
    } else {
        ESP_LOGE(TAG, "Failed to start WiFi!");
        // Handle error (e.g., reboot)
    }

    // camera initialization
    err = init_camera();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Camera init failed: %s", esp_err_to_name(err));
        return;
    }

    // server initialization
    setup_server();
    ESP_LOGI(TAG, "ESP32 CAM Web Server is up and running");
}
