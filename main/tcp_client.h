#pragma once

#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "esp_netif.h"

class TCPClient
{
private:
    static int s_Socket;
    static fd_set s_ReadFds, s_WriteFds;
    static struct timeval s_Timeout;
public:
    static bool Init(const char *host_ip, uint16_t port);

    static size_t Send(const char *message);
    static size_t Receive(char *buffer, size_t size);
};