//Server

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#define MAX_SIZE 1024
#define TRUE 1
#define FALSE 0;
char buf[MAX_SIZE];
#define PORT 16200 //server will listen on this port

int sendall(int s, char *buf, int *len);

int main(int argc, char *argv[]) {
    int sockfd, client_sockfd, remote_sock, status;
    int nread, len, num_bytes;
    struct sockaddr_in serv_addr, client_addr, remote_serv_addr;
    struct hostent * h;
    FILE * fp;
    char file_buff[MAX_SIZE];
    char command[MAX_SIZE];
    int BAD_REQUEST = FALSE;

    if (argc != 3) {
        fprintf(stderr, "arguments for %s: word to replace, server port\n", argv[0]);
        exit(1);
    }
    int arg_length = strlen(argv[1]);

    //create censor array
    char censor_arr[arg_length+1];
    for(int i = 0; i < arg_length; i++){
        censor_arr[i] = '*';
    }

    censor_arr[arg_length] = '\0';

    // //cwb debug
    // char str[] = "This is a simple string simple simple simfs dsfa dsa fsad fsad simple";
    // char * pointer;
    // while((pointer = strstr (str,"simple")) != NULL){
    //     strncpy (pointer, "sample", 6);
    //     printf("modified string: %s", str);
    // }
    // //end cwb debug

    /* create endpoint */
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror(NULL);
        exit(2);
    }

    /* bind address */
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(PORT);
    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror(NULL);
        exit(3);
    }

    /* specify queue */
    len = sizeof(client_addr);

    printf("Waiting for connection...\n");
    fflush(stdout);
    listen(sockfd, 5);
    for (;;) { //analogous to while(true)
        client_sockfd = accept(sockfd, (struct sockaddr *)&client_addr, &len);
        printf("Connected to client at %u:%d\n", client_addr.sin_addr.s_addr, client_addr.sin_port);
        fflush(stdout);
        if (client_sockfd == -1) {
            fflush(stdout);
            perror(NULL);
            continue;
        }

        //read file name from client
        num_bytes = read(client_sockfd, buf, MAX_SIZE);
        buf[num_bytes] = '\0';

        fflush(stdout);

        printf("Downloading %s\n", buf);
        fflush(stdout);
        sprintf(command, "curl -s -o %s http://user.engineering.uiowa.edu/~jwryan/Communication_Networks/%s --fail", buf, buf);
        status = system(command);
        if (status != 0){
            printf("Bad Request\n");
            fflush(stdout);
            BAD_REQUEST = TRUE;
        }

        if(BAD_REQUEST) {
            char br_buf[] = "Bad Request\n";
            len = strlen(br_buf) + 1;
            write(client_sockfd, br_buf, len);
        }
        else {
            printf("Download complete\n");
            fflush(stdout);

            //read file
            fp = fopen(buf, "r"); //txt file name will come from client
            if(fp == NULL) {
                perror("Error opening file");
                return(-1);
            }

            while (fgets(file_buff, MAX_SIZE, fp) != NULL ) {
                char * pointer;
                char * censored_buf;
                sprintf(buf, "%s", file_buff);
                len = strlen(buf) + 1;
                //censored_buf = replaceWord(buf, argv[1], censor_arr);
                while((pointer = strstr (buf,argv[1])) != NULL){
                    strncpy (pointer, censor_arr, strlen(censor_arr));
                }
                // int send_status = sendall(client_sockfd, buf, len);
                // if (send_status == -1){ printf("send error"); }
                write(client_sockfd, buf, len);
            }
        }

        /* transfer data */
        printf("Closing Connection\n");
        fflush(stdout);
        close(client_sockfd);
        exit(0);
    }
}

int sendall(int s, char *buf, int *len)
{
    int total = 0;        // how many bytes we've sent
    int bytesleft = *len; // how many we have left to send
    int n;

    while(total < *len) {
        n = write(s, buf+total, bytesleft);
        if (n == -1) { break; }
        total += n;
        bytesleft -= n;
    }

    *len = total; // return number actually sent here

    return n==-1?-1:0; // return -1 on failure, 0 on success
} 