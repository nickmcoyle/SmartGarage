// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "esp_http_server.h"
#include "esp_timer.h"
#include "esp_camera.h"
#include "img_converters.h"
#include "camera_index.h"
#include "auth.h"
#include "web_update.h"
#include "success.h"
#include "Arduino.h"

#include "fb_gfx.h"
#include "fd_forward.h"
#include "dl_lib.h"
#include "fr_forward.h"

#include <sys/param.h> //has MIN function

#include <esp_ota_ops.h> //for over the air updates

#include "door_pin.h"

#ifdef __cplusplus
extern "C" {
#endif
uint8_t temprature_sens_read();
#ifdef __cplusplus
}
#endif
uint8_t temprature_sens_read();

uint8_t TEMPERATURE;
float TEMPERATURE_CALIBRATION = 0.56;

// include library to read and write from flash memory
#include <EEPROM.h>
// define the number of bytes you want to access
#define EEPROM_SIZE 4
const int COUNTER_ADDRESS = 0;
int BUTTON_COUNTER;

byte getButtonCounter() {
  EEPROM.get(COUNTER_ADDRESS, BUTTON_COUNTER);  
  return BUTTON_COUNTER;
}

void setButtonCounter() {
  BUTTON_COUNTER+=1;  
  EEPROM.put(COUNTER_ADDRESS, BUTTON_COUNTER);
  EEPROM.commit();   
}

void resetButtonCounter() {  
  BUTTON_COUNTER = 0;  
  EEPROM.put(COUNTER_ADDRESS, BUTTON_COUNTER);
  EEPROM.commit();   
}

void initEeprom() {
  // initialize EEPROM with predefined size
  EEPROM.begin(EEPROM_SIZE);
}

uint8_t getTemperature() {
  TEMPERATURE = temprature_sens_read();
  return TEMPERATURE * TEMPERATURE_CALIBRATION;
}
  
const int NUM_AUTH_USERNAMES = 3;
const char* AUTH_USERNAMES[] = {
  "Nick",
  "Joseph",
  "Katherine"
};
const char* AUTH_PASSWORD = "purple12";

typedef struct {
        size_t size; //number of values used for filtering
        size_t index; //current value index
        size_t count; //value count
        int sum;
        int * values; //array to be filled with values
} ra_filter_t;

typedef struct {
        httpd_req_t *req;
        size_t len;
} jpg_chunking_t;

#define PART_BOUNDARY "123456789000000000000987654321"
static const char* _STREAM_CONTENT_TYPE = "multipart/x-mixed-replace;boundary=" PART_BOUNDARY;
static const char* _STREAM_BOUNDARY = "\r\n--" PART_BOUNDARY "\r\n";
static const char* _STREAM_PART = "Content-Type: image/jpeg\r\nContent-Length: %u\r\n\r\n";

static ra_filter_t ra_filter;
httpd_handle_t stream_httpd = NULL;
httpd_handle_t camera_httpd = NULL;

static mtmn_config_t mtmn_config = {0};
static int8_t detection_enabled = 0;
static int8_t recognition_enabled = 0;
static int8_t is_enrolling = 0;
static face_id_list id_list = {0};

static ra_filter_t * ra_filter_init(ra_filter_t * filter, size_t sample_size){
    memset(filter, 0, sizeof(ra_filter_t));

    filter->values = (int *)malloc(sample_size * sizeof(int));
    if(!filter->values){
        return NULL;
    }
    memset(filter->values, 0, sample_size * sizeof(int));

    filter->size = sample_size;
    return filter;
}

static int ra_filter_run(ra_filter_t * filter, int value){
    if(!filter->values){
        return value;
    }
    filter->sum -= filter->values[filter->index];
    filter->values[filter->index] = value;
    filter->sum += filter->values[filter->index];
    filter->index++;
    filter->index = filter->index % filter->size;
    if (filter->count < filter->size) {
        filter->count++;
    }
    return filter->sum / filter->count;
}

