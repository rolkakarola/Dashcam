#ifndef TEMP_SENSOR_H
#define TEMP_SENSOR_H

#include "esp_err.h"

esp_err_t init_temp_sensor(void);
float get_temperature(void);

#endif
