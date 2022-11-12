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

#include "app_httpd.h"
#include "esp_http_server.h"
#include "esp_timer.h"
#include "app_httpd_helpers.h"
#include "app_index.h"
#include "app_log_view.h"
#include "app_web_update.h"
#include "app_settings.h"
#include "settings.h"
#include "success.h"
#include "Arduino.h"
#include "accessory_pins.h"
#include "PID.h"
#include "smart_outlet.h"
#include "temperature_sensor.h"
#include <sys/param.h> //has MIN function
#include <esp_ota_ops.h> //for over the air updates
#include <tcpip_adapter.h>
#include <sys/socket.h>

//server instance
httpd_handle_t server = NULL;

static esp_err_t index_handler(httpd_req_t *req) {
  //print_server_ip();
  //print_client_ip(req);
  httpd_resp_set_type(req, "text/html");
  httpd_resp_set_hdr(req, "Content-Encoding", "text/html");
  return httpd_resp_send(req, (const char *)index_html, index_html_len);
}

static esp_err_t status_handler(httpd_req_t *req) {
  static char json_response[64];
  char * p = json_response;
  *p++ = '{';
  p += sprintf(p, "\"app-name\":\"%s\",", settings::get_app_name()); //Kludge shouldnt be in this request but want to limit number of server calls
  p += sprintf(p, "\"temperature\":%f,", getFreshTemp());
  p += sprintf(p, "\"power-status\":\"%s\",", getOutletStatus()); //must wrap the String in quotes so it can be parsed by JSON on the client side
  p += sprintf(p, "\"run-time\":\"%s\"", getRuntimeString()); //must wrap the String in quotes so it can be parsed by JSON on the client side
  *p++ = '}';
  *p++ = 0;
  httpd_resp_set_type(req, "application/json");
  httpd_resp_set_hdr(req, "Content-Encoding", "application/json");
  httpd_resp_set_hdr(req, "Content-Type", "application/json");
  httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
  return httpd_resp_send(req, json_response, strlen(json_response));
}

static esp_err_t smart_outlet_handler(httpd_req_t *req) {
  //toggle the smart outlet
  togglePower();

  static char json_response[64];
  char * p = json_response;
  *p++ = '{';
  p += sprintf(p, "\"power-status\":\"%s\"", getOutletStatus()); //must wrap the String in quotes so it can be parsed by JSON on the client side
  *p++ = '}';
  *p++ = 0;
  httpd_resp_set_type(req, "application/json");
  httpd_resp_set_hdr(req, "Content-Encoding", "application/json");
  httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
  return httpd_resp_send(req, json_response, strlen(json_response));
}

static esp_err_t get_temperature_handler(httpd_req_t *req) {
  static char json_response[64];
  char * p = json_response;
  *p++ = '{';
  p += sprintf(p, "\"temperature\":%f", getFreshTemp());
  *p++ = '}';
  *p++ = 0;
  httpd_resp_set_type(req, "application/json");
  httpd_resp_set_hdr(req, "Content-Encoding", "application/json");
  httpd_resp_set_hdr(req, "Content-Type", "application/json");
  httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
  return httpd_resp_send(req, json_response, strlen(json_response));
}

static esp_err_t get_pid_values_handler(httpd_req_t *req) {
  static char json_response[64];
  char * p = json_response;
  *p++ = '{';
  p += sprintf(p, "\"target-temperature\":%f,", getTargetTemp());
  p += sprintf(p, "\"p-gain\":%f,", getPGain());
  p += sprintf(p, "\"i-gain\":%f,", getIGain());
  p += sprintf(p, "\"d-gain\":%f,", getDGain());
  p += sprintf(p, "\"windup-guard-gain\":%f", getWindupGuardGain());
  *p++ = '}';
  *p++ = 0;
  httpd_resp_set_type(req, "application/json");
  httpd_resp_set_hdr(req, "Content-Encoding", "application/json");
  httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");

  return httpd_resp_send(req, json_response, strlen(json_response));
}