static void rgb_print(dl_matrix3du_t *image_matrix, uint32_t color, const char * str){
    fb_data_t fb;
    fb.width = image_matrix->w;
    fb.height = image_matrix->h;
    fb.data = image_matrix->item;
    fb.bytes_per_pixel = 3;
    fb.format = FB_BGR888;
    fb_gfx_print(&fb, (fb.width - (strlen(str) * 14)) / 2, 10, color, str);
}

static int rgb_printf(dl_matrix3du_t *image_matrix, uint32_t color, const char *format, ...){
    char loc_buf[64];
    char * temp = loc_buf;
    int len;
    va_list arg;
    va_list copy;
    va_start(arg, format);
    va_copy(copy, arg);
    len = vsnprintf(loc_buf, sizeof(loc_buf), format, arg);
    va_end(copy);
    if(len >= sizeof(loc_buf)){
        temp = (char*)malloc(len+1);
        if(temp == NULL) {
            return 0;
        }
    }
    vsnprintf(temp, len+1, format, arg);
    va_end(arg);
    rgb_print(image_matrix, color, temp);
    if(len > 64){
        free(temp);
    }
    return len;
}

char* get_cookie_from_header(httpd_req_t *req) { 
    char*  buf;
    size_t buf_len;

    /* Get header value string length and allocate memory for length + 1,
     * extra byte for null termination */
    buf_len = httpd_req_get_hdr_value_len(req, "Cookie") + 1;
    if (buf_len > 1) {
        buf = (char*)malloc(buf_len);
        /* Copy null terminated value string into buffer */
        if (httpd_req_get_hdr_value_str(req, "Cookie", buf, buf_len) == ESP_OK) {
            ESP_LOGI(TAG, "Found header: %s", buf);
        }                
        //free(buf);
    }
    return buf;
}

const char* generate_cookie() {  
  const char* result = "sessId=e39g83qgdcoegbnrjf";  
  return result;
}

boolean is_authorized(httpd_req_t *req) {
  const char* cookie_from_header_char = get_cookie_from_header(req);  
  String cookie_from_header_string(cookie_from_header_char); 
  const char* cookie_from_server_char = generate_cookie();
  String cookie_from_server_string(cookie_from_server_char);
    
  return (-1 != cookie_from_header_string.indexOf(cookie_from_server_string));
}

static size_t jpg_encode_stream(void * arg, size_t index, const void* data, size_t len){
    jpg_chunking_t *j = (jpg_chunking_t *)arg;
    if(!index){
        j->len = 0;
    }
    if(httpd_resp_send_chunk(j->req, (const char *)data, len) != ESP_OK){
        return 0;
    }
    j->len += len;
    return len;
}

//define 401 response for unauthorized
esp_err_t httpd_resp_send_401(httpd_req_t *req) {
  httpd_resp_set_type(req, "text/html");
  httpd_resp_set_hdr(req, "Content-Encoding", "text/html");      
  return httpd_resp_send(req, (const char *)auth_html, auth_html_len);
}

//define 500 response for internal server error
esp_err_t httpd_resp_send_500(httpd_req_t *req, const char* errorMsg) {
  httpd_resp_set_type(req, "text/html");
  httpd_resp_set_hdr(req, "Content-Encoding", "text/html");      
  return httpd_resp_send(req, errorMsg, strlen(errorMsg));
}

