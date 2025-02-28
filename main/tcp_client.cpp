#include "tcp_client.h"
#include "esp_log.h"

int TCPClient::s_Socket = 0;
fd_set TCPClient::s_ReadFds = {0}, TCPClient::s_WriteFds = {0};
struct timeval TCPClient::s_Timeout = (struct timeval){5, 0};

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
    int flags = fcntl(s_Socket, F_GETFL, 0);
    fcntl(s_Socket, F_SETFL, flags | O_NONBLOCK);

    int err = 0; 
    err = connect(s_Socket, (struct sockaddr *)&dest, sizeof(dest));
    if (err != 0 && errno != EINPROGRESS)
    {
        ESP_LOGE("TCP", "Nao foi possível conectar com o socket\n");
        close(s_Socket);
        return false;
    }

    FD_ZERO(&s_WriteFds);
    FD_ZERO(&s_ReadFds);
    FD_SET(s_Socket, &s_WriteFds);
    FD_SET(s_Socket, &s_ReadFds);

    // Aguarda até que o socket esteja pronto para escrita (conexão concluída)
    err = select(s_Socket + 1, &s_ReadFds, &s_WriteFds, NULL, &s_Timeout);

    if (err == 0)
    {
        // Timeout: a conexão não foi concluída no tempo especificado
        close(s_Socket);
        ESP_LOGE("TCP", "Timeout na conexão!\n");
        return false;
    } 
    else if (err < 0) 
    {
        // Erro no select()
        ESP_LOGE("TCP", "Erro no select");
        close(s_Socket);
        return false;
    }

    int error_code;
    socklen_t error_len = sizeof(error_code);
    getsockopt(s_Socket, SOL_SOCKET, SO_ERROR, &error_code, &error_len);

    if (error_code != 0) 
    {
        // Houve um erro durante a conexão
        ESP_LOGE("TCP", "Erro na conexão: %s\n", strerror(error_code));
        close(s_Socket);
        return false;
    }

    return true;
}

size_t TCPClient::Send(const char *message)
{
    FD_ZERO(&s_ReadFds);
    FD_SET(s_Socket, &s_ReadFds);

    int err = select(s_Socket + 1, &s_ReadFds, NULL, NULL, &s_Timeout);

    if (err == 0)
    {
        ESP_LOGE("TCP", "Timeout de envio\n");
        return 0;
    }
    else if (err < 0)
    {
        ESP_LOGE("TCP", "Erro no select do envio\n");
        return 0;
    }

    return send(s_Socket, message, strlen(message), 0);
}

size_t TCPClient::Receive(char *buffer, size_t size)
{
    FD_ZERO(&s_WriteFds);
    FD_SET(s_Socket, &s_WriteFds);

    int err = select(s_Socket + 1, NULL, &s_WriteFds, NULL, &s_Timeout);

    if (err == 0)
    {
        ESP_LOGE("TCP", "Timeout de leitura\n");
        return 0;
    }
    else if (err < 0)
    {
        ESP_LOGE("TCP", "Erro no select de leitura\n");
        return 0;
    }

    return read(s_Socket, buffer, size);
}