static esp_err_t set_pid_values_handler(httpd_req_t *req) {
  //server response telling the user what happened
  static char json_response[64];
  char * p = json_response;
  httpd_resp_set_type(req, "application/json");
  httpd_resp_set_hdr(req, "Content-Encoding", "application/json");
  httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
  //start creating the response
  *p++ = '{';

  size_t recv_size = req->content_len;
  char content [recv_size];
  int ret = httpd_req_recv(req, content, recv_size);
  if (!ret) {
    p += sprintf(p, "\"error\":%s", "couldnt read data from http request set_pid_values");
    *p++ = '}';
    *p++ = 0;
    return httpd_resp_send(req, json_response, strlen(json_response));
  }

  //data binding
  const char* key = "targettemp";
  //parse tag value from JSON
  const char* targetTempValue = getTagValue(content, key);
  setTargetTemp(atof(targetTempValue));
  key = "pgain";
  //parse tag value from JSON
  const char* pgainValue = getTagValue(content, key);
  setPGain(atof(pgainValue));
  key = "igain";
  //parse tag value from JSON
  const char* igainValue = getTagValue(content, key);
  setIGain(atof(igainValue));
  key = "dgain";
  //parse tag value from JSON
  const char* dgainValue = getTagValue(content, key);
  setDGain(atof(dgainValue));
  key = "windupguard";
  //parse tag value from JSON
  const char* windupguardValue = getTagValue(content, key);
  setWindupGuardGain(atof(windupguardValue));

  p += sprintf(p, "\"success\":%s", "true");
  *p++ = '}';
  *p++ = 0;

  return httpd_resp_send(req, json_response, strlen(json_response));
}

static esp_err_t set_default_pid_values_handler(httpd_req_t *req) {
  initDefaultPIDValues();
  static char json_response[64];
  char * p = json_response;
  *p++ = '{';
  p += sprintf(p, "\"success\":%s", "true");
  *p++ = '}';
  *p++ = 0;
  httpd_resp_set_type(req, "application/json");
  httpd_resp_set_hdr(req, "Content-Encoding", "application/json");
  httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");

  return httpd_resp_send(req, json_response, strlen(json_response));
}

static esp_err_t get_settings_handler(httpd_req_t *req) {  
  static char json_response[128];
  char * p = json_response;  
  *p++ = '{';
  p += sprintf(p, "\"app-name\":\"%s\",", settings::get_app_name()); //must wrap the String in quotes so it can be parsed by JSON on the client side
  p += sprintf(p, "\"runtime-limit\":%d,", settings::get_runtime_limit());
  p += sprintf(p, "\"temperature-limit\":%f,", settings::get_temperature_limit());
  p += sprintf(p, "\"temperature-sensor-offset\":%f,", settings::get_temperature_sensor_offset());
  p += sprintf(p, "\"serial-graphing-enabled\":%d,", settings::get_serial_graphing_enabled());
  p += sprintf(p, "\"logging-enabled\":%d", settings::get_logging_enabled());
  *p++ = '}';
  *p++ = 0;
  httpd_resp_set_type(req, "application/json");
  httpd_resp_set_hdr(req, "Content-Encoding", "application/json");
  httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");

  return httpd_resp_send(req, json_response, strlen(json_response));  
}

static esp_err_t settings_handler(httpd_req_t *req) {  
  httpd_resp_set_type(req, "text/html");
  httpd_resp_set_hdr(req, "Content-Encoding", "text/html");
  return httpd_resp_send(req, (const char *)settings_html, settings_html_len);
}

