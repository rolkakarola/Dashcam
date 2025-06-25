#include "web_server.h"
#include "esp_camera.h"
#include "esp_http_server.h"
#include "esp_log.h"
#include "esp_timer.h"
#include <string.h>

// separator for streamed image parts
#define PART_BOUNDARY "123456789000000000000987654321"

static const char *TAG = "web_server";
// tells the web browser that the response will contain multiple parts (images)
static const char* _STREAM_CONTENT_TYPE = "multipart/x-mixed-replace;boundary=" PART_BOUNDARY;
// actual boundary marker that is sent between images
static const char* _STREAM_BOUNDARY = "\r\n--" PART_BOUNDARY "\r\n";
// header for each image
static const char* _STREAM_PART = "Content-Type: image/jpeg\r\nContent-Length: %u\r\n\r\n";

esp_err_t jpg_stream_httpd_handler(httpd_req_t *req) {
    camera_fb_t * fb = NULL;  // camera fame buffer
    esp_err_t res = ESP_OK;

    // Buffers to hold the JPEG data and HTTP headers for each frame
    size_t _jpg_buf_len;
    uint8_t * _jpg_buf;
    char part_buf[64];
    static int64_t last_frame = 0;

    // measuring frame time and calculating FPS
    if(!last_frame) {
        last_frame = esp_timer_get_time();
    }

    // Tells the browser that this is a multipart MJPEG stream
    res = httpd_resp_set_type(req, _STREAM_CONTENT_TYPE);
    if(res != ESP_OK){
        return res;
    }

    // continuously grabbing camera frames and sending them to the browser
    while(true){
        // capturing images from camera
        fb = esp_camera_fb_get(); // camera fame
        if (!fb) {
            ESP_LOGE(TAG, "Camera capture failed");
            res = ESP_FAIL;
            break;
        }

        // checking and converting from raw to JPEG
        if(fb->format != PIXFORMAT_JPEG){
            // 80 - quality level,  _jpg_buf - hold the JPEG data, _jpg_buf_len - size in bytes 
            bool jpeg_converted = frame2jpg(fb, 80, &_jpg_buf, &_jpg_buf_len);
            if(!jpeg_converted){ // If cinversion failes
                ESP_LOGE(TAG, "JPEG compression failed");
                esp_camera_fb_return(fb); // release the frame
                res = ESP_FAIL;
                break;
            }
        } else { // If frame is already JPEG
            _jpg_buf_len = fb->len;
            _jpg_buf = fb->buf;
        }

        if(res == ESP_OK)
            // It sends the boundary string to the client only if no errors happened before
            res = httpd_resp_send_chunk(req, _STREAM_BOUNDARY, strlen(_STREAM_BOUNDARY));
        if(res == ESP_OK){
            // it sends the HTTP header describing the next JPEG image chunk if no errors occurred before
            size_t hlen = snprintf(part_buf, sizeof(part_buf), _STREAM_PART, _jpg_buf_len);
            res = httpd_resp_send_chunk(req, part_buf, hlen);
        }
        if(res == ESP_OK)
            // It sends the raw JPEG image bytes over the HTTP connection
            res = httpd_resp_send_chunk(req, (const char *)_jpg_buf, _jpg_buf_len);

        // If the frame buffer's format is not already JPEG then free the allocated 
        // JPEG buffer _jpg_buf to avoid memory leaks
        if(fb->format != PIXFORMAT_JPEG){
            free(_jpg_buf);
        }
        esp_camera_fb_return(fb); // Return the frame buffer fb back to the camera drive

        if(res != ESP_OK){
            break;
        }

        int64_t fr_end = esp_timer_get_time(); // current time from ESP
        int64_t frame_time = fr_end - last_frame; // time elapsed since the last frame
        last_frame = fr_end;  // Updates last_frame to the current time
        frame_time /= 1000;  // μs -> ms
        ESP_LOGI(TAG, "MJPG: %uKB %ums (%.1ffps)", // info about current frame
            (uint32_t)(_jpg_buf_len/1024), // size kb
            // time to process the fame, frames per second
            (uint32_t)frame_time, 1000.0 / (uint32_t)frame_time); 
    }

    last_frame = 0; // resets last frame to 0
    return res; // returns result
}

httpd_handle_t setup_server(void) { //reference to the http running server
    // Loads the default HTTP server configuration (like port number, max connections, etc)
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    httpd_handle_t stream_httpd = NULL; // handle for the HTTP server

    httpd_uri_t uri_get = {
        .uri = "/", 
        .method = HTTP_GET,
        .handler = jpg_stream_httpd_handler,  // strams camera frames
        .user_ctx = NULL
    };

    if (httpd_start(&stream_httpd, &config) == ESP_OK) {
        httpd_register_uri_handler(stream_httpd, &uri_get);
    }

    return stream_httpd;
}