#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <fcntl.h>

#define TCP_PORT 5100
#define MAX_CLIENTS 10
#define BUFSIZE 1024

typedef struct {
    char type[10];  // Message type ("LOGIN", "MSG")
    char username[50];  // Username
    char content[256];  // Message content
} Message;

typedef struct {
    int sockfd;
    char username[50];
    pid_t pid;
} Client;

Client clients[MAX_CLIENTS];
int client_count = 0;
int pipe1[MAX_CLIENTS][2]; // 자식 -> 부모
int pipe2[MAX_CLIENTS][2]; // 부모 -> 자식

// Signal handler to clean up terminated child processes
void handle_sigchld(int sig) {
    int status;
    while (waitpid(-1, &status, WNOHANG) > 0) {}
}

// Broadcast message to all clients except the sender
void broadcast_message(Message *msg, int sender_index) {
    for (int i = 0; i < client_count; i++) {
        if (i != sender_index) {
            // Send message to child process via pipe
            write(pipe2[i][1], msg, sizeof(Message));
            // Notify child process to read the message
            kill(clients[i].pid, SIGUSR1);
        }
    }
}

// Parent reads from pipe1 (child -> parent) and broadcasts message to other clients
void handle_parent_read(int sender_index) {
    Message msg;
    read(pipe1[sender_index][0], &msg, sizeof(Message));
    printf("[PARENT] Received from client %d: [%s] %s\n", sender_index, msg.username, msg.content);
    broadcast_message(&msg, sender_index);
}

// Child reads from client socket and sends to parent via pipe
void handle_child_communication(int client_sock, int client_index) {
    Message msg;
    int n;
    while ((n = read(client_sock, &msg, sizeof(msg))) > 0) {
        printf("[CHILD] Received from client: [%s] %s\n", msg.username, msg.content);

        // Send the message to the parent
        write(pipe1[client_index][1], &msg, sizeof(msg));

        // Notify the parent to read the message
        kill(getppid(), SIGUSR1);

        // If the client sends 'q', terminate the connection
        if (strncmp(msg.content, "q", 1) == 0) {
            break;
        }
    }
    close(client_sock);
    exit(0);
}

// Signal handler for child processes to read the broadcasted message from parent
void handle_child_signal(int sig) {
    Message msg;
    int client_index = -1;

    for (int i = 0; i < client_count; i++) {
        if (clients[i].pid == getpid()) {
            client_index = i;
            break;
        }
    }

    if (client_index != -1) {
        read(pipe2[client_index][0], &msg, sizeof(Message));
        printf("[CHILD] Received broadcast message: [%s] %s\n", msg.username, msg.content);
        write(clients[client_index].sockfd, &msg, sizeof(Message));
    }
}

int main(int argc, char **argv) {
    signal(SIGCHLD, handle_sigchld);  // Handle child process termination
    signal(SIGUSR1, handle_child_signal);  // Handle message broadcast to children

    int ssock, csock;
    struct sockaddr_in servaddr, cliaddr;
    socklen_t clen = sizeof(cliaddr);

    // Create server socket
    if ((ssock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket()");
        return -1;
    }

    // Set up server address
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(TCP_PORT);

    // Bind and listen
    if (bind(ssock, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("bind()");
        return -1;
    }
    if (listen(ssock, MAX_CLIENTS) < 0) {
        perror("listen()");
        return -1;
    }

    printf("Server listening on port %d...\n", TCP_PORT);

    while (1) {
        csock = accept(ssock, (struct sockaddr *)&cliaddr, &clen);
        if (csock < 0) {
            perror("accept()");
            continue;
        }

        // Create pipes for communication between parent and child
        pipe(pipe1[client_count]);  // Child -> Parent
        pipe(pipe2[client_count]);  // Parent -> Child

        pid_t pid = fork();
        if (pid < 0) {
            perror("fork()");
            return -1;
        }

        if (pid == 0) {
            // In child process
            close(ssock);
            handle_child_communication(csock, client_count);
        } else {
            // In parent process
            clients[client_count].sockfd = csock;
            clients[client_count].pid = pid;
            client_count++;
            close(csock);
        }
    }

    close(ssock);
    return 0;
}