static esp_err_t set_settings_handler(httpd_req_t *req) {
  //server response telling the user what happened
  static char json_response[64];
  char * p = json_response;
  httpd_resp_set_type(req, "application/json");
  httpd_resp_set_hdr(req, "Content-Encoding", "application/json");
  httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
  //start creating the response
  *p++ = '{';

  size_t recv_size = req->content_len;
  char content [recv_size];
  int ret = httpd_req_recv(req, content, recv_size);
  if (!ret) {
    p += sprintf(p, "\"error\":%s", "couldnt read data from http request set_settings");
    *p++ = '}';
    *p++ = 0;
    return httpd_resp_send(req, json_response, strlen(json_response));
  }

  //data binding
  const char* key = "appname";
  //parse tag value from JSON
  const char* app_name_str = getTagValue(content, key);
  settings::set_app_name(app_name_str);  
  
  key = "runtimelimit";
  //parse tag value from JSON
  const char* runtime_limit_str = getTagValue(content, key);
  settings::set_runtime_limit(atoi(runtime_limit_str));
  
  key = "temperaturelimit";
  //parse tag value from JSON
  const char* temperature_limit_str = getTagValue(content, key);
  settings::set_temperature_limit(atof(temperature_limit_str));

  key = "temperaturesensoroffset";
  //parse tag value from JSON
  const char* temperature_sensor_offset_str = getTagValue(content, key);
  settings::set_temperature_sensor_offset(atof(temperature_sensor_offset_str));
  
  key = "serialgraphingenabled";
  //parse tag value from JSON
  const char* serial_graphing_enabled_str = getTagValue(content, key);  
  bool vOut = (strcmp(serial_graphing_enabled_str, "true") == 0); //strcmp returns 0 when the strings match    
  settings::set_serial_graphing_enabled(vOut);  

  key = "loggingenabled";
  //parse tag value from JSON
  const char* logging_enabled_str = getTagValue(content, key);  
  vOut = (strcmp(logging_enabled_str, "true") == 0); //strcmp returns 0 when the strings match    
  settings::set_logging_enabled(vOut);    

  p += sprintf(p, "\"success\":%s", "true");
  *p++ = '}';
  *p++ = 0;

  return httpd_resp_send(req, json_response, strlen(json_response));
}

static esp_err_t set_default_settings_handler(httpd_req_t *req) {
  settings::set_defaults();
  static char json_response[64];
  char * p = json_response;
  *p++ = '{';
  p += sprintf(p, "\"success\":%s", "true");
  *p++ = '}';
  *p++ = 0;
  httpd_resp_set_type(req, "application/json");
  httpd_resp_set_hdr(req, "Content-Encoding", "application/json");
  httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");

  return httpd_resp_send(req, json_response, strlen(json_response));
}

static esp_err_t get_log_handler(httpd_req_t *req) {
  httpd_resp_set_type(req, "text/html");
  httpd_resp_set_hdr(req, "Content-Encoding", "text/html");

  //static buffer using the max amount of memory that could be in the log file
  //cant use regular char array because it's too large and causes stack overflow
  //dont use malloc because you can't free the memory after this function returns
  static char log_buf[(10*1024)+1];
  logger->readLogFile(log_buf);

  return httpd_resp_send(req, (const char *)log_buf, strlen(log_buf));
}

static esp_err_t view_log_handler(httpd_req_t *req) {
  httpd_resp_set_type(req, "text/html");
  httpd_resp_set_hdr(req, "Content-Encoding", "text/html");
  return httpd_resp_send(req, (const char *)view_log_html, view_log_html_len);
}

static esp_err_t clear_log_handler(httpd_req_t *req) {
  logger->clearLogFile();
  static char json_response[64];
  char * p = json_response;
  *p++ = '{';
  p += sprintf(p, "\"success\":%s", "true");
  *p++ = '}';
  *p++ = 0;
  httpd_resp_set_type(req, "application/json");
  httpd_resp_set_hdr(req, "Content-Encoding", "application/json");
  httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");

  return httpd_resp_send(req, json_response, strlen(json_response));
}

