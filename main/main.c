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
#include "temp_sensor.h"



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

    connect_wifi();

    if (wifi_connect_status)
    {
        err = init_camera();
        if (err != ESP_OK)
        {
            ESP_LOGE(TAG, "Camera init failed: %s", esp_err_to_name(err));
            return;
        }

        setup_server();
        ESP_LOGI(TAG, "ESP32 CAM Web Server is up and running");
    }
    else
    {
        ESP_LOGI(TAG, "Failed to connect to Wi-Fi. Check credentials.");
    }

    if (init_temp_sensor() == ESP_OK)
    {
        float temp = get_temperature();
        if (temp > -100)  // basic sanity check
        {
            ESP_LOGI(TAG, "Measured temperature: %.2f°C", temp);
        }
        else
        {
            ESP_LOGW(TAG, "Temperature sensor returned invalid value");
        }
    }
    else
    {
        ESP_LOGE(TAG, "Temperature sensor init failed");
    }
}
