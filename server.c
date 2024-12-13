#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include <math.h>   // For fabs()
#include "utils.h"

#define NUM_EXTERNALS 4  // Number of external processes

int *establishConnectionsFromExternalProcesses() {
    int socket_desc;
    static int client_socket[NUM_EXTERNALS];
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_size = sizeof(client_addr);

    socket_desc = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_desc < 0) {
        printf("Error creating socket\n");
        exit(1);
    }
    printf("Socket created successfully\n");

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(2000);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (bind(socket_desc, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        printf("Binding failed\n");
        exit(1);
    }
    printf("Binding successful\n");

    if (listen(socket_desc, NUM_EXTERNALS) < 0) {
        printf("Error while listening\n");
        exit(1);
    }
    printf("Listening for incoming connections...\n");

    for (int i = 0; i < NUM_EXTERNALS; i++) {
        client_socket[i] = accept(socket_desc, (struct sockaddr *)&client_addr, &client_size);
        if (client_socket[i] < 0) {
            printf("Connection failed\n");
            exit(1);
        }
        printf("External process connected from IP: %s, Port: %d\n", 
               inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
    }

    return client_socket;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: ./server <initial_central_temperature>\n");
        exit(1);
    }

    float centralTemp = atof(argv[1]);
    int *client_socket = establishConnectionsFromExternalProcesses();
    struct msg messageFromClient;
    float externalTemps[NUM_EXTERNALS];
    bool stable = false;

    while (!stable) {
        // Receive temperatures from all external processes
        for (int i = 0; i < NUM_EXTERNALS; i++) {
            if (recv(client_socket[i], &messageFromClient, sizeof(messageFromClient), 0) < 0) {
                printf("Error receiving from process %d\n", i);
                exit(1);
            }
            externalTemps[i] = messageFromClient.T;
            printf("Received temp from Process %d: %f\n", i, externalTemps[i]);
        }

        // Calculate the new central temperature
        float newCentralTemp = (2 * centralTemp + externalTemps[0] + externalTemps[1] +
                                externalTemps[2] + externalTemps[3]) / 6;

        // Check if the system has stabilized
        if (fabs(newCentralTemp - centralTemp) < 0.01) {
            stable = true;
            printf("System stabilized at temperature: %f\n", newCentralTemp);
        } else {
            centralTemp = newCentralTemp;  // Update the central temperature

            // Send the updated central temperature to all external processes
            struct msg updatedMessage = { .T = centralTemp, .Index = 0 };
            for (int i = 0; i < NUM_EXTERNALS; i++) {
                if (send(client_socket[i], &updatedMessage, sizeof(updatedMessage), 0) < 0) {
                    printf("Error sending to process %d\n", i);
                    exit(1);
                }
            }
        }
    }

    // Close all sockets after stabilization
    for (int i = 0; i < NUM_EXTERNALS; i++) {
        close(client_socket[i]);
    }

    printf("All connections closed. Server exiting.\n");
    return 0;
}
