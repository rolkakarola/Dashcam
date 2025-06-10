#include "camera_config.h"
#include "esp_camera.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_psram.h"
#include "camera.h"

// OV2640 camera configuration for ESP32-CAM
static camera_config_t camera_config = {
    .pin_pwdn = 32,
    .pin_reset = -1,
    .pin_xclk = 0,
    .pin_sccb_sda = 26,
    .pin_sccb_scl = 27,
    .pin_d7 = 35,
    .pin_d6 = 34,
    .pin_d5 = 39,
    .pin_d4 = 36,
    .pin_d3 = 21,
    .pin_d2 = 19,
    .pin_d1 = 18,
    .pin_d0 = 5,
    .pin_vsync = 25,
    .pin_href = 23,
    .pin_pclk = 22,
    .xclk_freq_hz = 20000000,
    .ledc_timer = LEDC_TIMER_0,
    .ledc_channel = LEDC_CHANNEL_0,
    .pixel_format = PIXFORMAT_JPEG,
    .frame_size = FRAMESIZE_SVGA,
    .jpeg_quality = 12,
    .fb_count = 2
};

esp_err_t init_camera() {
    // Initialize the camera
    esp_err_t err = esp_camera_init(&camera_config);
    if (err != ESP_OK) {
        ESP_LOGE("CAMERA", "Camera init failed with error 0x%x", err);
        return err;
    }
    
    // Get camera sensor and set specific settings for OV2640
    sensor_t *s = esp_camera_sensor_get();
    s->set_framesize(s, FRAMESIZE_VGA);  // Set resolution to VGA
    
    ESP_LOGI("CAMERA", "Camera initialized successfully");
    return ESP_OK;
}