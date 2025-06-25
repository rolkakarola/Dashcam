#include "camera_init.h"
#include "esp_camera.h"
#include "camera_pins.h"

#define CONFIG_XCLK_FREQ 20000000  // 20 MHz clock frequency for camera XCLK

esp_err_t init_camera(void)
{
    camera_config_t camera_config = {
        .pin_pwdn  = CAM_PIN_PWDN,   // Power down pin
        .pin_reset = CAM_PIN_RESET,  // Reset pin
        .pin_xclk = CAM_PIN_XCLK,    // External clock pin (XCLK)
        .pin_sccb_sda = CAM_PIN_SIOD,// SCCB (I2C) data pin
        .pin_sccb_scl = CAM_PIN_SIOC,// SCCB (I2C) clock pin

        .pin_d7 = CAM_PIN_D7,        // Data transfer pins (7 to 0)
        .pin_d6 = CAM_PIN_D6,
        .pin_d5 = CAM_PIN_D5,
        .pin_d4 = CAM_PIN_D4,
        .pin_d3 = CAM_PIN_D3,
        .pin_d2 = CAM_PIN_D2,
        .pin_d1 = CAM_PIN_D1,
        .pin_d0 = CAM_PIN_D0,

        .pin_vsync = CAM_PIN_VSYNC,  // Vertical sync pin
        .pin_href = CAM_PIN_HREF,    // Horizontal reference pin
        .pin_pclk = CAM_PIN_PCLK,    // Pixel clock pin

        .xclk_freq_hz = CONFIG_XCLK_FREQ, // Frequency for external clock to camera (20 MHz)
        .ledc_timer = LEDC_TIMER_0,        // LEDC timer for generating XCLK
        .ledc_channel = LEDC_CHANNEL_0,    // LEDC channel for generating XCLK

        // pixel format and frame size
        .pixel_format = PIXFORMAT_JPEG,   // Output image format (JPEG)
        .frame_size = FRAMESIZE_VGA,      // Frame resolution (VGA = 640x480)

        .jpeg_quality = 10,               // JPEG quality (lower = higher quality)

        // one buffer is allocated to store captured frames
        .fb_count = 1,                   // Number of frame buffers
        .grab_mode = CAMERA_GRAB_WHEN_EMPTY  // Frame grab mode
    };

    return esp_camera_init(&camera_config);  // Initialize camera with this config
}