static esp_err_t capture_handler(httpd_req_t *req){
    if(!is_authorized(req)) {
      return httpd_resp_send_401(req);
    }
    camera_fb_t * fb = NULL;
    esp_err_t res = ESP_OK;
    int64_t fr_start = esp_timer_get_time();

    fb = esp_camera_fb_get();
    if (!fb) {
        Serial.println("Camera capture failed");
        httpd_resp_send_500(req);
        return ESP_FAIL;
    }

    httpd_resp_set_type(req, "image/jpeg");
    httpd_resp_set_hdr(req, "Content-Disposition", "inline; filename=capture.jpg");

    size_t out_len, out_width, out_height;
    uint8_t * out_buf;
    bool s;
    bool detected = false;
    int face_id = 0;
    if(!detection_enabled || fb->width > 400){
        size_t fb_len = 0;
        if(fb->format == PIXFORMAT_JPEG){
            fb_len = fb->len;
            res = httpd_resp_send(req, (const char *)fb->buf, fb->len);
        } else {
            jpg_chunking_t jchunk = {req, 0};
            res = frame2jpg_cb(fb, 80, jpg_encode_stream, &jchunk)?ESP_OK:ESP_FAIL;
            httpd_resp_send_chunk(req, NULL, 0);
            fb_len = jchunk.len;
        }
        esp_camera_fb_return(fb);
        int64_t fr_end = esp_timer_get_time();
        Serial.printf("JPG: %uB %ums\n", (uint32_t)(fb_len), (uint32_t)((fr_end - fr_start)/1000));
        return res;
    }

    dl_matrix3du_t *image_matrix = dl_matrix3du_alloc(1, fb->width, fb->height, 3);
    if (!image_matrix) {
        esp_camera_fb_return(fb);
        Serial.println("dl_matrix3du_alloc failed");
        httpd_resp_send_500(req);
        return ESP_FAIL;
    }

    out_buf = image_matrix->item;
    out_len = fb->width * fb->height * 3;
    out_width = fb->width;
    out_height = fb->height;

    s = fmt2rgb888(fb->buf, fb->len, fb->format, out_buf);
    esp_camera_fb_return(fb);
    if(!s){
        dl_matrix3du_free(image_matrix);
        Serial.println("to rgb888 failed");
        httpd_resp_send_500(req);
        return ESP_FAIL;
    }

    box_array_t *net_boxes = face_detect(image_matrix, &mtmn_config);

    if (net_boxes){
        detected = true;
        
        free(net_boxes->box);
        free(net_boxes->landmark);
        free(net_boxes);
    }

    jpg_chunking_t jchunk = {req, 0};
    s = fmt2jpg_cb(out_buf, out_len, out_width, out_height, PIXFORMAT_RGB888, 90, jpg_encode_stream, &jchunk);
    dl_matrix3du_free(image_matrix);
    if(!s){
        Serial.println("JPEG compression failed");
        return ESP_FAIL;
    }

    int64_t fr_end = esp_timer_get_time();
    Serial.printf("FACE: %uB %ums %s%d\n", (uint32_t)(jchunk.len), (uint32_t)((fr_end - fr_start)/1000), detected?"DETECTED ":"", face_id);
    return res;
}

