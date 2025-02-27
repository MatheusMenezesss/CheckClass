#include "tcp_client.h"
#include "esp_log.h"

int TCPClient::s_Socket = 0;

bool TCPClient::Init(const char *host_ip, uint16_t port)
{
    struct sockaddr_in dest;
    lwip_inet_pton(AF_INET, host_ip, &dest.sin_addr);

    dest.sin_family = AF_INET;
    dest.sin_port = htons(port);

    s_Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    if (s_Socket < 0)
    {
        ESP_LOGE("TCP", "Nao foi possivel criar o socket\n");
        return false;
    }

    int err = connect(s_Socket, (struct sockaddr *)&dest, sizeof(dest));
    if (err != 0)
    {
        ESP_LOGE("TCP", "Nao foi possível conectar com o socket\n");
        return false;
    }

    return true;
}

size_t TCPClient::Send(const char *message)
{
    return send(s_Socket, message, strlen(message), 0);
}

size_t TCPClient::Receive(char *buffer, size_t size)
{
    return read(s_Socket, buffer, size);
}