/*
    ESP32 partition table needs to have two partitions for bin images which are both big enough for the program
    usually called ota_0 and ota_1 and 1MB each
    on boot, code in one of the partitions is run. To perform an OTA update, we simply download a new image via WiFi
    and save it to the other image partition that is not booted from. If the download and writing to partition
    completes successfully, then we flag the partition with the newly written image as the one we wish to boot from.
    The configuration data which defines which partition we wish to use to load our application is known as the OTA
    Data Partition and is of type (ESP_PARTITION_TYPE_DATA) with subtype ota (ESP_PARTITION_SUBTYPE_DATA_OTA) and
    has size 0x2000 bytes(8192bytes).

    According to Kolban pg804 OTA works like this:
    1. call esp_ota_begin()
    2. receive data that is part of the image
    3. call esp_ota_write() to write the data we just read
    4. go back to 2 while there is still more of the image to receive over the network
    5. call esp_ota_end() to flag the end of the OTA image write
    6. call esp_ota_set_boot_partition() to specify which partition will be booted on
        next reboot. This will update the OTA data partition which records such information.
 * */
static esp_err_t web_updater_upload_page_handler(httpd_req_t *req) {
  httpd_resp_set_type(req, "text/html");
  httpd_resp_set_hdr(req, "Content-Encoding", "text/html");
  return httpd_resp_send(req, (const char *)web_update_html, web_update_html_len);
}

