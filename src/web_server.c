#include "esp_http_server.h"
#include "esp_camera.h"
#include "esp_log.h"

static const char *TAG = "WEB_SRV";

// Handler for /jpg endpoint
static esp_err_t jpg_handler(httpd_req_t *req) {
    camera_fb_t *fb = NULL;
    esp_err_t res = ESP_OK;
    
    fb = esp_camera_fb_get();
    if(!fb) {
        ESP_LOGE(TAG, "Camera capture failed");
        httpd_resp_send_500(req);
        return ESP_FAIL;
    }

    httpd_resp_set_type(req, "image/jpeg");
    httpd_resp_set_hdr(req, "Content-Disposition", "inline; filename=capture.jpg");
    res = httpd_resp_send(req, (const char *)fb->buf, fb->len);
    
    esp_camera_fb_return(fb);
    return res;
}

// Handler for root endpoint
static esp_err_t index_handler(httpd_req_t *req) {
    const char *html = 
        "<html><head><title>ESP32-CAM</title></head>"
        "<body><h1>ESP32-CAM Stream</h1>"
        "<img src='/jpg' width='640' height='480'></body></html>";
    
    httpd_resp_set_type(req, "text/html");
    return httpd_resp_send(req, html, strlen(html));
}

// URI Handlers
static const httpd_uri_t index_uri = {
    .uri       = "/",
    .method    = HTTP_GET,
    .handler   = index_handler,
    .user_ctx  = NULL
};

static const httpd_uri_t jpg_uri = {
    .uri       = "/jpg",
    .method    = HTTP_GET,
    .handler   = jpg_handler,
    .user_ctx  = NULL
};

httpd_handle_t start_webserver() {
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    httpd_handle_t server = NULL;
    
    if(httpd_start(&server, &config) == ESP_OK) {
        httpd_register_uri_handler(server, &index_uri);
        httpd_register_uri_handler(server, &jpg_uri);
        return server;
    }
    return NULL;
}
