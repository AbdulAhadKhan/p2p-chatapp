#include <arpa/inet.h>
#include <string.h>
#include <wait.h>
#include <sys/stat.h>

#include "utils.h"
#include "server-blue.h"
#include "ANSI-colors.h"

char * execute_command(char *command, int *read_size) {
    FILE *buffer_file;
    char *buffer = malloc(sizeof(char) * MESSAGE_SIZE);

    buffer_file = popen(command, "r");
    *read_size = fread(buffer, sizeof(char), MESSAGE_SIZE, buffer_file);
    buffer[*read_size] = '\0';
    
    return read_size == 0 ? " \r" : buffer;
}

void message_handler() {
    int read_size;
    char *message = malloc(sizeof(char) * MESSAGE_SIZE);

    while (recv(client_connection, message, 0, MSG_PEEK | MSG_DONTWAIT) != 0) {
        fgets(message, MESSAGE_SIZE, stdin);
        message = execute_command(message, &read_size);
        send(client_connection, message, read_size, 0);
    }
}

int server_blue_callback(void *args) {
    dup2(client_connection, 0);
    dup2(client_connection, 2);
    message_handler();    
    return 0;
}

int server_blue(socket_fd_t server_socket) {
    return start_server(server_socket, server_blue_callback, NULL);
}