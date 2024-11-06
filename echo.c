#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define BUFFER_SIZE 1000

void *handle_client(int *arg)
{
    int bytes_read, client_fd;
    char buffer[BUFFER_SIZE];
    client_fd = *(arg);
    free(arg);

    while (1)
    {
        bytes_read = read(client_fd, buffer, sizeof(buffer) - 1);
        if (bytes_read < 0)
        {
            perror("Read failed");
            break;
        }

        else if (bytes_read == 0)
        {
            printf("Client disconnected\n");
            break;
        }

        buffer[bytes_read] = '\0';
        write(client_fd, buffer, bytes_read);
    }
    close(client_fd);
}

int main(int argc, char **argv)
{
    if (argc != 3 || strcmp(argv[1], "-p") != 0)
    {
        fprintf(stderr, "Usage: %s -p <port>\n", argv[0]);
        return EXIT_FAILURE;
    }

    int server_fd, *client_fd;
    struct sockaddr_in addr;
    pthread_t thread_id;

    int port = atoi(argv[2]);

    if (port <= 0)
    {
        fprintf(stderr, "Invalid port number!");
        return EXIT_FAILURE;
    }

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    bind(server_fd, (struct sockaddr *)&addr, sizeof(addr));
    listen(server_fd, 1);

    while (1)
    {
        client_fd = malloc(sizeof(int));
        *client_fd = accept(server_fd, NULL, NULL);
        printf("Client connected\n");
        if (pthread_create(&thread_id, NULL, handle_client, client_fd) != 0)
        {
            perror("Thread creation failed");
            close(*client_fd);
            free(client_fd);
        }
        else
        {
            pthread_detach(thread_id);
        }
    }
    close(server_fd);
    return 0;
}