static esp_err_t web_updater_data_handler(httpd_req_t *req) {
  char* server_response;
  httpd_resp_set_type(req, "text/html");
  httpd_resp_set_hdr(req, "Content-Encoding", "text/html");

  int MAX_FILE_SIZE = (1500 * 1024); // 1.5MB
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
     Begin the activity of flash/loading new image
     must #include <esp_ota_ops.h>
     esp_ota_begin(
      const esp_partition_t* partition //the partition to load with the new image
      size_t image_size //if size of new image is unknown, use OTA_SIZE_UNKNOWN
      esp_ota_handle_t* out_handle //the handle used for this OTA activity
       )
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

static esp_err_t restart_handler(httpd_req_t *req) {
  char* server_response;
  httpd_resp_set_type(req, "text/html");
  httpd_resp_set_hdr(req, "Content-Encoding", "text/html");
  server_response = "Update successful! Restarting in 5 seconds...";
  httpd_resp_send(req, server_response, strlen(server_response));
  delay(5000);
  esp_restart();
}

/* Function for starting the webserver */
//extern httpd_handle_t start_webserver() {
extern void start_webserver() {
  httpd_config_t config = HTTPD_DEFAULT_CONFIG();
  config.max_uri_handlers = 24; //allow more uri endpoints on my server, the default is only 8
  // When LRU purge is enabled, if a client is requesting for connection but maximum number of sockets/sessions is reached,
  // then the session having the earliest LRU counter is closed automatically.
  config.lru_purge_enable = true; //Purge “Least Recently Used” connection  

  httpd_uri_t index_uri = {
    .uri       = "/",
    .method    = HTTP_GET,
    .handler   = index_handler,
    .user_ctx  = NULL
  };

  httpd_uri_t status_uri = {
    .uri       = "/status",
    .method    = HTTP_GET,
    .handler   = status_handler,
    .user_ctx  = NULL
  };

  httpd_uri_t smart_outlet_uri = {
    .uri       = "/toggle-power",
    .method    = HTTP_POST,
    .handler   = smart_outlet_handler,
    .user_ctx  = NULL
  };

  httpd_uri_t get_temperature_uri = {
    .uri       = "/get-temperature",
    .method    = HTTP_GET,
    .handler   = get_temperature_handler,
    .user_ctx  = NULL
  };

  httpd_uri_t get_pid_values_uri = {
    .uri       = "/get-pid-values",
    .method    = HTTP_GET,
    .handler   = get_pid_values_handler,
    .user_ctx  = NULL
  };

  httpd_uri_t set_pid_values_uri = {
    .uri       = "/set-pid-values",
    .method    = HTTP_POST,
    .handler   = set_pid_values_handler,
    .user_ctx  = NULL
  };

  httpd_uri_t set_default_pid_values_uri = {
    .uri       = "/set-default-pid-values",
    .method    = HTTP_POST,
    .handler   = set_default_pid_values_handler,
    .user_ctx  = NULL
  };

  httpd_uri_t settings_uri = {
    .uri       = "/settings",
    .method    = HTTP_GET,
    .handler   = settings_handler,
    .user_ctx  = NULL
  };

  httpd_uri_t get_settings_uri = {
    .uri       = "/get-settings",
    .method    = HTTP_GET,
    .handler   = get_settings_handler,
    .user_ctx  = NULL
  };

  httpd_uri_t set_settings_uri = {
    .uri       = "/set-settings",
    .method    = HTTP_POST,
    .handler   = set_settings_handler,
    .user_ctx  = NULL
  };

  httpd_uri_t set_default_settings_uri = {
    .uri       = "/set-default-settings",
    .method    = HTTP_POST,
    .handler   = set_default_settings_handler,
    .user_ctx  = NULL
  }; 
  
  httpd_uri_t get_log_uri = {
    .uri       = "/get-log",
    .method    = HTTP_GET,
    .handler   = get_log_handler,
    .user_ctx  = NULL
  };

  httpd_uri_t index_log_uri = {
    .uri       = "/view-log",
    .method    = HTTP_GET,
    .handler   = view_log_handler,
    .user_ctx  = NULL
  };

  httpd_uri_t clear_log_uri = {
    .uri       = "/clear-log",
    .method    = HTTP_POST,
    .handler   = clear_log_handler,
    .user_ctx  = NULL
  };

  httpd_uri_t web_updater_upload_page_uri = {
    .uri       = "/web-updater-upload-page",
    .method    = HTTP_GET,
    .handler   = web_updater_upload_page_handler,
    .user_ctx  = NULL
  };

  httpd_uri_t web_updater_uri = {
    .uri       = "/web-updater",
    .method    = HTTP_POST,
    .handler   = web_updater_data_handler,
    .user_ctx  = NULL
  };

  httpd_uri_t restart_uri = {
    .uri       = "/restart",
    .method    = HTTP_GET,
    .handler   = restart_handler,
    .user_ctx  = NULL
  };

  Serial.printf("Starting web server on port: '%d'\n", config.server_port);
  if (httpd_start(&server, &config) == ESP_OK) {
    httpd_register_uri_handler(server, &index_uri);
    httpd_register_uri_handler(server, &status_uri);
    httpd_register_uri_handler(server, &smart_outlet_uri);
    httpd_register_uri_handler(server, &get_temperature_uri);
    httpd_register_uri_handler(server, &get_pid_values_uri);
    httpd_register_uri_handler(server, &set_pid_values_uri);
    httpd_register_uri_handler(server, &set_default_pid_values_uri);
    httpd_register_uri_handler(server, &settings_uri);
    httpd_register_uri_handler(server, &get_settings_uri);
    httpd_register_uri_handler(server, &set_settings_uri);
    httpd_register_uri_handler(server, &set_default_settings_uri);    
    httpd_register_uri_handler(server, &get_log_uri);
    httpd_register_uri_handler(server, &index_log_uri);
    httpd_register_uri_handler(server, &clear_log_uri);
    httpd_register_uri_handler(server, &web_updater_upload_page_uri);
    httpd_register_uri_handler(server, &web_updater_uri);
    httpd_register_uri_handler(server, &restart_uri);
  }

  config.server_port += 1;
  config.ctrl_port += 1;

  //return server;
}

//Function for stopping the webserver
//extern void stop_webserver(httpd_handle_t server)
extern void stop_webserver()
{
    if (server) {
        //Stop the httpd server
        httpd_stop(server);
    }
}
