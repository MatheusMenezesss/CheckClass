#pragma once

#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "esp_netif.h"

class TCPClient
{
private:
public:
    static bool Init(const char *host_ip, uint16_t port);
};