static esp_err_t stream_handler(httpd_req_t *req){
    if(!is_authorized(req)) {
      return httpd_resp_send_401(req);
    }
    camera_fb_t * fb = NULL;
    esp_err_t res = ESP_OK;
    size_t _jpg_buf_len = 0;
    uint8_t * _jpg_buf = NULL;
    char * part_buf[64];
    dl_matrix3du_t *image_matrix = NULL;
    bool detected = false;
    int face_id = 0;
    int64_t fr_start = 0;
    int64_t fr_ready = 0;
    int64_t fr_face = 0;
    int64_t fr_recognize = 0;
    int64_t fr_encode = 0;

    static int64_t last_frame = 0;
    if(!last_frame) {
        last_frame = esp_timer_get_time();
    }

    res = httpd_resp_set_type(req, _STREAM_CONTENT_TYPE);
    if(res != ESP_OK){
        return res;
    }

    while(true){
        detected = false;
        face_id = 0;
        fb = esp_camera_fb_get();
        if (!fb) {
            Serial.println("Camera capture failed");
            res = ESP_FAIL;
        } else {
            fr_start = esp_timer_get_time();
            fr_ready = fr_start;
            fr_face = fr_start;
            fr_encode = fr_start;
            fr_recognize = fr_start;
            if(!detection_enabled || fb->width > 400){
                if(fb->format != PIXFORMAT_JPEG){
                    bool jpeg_converted = frame2jpg(fb, 80, &_jpg_buf, &_jpg_buf_len);
                    esp_camera_fb_return(fb);
                    fb = NULL;
                    if(!jpeg_converted){
                        Serial.println("JPEG compression failed");
                        res = ESP_FAIL;
                    }
                } else {
                    _jpg_buf_len = fb->len;
                    _jpg_buf = fb->buf;
                }
            } else {

                image_matrix = dl_matrix3du_alloc(1, fb->width, fb->height, 3);

                if (!image_matrix) {
                    Serial.println("dl_matrix3du_alloc failed");
                    res = ESP_FAIL;
                } else {
                    if(!fmt2rgb888(fb->buf, fb->len, fb->format, image_matrix->item)){
                        Serial.println("fmt2rgb888 failed");
                        res = ESP_FAIL;
                    } else {
                        fr_ready = esp_timer_get_time();
                        box_array_t *net_boxes = NULL;
                        if(detection_enabled){
                            net_boxes = face_detect(image_matrix, &mtmn_config);
                        }
                        fr_face = esp_timer_get_time();
                        fr_recognize = fr_face;
                        if (net_boxes || fb->format != PIXFORMAT_JPEG){
                            if(net_boxes){
                                detected = true;
                                
                                fr_recognize = esp_timer_get_time();
                                
                                free(net_boxes->box);
                                free(net_boxes->landmark);
                                free(net_boxes);
                            }
                            if(!fmt2jpg(image_matrix->item, fb->width*fb->height*3, fb->width, fb->height, PIXFORMAT_RGB888, 90, &_jpg_buf, &_jpg_buf_len)){
                                Serial.println("fmt2jpg failed");
                                res = ESP_FAIL;
                            }
                            esp_camera_fb_return(fb);
                            fb = NULL;
                        } else {
                            _jpg_buf = fb->buf;
                            _jpg_buf_len = fb->len;
                        }
                        fr_encode = esp_timer_get_time();
                    }
                    dl_matrix3du_free(image_matrix);
                }
            }
        }
        if(res == ESP_OK){
            size_t hlen = snprintf((char *)part_buf, 64, _STREAM_PART, _jpg_buf_len);
            res = httpd_resp_send_chunk(req, (const char *)part_buf, hlen);
        }
        if(res == ESP_OK){
            res = httpd_resp_send_chunk(req, (const char *)_jpg_buf, _jpg_buf_len);
        }
        if(res == ESP_OK){
            res = httpd_resp_send_chunk(req, _STREAM_BOUNDARY, strlen(_STREAM_BOUNDARY));
        }
        if(fb){
            esp_camera_fb_return(fb);
            fb = NULL;
            _jpg_buf = NULL;
        } else if(_jpg_buf){
            free(_jpg_buf);
            _jpg_buf = NULL;
        }
        if(res != ESP_OK){
            break;
        }
        int64_t fr_end = esp_timer_get_time();

        int64_t ready_time = (fr_ready - fr_start)/1000;
        int64_t face_time = (fr_face - fr_ready)/1000;
        int64_t recognize_time = (fr_recognize - fr_face)/1000;
        int64_t encode_time = (fr_encode - fr_recognize)/1000;
        int64_t process_time = (fr_encode - fr_start)/1000;
        
        int64_t frame_time = fr_end - last_frame;
        last_frame = fr_end;
        frame_time /= 1000;
        uint32_t avg_frame_time = ra_filter_run(&ra_filter, frame_time);
        /*
        Serial.printf("MJPG: %uB %ums (%.1ffps), AVG: %ums (%.1ffps), %u+%u+%u+%u=%u %s%d\n",
            (uint32_t)(_jpg_buf_len),
            (uint32_t)frame_time, 1000.0 / (uint32_t)frame_time,
            avg_frame_time, 1000.0 / avg_frame_time,
            (uint32_t)ready_time, (uint32_t)face_time, (uint32_t)recognize_time, (uint32_t)encode_time, (uint32_t)process_time           
        );
        */
    }

    last_frame = 0;
    return res;
}

