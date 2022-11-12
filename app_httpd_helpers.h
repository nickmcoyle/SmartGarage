#pragma once;

#include "Arduino.h"
#include "esp_http_server.h"
#include <tcpip_adapter.h>
#include <sys/socket.h>

//define 500 response for internal server error
esp_err_t httpd_resp_send_500(httpd_req_t *req, const char* errorMsg) {
  httpd_resp_set_type(req, "text/html");
  httpd_resp_set_hdr(req, "Content-Encoding", "text/html");
  return httpd_resp_send(req, errorMsg, strlen(errorMsg));
}

void print_client_ip(httpd_req_t *req)
{
  int sockfd = httpd_req_to_sockfd(req);
  char ipstr[INET6_ADDRSTRLEN];
  struct sockaddr_in6 addr;   // esp_http_server uses IPv6 addressing
  socklen_t addr_size = sizeof(addr);

  if (getpeername(sockfd, (struct sockaddr *)&addr, &addr_size) < 0) {
    ESP_LOGE(TAG, "Error getting client IP");
    return;
  }

  // Convert to IPv4 string
  inet_ntop(AF_INET, &addr.sin6_addr.un.u32_addr[3], ipstr, sizeof(ipstr));
  //ESP_LOGI(TAG, "Client IP => %s", ipstr);
  static char ip[64];
  char * p = ip;
  *p++ =  printf("Client IP: %s\n", ipstr);
  *p++ = 0;
  Serial.println(ip);
}

void print_server_ip() {
  tcpip_adapter_ip_info_t ipInfoSTA;
  tcpip_adapter_ip_info_t ipInfoAP;
  // IP address.
  tcpip_adapter_get_ip_info(TCPIP_ADAPTER_IF_STA, &ipInfoSTA);
  tcpip_adapter_get_ip_info(TCPIP_ADAPTER_IF_AP, &ipInfoAP);
  static char ip[64];
  char * p = ip;
  *p++ =  printf("Station IP: " IPSTR "\n", IP2STR(&ipInfoSTA.ip));
  *p++ =  printf("SoftAP IP: " IPSTR "\n", IP2STR(&ipInfoAP.ip));
  *p++ = 0;
  Serial.println(ip);
}

//finds the value part of a key value pair from data received by the web server
const char* getTagValue(const char* a_tag_list, const char* a_tag)
{
  static char result[64];
  size_t value_end;
  size_t value_start;
  String s(a_tag_list);
  value_start = s.indexOf(a_tag);
  if (value_start) {
    value_start += strlen(a_tag) + 1; //skip the ":" after the tag
    value_end = (s.indexOf(',', value_start) < s.length()) ? s.indexOf(',', value_start) : s.indexOf('}', value_start);
    //result = (char*)malloc(value_end - value_start + 1);
    s = s.substring(value_start, value_end-1);
    strcpy(result, s.c_str());    
    result[63] = '\0';    
  }  
  return result;
}
