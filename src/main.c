#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "camera.h"
#include "wifi.h"
#include "web_server.h"

void app_main() {

    
    // Initialize components
    ESP_LOGI("MAIN", "System starting...");
    
    // 1. Initialize Camera
    if(init_camera() != ESP_OK) {
        ESP_LOGE("MAIN", "Camera init failed - check pins and power");
        while(1) { vTaskDelay(1000); } // Halt with error
    }
    
    ESP_LOGI("MAIN", "Camera ready");

    // 2. Connect to WiFi
    wifi_init();
    ESP_LOGI("MAIN", "WiFi initialized");
    
    // 3. Start Web Server
    httpd_handle_t server = start_webserver();
    if(!server) {
        ESP_LOGE("MAIN", "Web server failed!");
        vTaskDelay(portMAX_DELAY);
        return;
    }
    ESP_LOGI("MAIN", "Web server started");

    // Main loop just keeps the system running
    while(1) {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}