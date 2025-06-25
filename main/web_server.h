#ifndef WEB_SERVER_H_
#define WEB_SERVER_H_

#include "esp_http_server.h"

httpd_handle_t setup_server(void);
esp_err_t init_spiffs();

#endif
