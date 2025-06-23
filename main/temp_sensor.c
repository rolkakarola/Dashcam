#include "temp_sensor.h"
#include "owb/owb.h"
#include "owb/owb_rmt.h"
#include "ds18b20/ds18b20.h"

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define TAG "temp_sensor"

#define DS18B20_GPIO         15  // Adjust to your wiring
#define DS18B20_RESOLUTION   DS18B20_RESOLUTION_12_BIT

static DS18B20_Info *ds18b20_info;
static OneWireBus *owb;

esp_err_t init_temp_sensor(void)
{
    owb_rmt_driver_info rmt_driver_info;
    owb = owb_rmt_initialize(&rmt_driver_info, DS18B20_GPIO, RMT_CHANNEL_1, RMT_CHANNEL_0);
    if (!owb)
    {
        ESP_LOGE(TAG, "Failed to initialize OneWire bus");
        return ESP_FAIL;
    }

    owb_use_crc(owb, true);
    vTaskDelay(pdMS_TO_TICKS(2000));  // Let bus settle

    ds18b20_info = ds18b20_malloc();
    if (!ds18b20_info) return ESP_FAIL;

    ds18b20_init_solo(ds18b20_info, owb);
    ds18b20_use_crc(ds18b20_info, true);
    ds18b20_set_resolution(ds18b20_info, DS18B20_RESOLUTION);

    ESP_LOGI(TAG, "Temperature sensor initialized");
    return ESP_OK;
}

float get_temperature(void)
{
    float temperature = 0.0f;

    if (!ds18b20_info) return -999;

    ds18b20_convert_all(owb);
    ds18b20_wait_for_conversion(ds18b20_info);

    DS18B20_ERROR err = ds18b20_read_temp(ds18b20_info, &temperature);
    if (err != DS18B20_OK)
    {
        ESP_LOGW(TAG, "Temperature read failed");
        return -999;
    }

    ESP_LOGI(TAG, "Current Temperature: %.2f°C", temperature);
    return temperature;
}
