#define _GNU_SOURCE
#include "httpserver.h" // Ensure this header has BUF_SIZE, BACKLOG, etc.
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>

#define PORTNUM 8080
#define BUF_SIZE 1024

char sensor_data[BUF_SIZE] = ""; // Buffer to store uploaded sensor data

void send_response(int cfd, const char *status, const char *content_type, const char *body) {
    char response[BUF_SIZE];
    snprintf(response, sizeof(response),
             "HTTP/1.1 %s\r\n"
             "Content-Type: %s\r\n"
             "Content-Length: %ld\r\n\r\n%s",
             status, content_type, strlen(body), body);
    send(cfd, response, strlen(response), 0);
}

void handle_request(int cfd) {
    char buffer[BUF_SIZE], method[16], path[256];

    int bytes_received = recv(cfd, buffer, sizeof(buffer) - 1, 0);
    if (bytes_received <= 0) {
        perror("Receive failed");
        return;
    }

    buffer[bytes_received] = '\0';
    sscanf(buffer, "%s %s", method, path);

    // Handle POST /upload
    if (strcmp(path, "/upload") == 0 && strcmp(method, "POST") == 0) {
        char *data_start = strstr(buffer, "\r\n\r\n");
        if (data_start) {
            data_start += 4;
            strncpy(sensor_data, data_start, BUF_SIZE - 1);
            sensor_data[BUF_SIZE - 1] = '\0';
            send_response(cfd, "200 OK", "text/plain", "Data uploaded successfully.");
            printf("SERVER: Received data: %s\n", sensor_data);
        } else {
            send_response(cfd, "400 Bad Request", "text/plain", "No data received.");
        }
    }
    // Handle GET /data
    else if (strcmp(path, "/data") == 0 && strcmp(method, "GET") == 0) {
        if (strlen(sensor_data) > 0) {
            send_response(cfd, "200 OK", "text/plain", sensor_data);
        } else {
            send_response(cfd, "204 No Content", "text/plain", "No data available.");
        }
    } else {
        send_response(cfd, "404 Not Found", "text/plain", "Invalid endpoint.");
    }
}

int main() {
    struct sockaddr_in server_addr, client_addr;
    int server_fd, cfd;
    socklen_t addr_len = sizeof(client_addr);

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORTNUM);

    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 10) == -1) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    printf("SERVER: Listening on port %d...\n", PORTNUM);

    while (1) {
        cfd = accept(server_fd, (struct sockaddr *)&client_addr, &addr_len);
        if (cfd == -1) {
            perror("Accept failed");
            continue;
        }
        handle_request(cfd);
        close(cfd);
    }

    close(server_fd);
    return 0;
}