static esp_err_t status_handler(httpd_req_t *req){
    if(!is_authorized(req)) {
      return httpd_resp_send_401(req);
    }
    
    static char json_response[64];    
    char * p = json_response;
    *p++ = '{';    
    p+=sprintf(p, "\"temperature\":%u,", getTemperature());
    p+=sprintf(p, "\"button-counter\":%u", getButtonCounter());    
    *p++ = '}';
    *p++ = 0;
    httpd_resp_set_type(req, "application/json");    
    httpd_resp_set_hdr(req, "Content-Encoding", "application/json");
    httpd_resp_set_hdr(req, "Content-Type", "application/json");
    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
    return httpd_resp_send(req, json_response, strlen(json_response));
}

static esp_err_t index_handler(httpd_req_t *req){
    if(!is_authorized(req)) {
      return httpd_resp_send_401(req);
    }
    httpd_resp_set_type(req, "text/html");
    httpd_resp_set_hdr(req, "Content-Encoding", "text/html");    
    sensor_t * s = esp_camera_sensor_get();    
    return httpd_resp_send(req, (const char *)index_html, index_html_len);   
}

const char* getTagValue(const char* a_tag_list, const char* a_tag)
{   
    char* result = "";
    size_t value_end;
    size_t value_start;       
    String s(a_tag_list); 
    //Serial.print("list of tags: ");  
    //Serial.println(a_tag_list);
    //Serial.print("a_tag we r looking for: ");  
    //Serial.println(a_tag);
    //check if tag is found in the tag list      
    value_start = s.indexOf(a_tag);
     if (value_start) {
        value_start+= strlen(a_tag) + 1; //there is a ":" after the tag                            
        value_end = (s.indexOf(',', value_start) < s.length())? s.indexOf(',', value_start) : s.indexOf('}', value_start);         
        result = (char*)malloc(value_end - value_start + 1);
        s = s.substring(value_start, value_end);        
        strcpy(result, s.c_str());                             
     }   
    //Serial.println(result);
    return result;
}
static esp_err_t login_handler(httpd_req_t *req){
    bool auth = false;
    //buffer of 200 characters should be plenty for username password
    char content [200];
    
    size_t recv_size = req->content_len;    
    int ret = httpd_req_recv(req, content, recv_size);
    const char* key = "username";
    //parse tag value from JSON
    const char* userValue = getTagValue(content, key);  
    key = "password";
    //parse tag value from JSON
    const char* passValue = getTagValue(content, key);
    bool user_found = false;    
    for(int i = 0; i < NUM_AUTH_USERNAMES; ++i) {
      if(0 == strcmp(AUTH_USERNAMES[i], userValue)) {
        user_found = true;
      }
    }
    if(user_found && 0 == strcmp(passValue, AUTH_PASSWORD)) {        
      auth = true;          
    }
    Serial.print("auth: ");
    Serial.println((auth)? "true" : "false");
    
    if(auth) {
      const char* cookie = generate_cookie();
      httpd_resp_set_hdr(req, "Cache-Control", "no-cache");  
      httpd_resp_set_hdr(req, "Set-Cookie", cookie);      
      static char json_response[32];    
      char * p = json_response;
      *p++ = '{';    
      p+=sprintf(p, "\"authorized\":\"true\"");        
      *p++ = '}';
      *p++ = 0;
      httpd_resp_set_type(req, "application/json");
      httpd_resp_set_hdr(req, "Access-Control-Allow-Credentials", "true");
      httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
      return httpd_resp_send(req, json_response, strlen(json_response));
      
    } else {
      static char json_response[32];    
      char * p = json_response;
      *p++ = '{';    
      p+=sprintf(p, "\"authorized\":\"false\"");      
      *p++ = '}';
      *p++ = 0;
      httpd_resp_set_type(req, "application/json");      
      return httpd_resp_send(req, json_response, strlen(json_response));      
    }   
}

