#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "simple_tls.h"

int main(void)
{
    int socket_desc , client_sock , read_size;
    socklen_t c;
    struct sockaddr_in server , client;
    char client_message[0xFFFF];
    const char msg[] = "HTTP/1.1 200 OK\r\nContent-type: text/plain\r\n\r\nClient Reception Confirmed";

    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
     
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(2000);
     
    bind(socket_desc,(struct sockaddr *)&server , sizeof(server));
     
    listen(socket_desc , 3);
     
    c = sizeof(struct sockaddr_in);

    unsigned int size;

    SSL *server_ctx = SSL_CTX_new(SSLv3_server_method());
    
    SSL_CTX_use_certificate_file(server_ctx, "testcert/fullchain.pem", SSL_SERVER_RSA_CERT);
    SSL_CTX_use_PrivateKey_file(server_ctx, "testcert/privkey.pem", SSL_SERVER_RSA_KEY);

    SSL_CTX_check_private_key(server_ctx);

    while (1) 
    {
        client_sock = accept(socket_desc, (struct sockaddr *)&client, &c);

        SSL *client = SSL_new(server_ctx);

        SSL_set_fd(client, client_sock);
        SSL_accept(client);
        
        printf("Cipher %s\n", tls_cipher_name(client));
        
        SSL_read(client, client_message, sizeof(client_message));
        printf("%s\n", client_message);
        SSL_write(client, msg, strlen(msg));
            
        SSL_shutdown(client);

        shutdown(client_sock, SHUT_RDWR);
        close(client_sock);
        
        SSL_free(client);
    }
    
    SSL_CTX_free(server_ctx);
    return 0;
}
