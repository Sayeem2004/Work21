#include "pipe_networking.h"

void print_error(int err, char *msg) {
    if (err == -1) {
        // If Error Print Error Message and Errno
        printf("Error: %s\n", msg);
        printf("%s\n", strerror(errno));
    }
}

/*=========================
    server_handshake
    args: int * to_client

    Performs the client side pipe 3 way handshake.
    Sets *to_client to the file descriptor to the downstream pipe.

    returns the file descriptor for the upstream pipe.
=========================*/
int server_handshake(int *to_client) {
    // Making Well Known Pipe
    int err1 = mkfifo(WKP, 0644);
    if (err1 == -1) {
        print_error(-1, "Server: Unable To Make Well Known Pipe");
        return -1;
    } else printf("Server: Successfully Made Well Known Pipe\n");

    // Opening Well Known Pipe
    int read_file = open(WKP, O_RDONLY);
    if (read_file == -1) {
        print_error(-1, "Server: Unable To Open Well Known Pipe");
        return -1;
    } else printf("Server: Successfully Opened Well Known Pipe\n");

    // Receiving Message Through Well Known Pipe
    char *name = calloc(HANDSHAKE_BUFFER_SIZE, sizeof(char));
    int err2 = read(read_file, name, HANDSHAKE_BUFFER_SIZE);
    if (err2 == -1) {
        print_error(-1, "Server: Unable To Read Secret Message");
        return -1;
    } else printf("Server: Successfully Read Secret Message\n");

    // Opening Secret Pipe
    int write_file = open(name, O_WRONLY);
    if (write_file == -1) {
        print_error(-1, "Server: Unable To Open Secret Pipe");
        return -1;
    } else printf("Server: Successfully Opened Secret Pipe\n");

    // Removing Well Known Pipe
    int err3 = remove(WKP);
    if (err3 == -1) {
        print_error(-1, "Server: Unable To Remove Well Known Pipe");
        return -1;
    } else printf("Server: Successfully Removed Well Known Pipe\n");

    // Sending Verification Message
    int err4 = write(write_file, ACK, HANDSHAKE_BUFFER_SIZE);
    if (err4 == -1) {
        print_error(-1, "Server: Unable To Send Verification Message");
        return -1;
    } else printf("Server: Successfully Sent Verification Message\n");

    // Receiving Verification Message
    char *buff = calloc(HANDSHAKE_BUFFER_SIZE, sizeof(char));
    int err5 = read(read_file, buff, HANDSHAKE_BUFFER_SIZE);
    if (err5 == -1) {
        print_error(-1, "Server: Unable To Receive Verification Message");
        return -1;
    } else printf("Server: Successfully Received Verification Message\n");

    // Checking Verification Message
    if (strcmp(buff, ACK) != 0) {
        print_error(-1, "Server: Unable To Match Verification Message");
        return -1;
    } else printf("Server: Successfully Matched Verification Message\n");

    // Print that Process Is Finished
    printf("Server: Successfully Completed 3-Way Handshake\n");

    // Freeing Data
    free(name);
    free(buff);

    // Returning File Descriptors
    *to_client = write_file;
    int from_client = read_file;
    return from_client;
}

/*=========================
    client_handshake
    args: int * to_server

    Performs the client side pipe 3 way handshake.
    qSets *to_server to the file descriptor for the upstream pipe.

    returns the file descriptor for the downstream pipe.
=========================*/
int client_handshake(int *to_server) {
    // Creating Secret Pipe
    char *name = calloc(HANDSHAKE_BUFFER_SIZE, sizeof(char));
    sprintf(name, "%d", getpid());
    int err1 = mkfifo(name, 0644);
    if (err1 == -1) {
        print_error(-1, "Client: Unable To Make Secret Pipe");
        return -1;
    } else printf("Client: Successfully Made Secret Pipe\n");

    // Opening Well Known Pipe
    int write_file = open(WKP, O_WRONLY);
    if (write_file == -1) {
        print_error(-1, "Client: Unable To Open Well Known Pipe");
        return -1;
    } else printf("Client: Successfully Opened Well Known Pipe\n");

    // Sending Message Through Well Known Pipe
    int err2 = write(write_file, name, HANDSHAKE_BUFFER_SIZE);
    if (err2 == -1) {
        print_error(-1, "Client: Unable To Send Secret Pipe");
        return -1;
    } else printf("Client: Successfully Sent Secret Pipe\n");

    // Opening Secret Pipe
    int read_file = open(name, O_RDONLY);
    if (read_file == -1) {
        print_error(-1, "Client: Unable To Open Secret Pipe");
        return -1;
    } else printf("Client: Successfully Opened Secret Pipe\n");

    // Removing Secret Pipe
    int err3 = remove(name);
    if (err3 == -1) {
        print_error(-1, "Client: Unable To Remove Secret Pipe");
        return -1;
    } else printf("Client: Successfully Removed Secret Pipe\n");

    // Receiving Verification Message
    char *buff = calloc(HANDSHAKE_BUFFER_SIZE, sizeof(char));
    int err4 = read(read_file, buff, HANDSHAKE_BUFFER_SIZE);
    if (err4 == -1) {
        print_error(-1, "Client: Unable To Receive Verification Message");
        return -1;
    } else printf("Client: Successfully Received Verification Message\n");

    // Checking Verification Message
    if (strcmp(buff, ACK) != 0) {
        print_error(-1, "Client: Unable To Match Verification Message");
        return -1;
    } else printf("Client: Successfully Matched Verification Message\n");

    // Sending Verification Message
    int err5 = write(write_file, ACK, HANDSHAKE_BUFFER_SIZE);
    if (err5 == -1) {
        print_error(-1, "Client: Unable To Send Verification Message");
        return -1;
    } else printf("Client: Successfully Sent Verification Message\n");

    // Print that Process Is Finished
    printf("Client: Successfully Completed 3-Way Handshake\n");

    // Freeing Data
    free(name);
    free(buff);

    // Returning File Descriptors
    *to_server = write_file;
    int from_server = read_file;
    return from_server;
}