static esp_err_t door_handler(httpd_req_t *req){  
    if(!is_authorized(req)) {
      return httpd_resp_send_401(req);
    }  
    
    //increment the counter
    setButtonCounter();
    //open the door
    //esp32 board reverses this, LOW actually turns the relay on
    digitalWrite (DOOR_PIN, LOW);
    delay(1000);
    digitalWrite (DOOR_PIN, HIGH);
    static char json_response[64];
    
    char * p = json_response;
    *p++ = '{';    
    p+=sprintf(p, "\"temperature\":%u,", getTemperature());
    p+=sprintf(p, "\"button-counter\":%u", getButtonCounter());    
    *p++ = '}';
    *p++ = 0;
    httpd_resp_set_type(req, "application/json");
    httpd_resp_set_hdr(req, "Content-Encoding", "application/json");
    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
    return httpd_resp_send(req, json_response, strlen(json_response));
}

static esp_err_t resetcounter_handler(httpd_req_t *req){ 
    if(!is_authorized(req)) {
      return httpd_resp_send_401(req);
    } 
    
    //reset the counter to 0
    resetButtonCounter();
    static char json_response[64];
    char * p = json_response;
    *p++ = '{';    
    p+=sprintf(p, "\"temperature\":%u,", getTemperature());
    p+=sprintf(p, "\"button-counter\":%u", getButtonCounter());    
    *p++ = '}';
    *p++ = 0;   
    httpd_resp_set_type(req, "application/json");
    httpd_resp_set_hdr(req, "Content-Encoding", "application/json");
    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
    return httpd_resp_send(req, json_response, strlen(json_response));
}

/* 
 *  ESP32 partition table needs to have two partitions for bin images which are both big enough for the program
 *  usually called ota_0 and ota_1 and 1MB each
 *  on boot, code in one of the partitions is run. To perform an OTA update, we simply download a new image via WiFi
 *  and save it to the other image partition that is not booted from. If the download and writing to partition
 *  completes successfully, then we flag the partition with the newly written image as the one we wish to boot from.
 *  The configuration data which defines which partition we wish to use to load our application is known as the OTA 
 *  Data Partition and is of type (ESP_PARTITION_TYPE_DATA) with subtype ota (ESP_PARTITION_SUBTYPE_DATA_OTA) and 
 *  has size 0x2000 bytes(8192bytes).
 *  
 *  According to Kolban pg804 OTA works like this:
 *  1. call esp_ota_begin()
 *  2. receive data that is part of the image
 *  3. call esp_ota_write() to write the data we just read
 *  4. go back to 2 while there is still more of the image to receive over the network
 *  5. call esp_ota_end() to flag the end of the OTA image write
 *  6. call esp_ota_set_boot_partition() to specify which partition will be booted on 
 *      next reboot. This will update the OTA data partition which records such information.
 * */ 
static esp_err_t web_update_handler(httpd_req_t *req){  
    if(!is_authorized(req)) {
      return httpd_resp_send_401(req);
    }    
    
    httpd_resp_set_type(req, "text/html");
    httpd_resp_set_hdr(req, "Content-Encoding", "text/html");        
    return httpd_resp_send(req, (const char *)web_update_html, web_update_html_len);   
}

