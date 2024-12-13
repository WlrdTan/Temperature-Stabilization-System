#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <math.h>  // Add this to avoid implicit declaration warning
#include "utils.h"

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: ./client <process_index> <initial_temperature>\n");
        exit(1);
    }

    int externalIndex = atoi(argv[1]);
    float externalTemp = atof(argv[2]);
    int socket_desc;
    struct sockaddr_in server_addr;
    struct msg message;

    // Create the socket
    socket_desc = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_desc < 0) {
        printf("Error creating socket\n");
        return -1;
    }

    // Configure the server address structure
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(2000);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Connect to the server
    if (connect(socket_desc, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        printf("Connection failed\n");
        return -1;
    }

    while (1) {
        // Send the external process's temperature to the server
        message = prepare_message(externalIndex, externalTemp);
        send(socket_desc, &message, sizeof(message), 0);

        // Receive the updated central temperature from the server
        recv(socket_desc, &message, sizeof(message), 0);
        float centralTemp = message.T;

        // Update the external process's temperature using the correct formula
        externalTemp = (3 * externalTemp + 2 * centralTemp) / 5;

        printf("Process %d: Updated temp = %f\n", externalIndex, externalTemp);

        // Check if the temperatures have stabilized
        if (fabs(externalTemp - centralTemp) < 0.01) {
            printf("Process %d: Temperature stabilized.\n", externalIndex);
            break;
        }
    }

    // Close the socket after stabilization
    close(socket_desc);
    return 0;
}
