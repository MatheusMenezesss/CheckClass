#include "tcp_client.h"
#include "esp_log.h"

bool TCPClient::Init(const char *host_ip, uint16_t port)
{
    struct sockaddr_in dest;
    lwip_inet_pton(AF_INET, host_ip, &dest.sin_addr);

    dest.sin_family = AF_INET;
    dest.sin_port = htons(port);

    int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    if (sock < 0)
    {
        ESP_LOGE("TCP", "Nao foi possivel criar o socket\n");
        return false;
    }

    int err = connect(sock, (struct sockaddr *)&dest, sizeof(dest));
    if (err != 0)
    {
        ESP_LOGE("TCP", "Nao foi possível conectar com o socket\n");
        return false;
    }

    return true;
}