static esp_err_t web_update_file_handler(httpd_req_t *req){    
    if(!is_authorized(req)) {
      return httpd_resp_send_401(req);
    }

    char* server_response;
    httpd_resp_set_type(req, "text/html");
    httpd_resp_set_hdr(req, "Content-Encoding", "text/html");  
    
    int MAX_FILE_SIZE = (1000*1024); // 1MB because both ota partitions are only 1MB;
    int image_size = req->content_len;
    
    //output parameter from ota_begin();
    esp_ota_handle_t update_handle = 0; 
    
    //Get the currently running partition to pass to method to get next one.
    const esp_partition_t* current_partition = esp_ota_get_running_partition();
        
    //Get the next update partition that can be used as the target for a new OTA load and passed into esp_ota_begin().     
    const esp_partition_t* update_partition = esp_ota_get_next_update_partition(current_partition);
    if (update_partition == NULL) {
        server_response = "Passive OTA partition not found";        
        httpd_resp_send(req, server_response, strlen(server_response));
        return ESP_FAIL;
    }
    
    /* File cannot be larger than a limit */
    if (image_size > MAX_FILE_SIZE) {
        server_response = "File too large. Firmware image file size must be less than 1MB";
        httpd_resp_send(req, server_response, strlen(server_response));
        return ESP_FAIL;
    }
    /*
     * Begin the activity of flash/loading new image
     * must #include <esp_ota_ops.h>
     * esp_ota_begin(
     *  const esp_partition_t* partition //the partition to load with the new image
     *  size_t image_size //if size of new image is unknown, use OTA_SIZE_UNKNOWN
     *  esp_ota_handle_t* out_handle //the handle used for this OTA activity
     *   )
     */
    esp_err_t ota_begin_result = esp_ota_begin(update_partition, image_size, &update_handle);
    if (ota_begin_result != ESP_OK) {
        server_response = "esp_ota_begin failed";
        httpd_resp_send(req, server_response, strlen(server_response));
        return ESP_FAIL;
    }   
              
    //create a fixed buffer
    const int bufferSize = 256; //try not to exceed memory
    char* upgrade_data_buf = (char*)malloc(bufferSize);
    char* html_header_buffer = (char*)malloc(2);
    if (!upgrade_data_buf) {
        server_response = "Couldn't allocate memory to upgrade data buffer";
        httpd_resp_send(req, server_response, strlen(server_response));        
        return ESP_ERR_NO_MEM;
    }

    //success or failure flag from data read of http request
    int data_read = 0;
    int remaining = image_size;
    int recv_len = 0;
    //function scope response message for the write portion of the update    
    esp_err_t ota_write_result = ESP_OK;
       
    while (remaining > 0) {         
      //find how much to write to buffer
      recv_len = MIN(sizeof(upgrade_data_buf) - 1, remaining);   
      //write to buffer
      data_read = httpd_req_recv(req, upgrade_data_buf, recv_len); 
      //Serial.print("upgrade_data_buf: ");     
      //Serial.println(upgrade_data_buf);      
      if (data_read == 0) {
            ESP_LOGI(TAG, "Connection closed,all data received");   
            break;         
        }
        if (data_read < 0) {
            ESP_LOGE(TAG, "Error: SSL data read error");
            break;
        }
        if (data_read > 0) {                       
            ota_write_result = esp_ota_write(update_handle, (const void *)upgrade_data_buf, data_read);           
            remaining -= data_read;            
            if (ota_write_result != ESP_OK) {
              //this global response message will be sent to the client after leaving this loop
              Serial.print("ota_write_result error: ");
              Serial.println(ota_write_result);
              break;
            }
        }        
    }
    
    free(upgrade_data_buf);        
    Serial.println("data buffer was deallocated");
    
    //verifies the image and "finishes the update" although we still have to set the new boot partition
    esp_err_t ota_end_result = esp_ota_end(update_handle);
    if (ota_write_result != ESP_OK) {
        server_response = "Error: esp_ota_write failed!";
        httpd_resp_send(req, server_response, strlen(server_response));
        return ESP_FAIL;
    } else if (ota_end_result != ESP_OK) {
        server_response = "Error: esp_ota_end failed! Image is invalid";
        httpd_resp_send(req, server_response, strlen(server_response));
        return ESP_FAIL;
    }

    //set the boot partition to the one with the newly uploaded image
    esp_err_t ota_set_boot_result = esp_ota_set_boot_partition(update_partition);
    if (ota_set_boot_result != ESP_OK) {
        server_response = "esp_ota_set_boot_partition failed!";
        httpd_resp_send(req, server_response, strlen(server_response));
        return ESP_FAIL;
    }     
     
    server_response = "Update successful! Restarting in 5 seconds...";    
    httpd_resp_send(req, server_response, strlen(server_response));
    delay(5000);
    esp_restart();        
}

