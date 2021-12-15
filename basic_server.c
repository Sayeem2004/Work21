#include "pipe_networking.h"

// Variable Declarations
char *buff;
int to_client;
int from_client;

void server_exit(int to_client, int from_client) {
    // Removing Well Known Pipe
    int err1 = remove(WKP);
    if (err1 == -1) {
        print_error(-1, "Server: Unable To Remove Well Known Pipe");
    } else printf("Server: Successfully Removed Well Known Pipe\n");

    // Closing Write File
    int err2 = close(to_client);
    if (err2 == -1) {
        print_error(-1, "Server: Unable To Close Write File");
    } else printf("Server: Successfully Closed Write File\n");

    // Closing Read File
    int err3 = close(from_client);
    if (err3 == -1) {
        print_error(-1, "Server: Unable To Close Read File");
    } else printf("Server: Successfully Closed Read File\n");

    // Exiting Function
    printf("Server: Successfully Shut Down\n");
    exit(0);
}

static void sighandler(int signo) {
    // SIGINT Case
    if (signo == SIGINT) {
        // Exiting Normally
        printf("\n");
        free(buff);
        server_exit(to_client, from_client);
    }

    // Exiting Function
    return;
}

int main() {
    // Signal Handling
    signal(SIGINT, sighandler);

    // Initializing Buffer
    buff = calloc(BUFFER_SIZE, sizeof(char));

    // Persistent Process
    while (1) {
        // Completing Handshake
        from_client = server_handshake(&to_client);
        printf("\n");

        // Constantly Runs
        while (1) {
            // Reading From User To Worker Pipe
            int err1 = read(from_client, buff, BUFFER_SIZE);
            if (err1 == -1) {
                print_error(-1, "Unable To Read Input From Client");
                continue;
            }
            if (err1 == 0) break;

            // String Manipulation (Upper Case)
            int i;
            for (i = 0; i < BUFFER_SIZE; i++) {
                if (buff[i] == '\0') break;
                buff[i] = toupper(buff[i]);
            }

            // Writing To Worker To User Pipe
            int err2 = write(to_client, buff, BUFFER_SIZE);
            if (err2 == -1) {
                print_error(-1, "Unable To Write Output To Client");
                continue;
            }
        }
    }

    // Exiting Normally
    printf("\n");
    free(buff);
    server_exit(to_client, from_client);
}
