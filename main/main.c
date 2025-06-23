#include <esp_system.h>
#include <nvs_flash.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"
#include "esp_camera.h"

#include "connect_wifi.h"
#include "camera_pins.h"
#include "web_server.h"
#include "camera_init.h"


static const char *TAG = "main";

void app_main(void)
{
    esp_err_t err;

    // Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }

    connect_wifi();  // this sets up ESP32 as AP

    // No need to check wifi_connect_status — AP always starts unless there's an error
    ESP_LOGI(TAG, "Wi-Fi AP is running. You can now connect to ESP32-CAM.");

    err = init_camera();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Camera init failed: %s", esp_err_to_name(err));
        return;
    }

    setup_server();
    ESP_LOGI(TAG, "ESP32 CAM Web Server is up and running");
}