static esp_err_t restart_handler(httpd_req_t *req){
  if(!is_authorized(req)) {
      return httpd_resp_send_401(req);
  }
    httpd_resp_set_type(req, "text/html");
    httpd_resp_set_hdr(req, "Content-Encoding", "text/html");
    const char* response = "restarting....";      
    httpd_resp_send(req, response, strlen(response));   
    esp_restart();
    return ESP_OK;
}

void startCameraServer(){
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    
    httpd_uri_t index_uri = {
        .uri       = "/",
        .method    = HTTP_GET,
        .handler   = index_handler,
        .user_ctx  = NULL
    };

    httpd_uri_t login_uri = {
        .uri       = "/login",
        .method    = HTTP_POST,
        .handler   = login_handler,
        .user_ctx  = NULL
    };

    httpd_uri_t status_uri = {
        .uri       = "/status",
        .method    = HTTP_GET,
        .handler   = status_handler,
        .user_ctx  = NULL
    };

    httpd_uri_t web_update_uri = {
        .uri       = "/webupdate",
        .method    = HTTP_GET,
        .handler   = web_update_handler,
        .user_ctx  = NULL
    };

    httpd_uri_t update_uri = {
        .uri       = "/update",
        .method    = HTTP_POST,
        .handler   = web_update_file_handler,
        .user_ctx  = NULL
    };

    httpd_uri_t door_uri = {
        .uri       = "/door",
        .method    = HTTP_GET,
        .handler   = door_handler,
        .user_ctx  = NULL
    };

    httpd_uri_t resetcounter_uri = {
        .uri       = "/resetcounter",
        .method    = HTTP_GET,
        .handler   = resetcounter_handler,
        .user_ctx  = NULL
    };  

    httpd_uri_t capture_uri = {
        .uri       = "/capture",
        .method    = HTTP_GET,
        .handler   = capture_handler,
        .user_ctx  = NULL
    };

   httpd_uri_t stream_uri = {
        .uri       = "/stream",
        .method    = HTTP_GET,
        .handler   = stream_handler,
        .user_ctx  = NULL
    };   

    httpd_uri_t restart_uri = {
        .uri       = "/restart",
        .method    = HTTP_GET,
        .handler   = restart_handler,
        .user_ctx  = NULL
    };   

    initEeprom();

    ra_filter_init(&ra_filter, 20);
    
    mtmn_config.min_face = 80;
    mtmn_config.pyramid = 0.7;
    mtmn_config.p_threshold.score = 0.6;
    mtmn_config.p_threshold.nms = 0.7;
    mtmn_config.r_threshold.score = 0.7;
    mtmn_config.r_threshold.nms = 0.7;
    mtmn_config.r_threshold.candidate_number = 4;
    mtmn_config.o_threshold.score = 0.7;
    mtmn_config.o_threshold.nms = 0.4;
    mtmn_config.o_threshold.candidate_number = 1;    
    
    Serial.printf("Starting web server on port: '%d'\n", config.server_port);
    if (httpd_start(&camera_httpd, &config) == ESP_OK) {
        httpd_register_uri_handler(camera_httpd, &index_uri);
        httpd_register_uri_handler(camera_httpd, &login_uri);
        httpd_register_uri_handler(camera_httpd, &web_update_uri);
        httpd_register_uri_handler(camera_httpd, &update_uri);        
        httpd_register_uri_handler(camera_httpd, &status_uri);
        httpd_register_uri_handler(camera_httpd, &capture_uri);
        httpd_register_uri_handler(camera_httpd, &door_uri);
        httpd_register_uri_handler(camera_httpd, &resetcounter_uri);
        httpd_register_uri_handler(camera_httpd, &restart_uri);        
    }

    config.server_port += 1;
    config.ctrl_port += 1;
    Serial.printf("Starting stream server on port: '%d'\n", config.server_port);
    if (httpd_start(&stream_httpd, &config) == ESP_OK) {
        httpd_register_uri_handler(stream_httpd, &stream_uri);
    }
}
