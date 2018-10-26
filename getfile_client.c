/* TCP client that finds the time from a server */
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#define SIZE 1024
char buf[SIZE];

int main(int argc, char *argv[]) {
    int sockfd;
    int nread;
    struct sockaddr_in serv_addr;
    struct hostent * h;
    char done_buf[1024] = "done";

    // if (argc != 2) {
    //     fprintf(stderr, "usage: %s IPaddr\n", argv[0]);
    //     exit(1);
    // }

    if (argc != 4) { //gonna be like 4 or something
        fprintf(stderr, "arguments for %s: filename, serv_IPaddr, serv_port\n", argv[0]);
        exit(1);
    }

    //char file[] = argv[1];

    printf("creating endpoint\n"); //cwb debug
    fflush(stdout);
    /* create endpoint */
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror(NULL);
        exit(2);
    }

    printf("connecting to server\n"); //cwb debug
    fflush(stdout);
    /* connect to server */
    serv_addr.sin_family = AF_INET;
    h = gethostbyname(argv[2]);
    bcopy(h->h_addr, (char *)&serv_addr.sin_addr, h->h_length);
    serv_addr.sin_port = htons((int) strtol(argv[3], NULL, 10));
    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror(NULL);
        exit(3);
    }

    printf("reqeusting file %s\n", argv[1]);
    fflush(stdout);
    if (send(sockfd, argv[1], 7, 0) == -1){
        perror("send");
    }
    /* transfer data */
    while(1){
        nread = read(sockfd, buf, SIZE);
        // if(strcmp(buf, done_buf) != 0) { //this doesn't work
             write(1, buf, nread); // Writes to standard output
        // }
    }
    close(sockfd);
    exit(0);
}